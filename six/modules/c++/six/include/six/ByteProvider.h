/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __SIX_BYTE_PROVIDER_H__
#define __SIX_BYTE_PROVIDER_H__

#include <nitf/ImageBlocker.hpp>
#include <nitf/NITFBufferList.hpp>
#include <mem/SharedPtr.h>
#include <six/Container.h>
#include <six/NITFWriteControl.h>
#include <six/NITFSegmentInfo.h>
#include <six/XMLControlFactory.h>

namespace six
{
/*!
 * \class ByteProvider
 * \brief Used to provide corresponding raw NITF bytes (including NITF headers)
 * when provided with some AOI of the pixel data.  The idea is that if
 * getBytes() is called multiple times, eventually for the entire image, the
 * raw bytes provided back will be the entire NITF file.  This abstraction is
 * useful if separate threads, processes, or even machines have only portions of
 * the SICD/SIDD pixel data and are all trying to write out a single file; in
 * that scenario, this class provides all the raw bytes corresponding to the
 * caller's AOI, including NITF headers if necessary.  The caller does not need
 * to understand anything about the NITF file layout in order to write out the
 * file.  The bytes are intentionally provided back as a series of pointers
 * rather than one contiguous block of memory in order to minimize the number of
 * copies.
 */
class ByteProvider
{
public:
    /*!
     * \return The total number of bytes in the NITF
     */
    nitf::Off getFileNumBytes() const
    {
        return mFileNumBytes;
    }

    /*!
     * Given a range of rows from [startRow, startRow + numRows), provide the
     * number of bytes that will appear in the NITF on disk (including NITF
     * file header, image subheader(s), and DES subheader(s) and data).  Calling
     * this method repeatedly, eventually providing the entire range of the
     * image, will produce the total number of bytes in the full NITF.
     *
     * \param startRow The global start row in pixels as to where these pixels
     * are in the image.  If this is a multi-segment NITF, this is still simply
     * the global pixel location.
     * \param numRows The number of rows.
     *
     * \return The associated number of bytes in the NITF
     */
    nitf::Off getNumBytes(size_t startRow, size_t numRows) const;

    /*!
     * The caller provides an AOI of the pixel data.  This method provides back
     * a list of contiguous buffers corresponding to the raw NITF bytes for
     * this portion of the file.  If this AOI is in the middle of an image
     * segment, this will be simply a buffer list of length 1 consisting of the
     * input pointer (no copy occurs).  Otherwise, pointers to various headers
     * (file header, image subheader(s), DES subheader and data) will
     * be in the buffer list before, in the middle, and/or after the image
     * data.  If this method is called multiple times with AOIs that
     * eventually consist of the entire image, and the raw bytes are written
     * out to disk in order with respect to the start pixel rows this method is
     * called with (or out of order but seeking to the provided file offset
     * each time), and in the order contained in the buffer list, it will form
     * a valid NITF.
     *
     * \note This method does not perform byte swapping on the pixel data for
     * efficiency reasons, but NITFs are written out in big endian order.  This
     * means that on a little endian system, you must byte swap the pixel data
     * prior to calling this method.
     *
     * \note This method does not perform any blocking on the pixel data.  If
     * the NITF is blocked, the pixel data must already be in a contiguous
     * blocked memory layout, including pad pixels for partial blocks.
     *
     * \param imageData The image data pixels to write.  The underlying type
     * will be complex short or complex float based on the complex data sent
     * in during initialize.  Must be in big endian order and blocked if the
     * NITF is.
     * \param startRow The global start row in pixels as to where these pixels
     * are in the image.  If this is a multi-segment NITF, this is still simply
     * the global pixel location.
     * \param numRows The number of rows in the provided 'imageData'
     * \param[out] fileOffset The offset in bytes in the NITF where these
     * buffers should be written
     * \param[out] buffers One or more pointers to raw bytes of data.  These
     * should be written out in the order they are provided in the buffer list.
     * The pointers point to the provided pixel data and, if required, one or
     * more NITF headers.  No copies occur, so these buffers are only valid for
     * the lifetime of this provider object (since this object owns the raw
     * bytes for the NITF headers) and the lifetime of the passed-in image
     * data.
     */
    void getBytes(const void* imageData,
                  size_t startRow,
                  size_t numRows,
                  nitf::Off& fileOffset,
                  nitf::NITFBufferList& buffers) const;

    /*!
     * \return ImageBlocker with settings in sync with how the image will be
     * blocked in the NITF
     */
    std::auto_ptr<const nitf::ImageBlocker> getImageBlocker() const;

protected:
    /*!
     * Constructor
     *
     * \param schemPaths Directories or files of schema locations
     * \param maxProductSize The max number of bytes in an image segment.
     * By default this is set automatically for you based on NITF file rules.
     */
    ByteProvider();

    /*!
     * Initialize the byte provider.  Must be called in the constructor of
     * inheriting classes
     *
     * \param container Container initialized with all associated data
     * \param xmlRegistry XML registry
     * \param schemaPaths Directories or files of schema locations
     * \param maxProductSize The max number of bytes in an image segment.
     * \param numRowsPerBlock The number of rows per block.  Only applies for
     * SIDD.  Defaults to no blocking.
     * \param numColsPerBlock The number of columns per block.  Only applies
     * for SIDD.  Defaults to no blocking.
     */
    void initialize(mem::SharedPtr<Container> container,
                    const XMLControlRegistry& xmlRegistry,
                    const std::vector<std::string>& schemaPaths,
                    size_t maxProductSize,
                    size_t numRowsPerBlock = 0,
                    size_t numColsPerBlock = 0);

    /*!
     * Initialize the byte provider.  Must be called in the constructor of
     * inheriting classes
     *
     * \param writer Writer.  Must be initialized via
     * NITFWriteControl::initialize() and have all desired product size and
     * blocking values set.
     * \param schemaPaths Directories or files of schema locations
     */
    void initialize(const NITFWriteControl& writer,
                    const std::vector<std::string>& schemaPaths);

private:
    void checkBlocking(size_t seg,
                       size_t startGlobalRowToWrite,
                       size_t numRowsToWrite) const;

private:
    size_t mNumCols;
    size_t mNumRowsPerBlock;
    size_t mNumColsPerBlock;
    size_t mNumBytesPerRow;

    std::vector<sys::byte> mFileHeader;
    std::vector<std::vector<sys::byte> > mImageSubheaders;
    std::vector<sys::byte> mDesSubheaderAndData;
    std::vector<nitf::Off> mImageSubheaderFileOffsets;
    std::vector<NITFSegmentInfo> mImageSegmentInfo;
    nitf::Off mDesSubheaderFileOffset;
    nitf::Off mFileNumBytes;
};
}

#endif
