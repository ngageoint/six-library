/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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

#ifndef __SIX_SICD_BYTE_PROVIDER_H__
#define __SIX_SICD_BYTE_PROVIDER_H__

#include <vector>

#include <six/sicd/ComplexData.h>

namespace six
{
namespace sicd
{
/*!
 * \class SICDByteProvider
 * \brief Used to provide corresponding raw NITF bytes (including NITF headers)
 * when provided with some AOI of the pixel data.  The idea is that if
 * getBytes() is called multiple times, eventually for the entire image, the
 * raw bytes provided back will be the entire NITF file.  This abstraction is
 * useful if separate threads, processes, or even machines have only portions of
 * the SICD pixel data and are all trying to write out a single file; in that
 * scenario, this class provides a contiguous chunk of memory corresponding to
 * the caller's AOI, including NITF headers if necessary.  The caller does not
 * need to understand anything about the NITF file layout in order to write out
 * the file.
 */
class SICDByteProvider
{
public:
    /*!
     * Constructor
     *
     * \param data Representation of the complex data
     * \param schemPaths Directories or files of schema locations
     */
    SICDByteProvider(const ComplexData& data,
                     const std::vector<std::string>& schemaPaths);

    /*!
     * \return The total number of bytes in the NITF
     */
    nitf::Off getFileNumBytes() const
    {
        return mFileNumBytes;
    }

    /*!
     * The caller provides an AOI of the pixel data.  This method provides back
     * a contiguous buffer corresponding to the raw NITF bytes for this portion
     * of the file.  If this AOI is in the middle of an image segment, the
     * input pointer is simply returned (no copy occurs).  Otherwise, various
     * headers (file header, image subheader(s), DES subheader and data) will
     * be copied before, in the middle, and/or after the image data in the
     * returned pointer.  If this method is called multiple times with AOIs that
     * eventually consist of the entire image, and the raw bytes are written
     * out to disk in order with respect to the start pixel rows this method is
     * called with, it will form a valid NITF.
     *
     * \note This method does not perform byte swapping on the pixel data for
     * efficiency reasons, but NITFs are written out in big endian order.  This
     * means that on a little endian system, you must byte swap the pixel data
     * prior to calling this method.
     *
     * \note This method is not thread-safe (due to reusing an internal buffer
     * to avoid repeated memory allocations).  However, this method can be
     * called any number of times in any order with respect to the start row.
     *
     * \param imageData The image data pixels to write.  The underlying type
     * will be complex short or complex float based on the complex data sent
     * in during initialize.  Must be in big endian order.
     * \param startRow The global start row in pixels as to where these pixels
     * are in the image.  If this is a multi-segment NITF, this is still simply
     * the global pixel location.
     * \param numRows The number of rows in the provided 'imageData'
     * \param[out] numBytes The number of bytes associated with the returned
     * pointer (this may be more than the size of the imagery as NITF headers
     * may be added)
     *
     * \return A pointer to a contiguous memory location which will include
     * the pixel data and, if required, one or more NITF headers.  This pointer
     * is only guaranteed to remain valid until the next call to getBytes() and
     * must also not be used once this class goes out of scope.
     */
    const void* getBytes(const void* imageData,
                         size_t startRow,
                         size_t numRows,
                         size_t& numBytes) const;

private:
    const size_t mNumBytesPerRow;

    std::vector<sys::byte> mFileHeader;
    std::vector<std::vector<sys::byte> > mImageSubheaders;
    std::vector<sys::byte> mDesSubheaderAndData;
    std::vector<nitf::Off> mImageSubheaderFileOffsets;
    std::vector<NITFSegmentInfo> mImageSegmentInfo;
    nitf::Off mDesSubheaderFileOffset;
    nitf::Off mFileNumBytes;

    mutable std::vector<sys::byte> mBuffer;
};
}
}

#endif
