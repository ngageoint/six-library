/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_COMPRESSED_BYTE_PROVIDER_HPP__
#define __NITF_COMPRESSED_BYTE_PROVIDER_HPP__
#pragma once

#include <vector>
#include <utility>
#include <memory>

#include <nitf/coda-oss.hpp>
#include <nitf/ByteProvider.hpp>
#include <nitf/System.hpp>
#include <nitf/Record.hpp>
#include <nitf/ImageBlocker.hpp>
#include <nitf/NITFBufferList.hpp>
#include <nitf/ImageSegmentComputer.h>

namespace nitf
{
/*!
 * \class CompressedByteProvider
 * \brief Used to provide corresponding raw NITF bytes (including NITF headers)
 * when provided with some AOI of the compressed pixel data. See docstring
 * of parent class for details.
 * Limitations:
 * - Graphics, labels, texts, and reserved extensions are not supported
 * - Image segments and data extension segments are supported.
 * - TREs are supported in the file header and/or image subheaders in either
 * the user-defined or extended sections
 * - Multiple image segments are supported, but only where the images are
 * intended to be vertically stacked (so the number of columns must match and
 * all indexing will be as if the image segments are intended to be vertically
 * stacked in order.  ILOC and IALVL are not checked).
 *
 * The NITF layout is described in ByteProvider.hpp
 */
class CompressedByteProvider : public ByteProvider
{
public:

    /*!
     * \param record Pre-populated NITF record.  All TREs, image subheader, and
     * DES subheader information must be filled out.  Record won't be modified.
     * \param bytesPerBlock A vector for each image segment. Each inner vector
     *        contains the compressed size for each block in the segment,
     *        in bytes.
     * \param desData Optional DES data (one per DES subheader).  These are
     * pointers to the raw DES binary data itself (just data, not subheader).
     * \param numRowsPerBlock The number of rows per block.  Defaults to no
     * blocking.
     * \param numColsPerBlock The number of columns per block.  Defaults to no
     * blocking.
     */
    CompressedByteProvider(Record& record,
            const std::vector<std::vector<size_t> >& bytesPerBlock,
            const std::vector<PtrAndLength>& desData =
                    std::vector<PtrAndLength>(),
            size_t numRowsPerBlock = 0,
            size_t numColsPerBlock = 0);

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
    nitf::Off getNumBytes(size_t startRow, size_t numRows) const override;

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
     * after compression will be a single byte per pixel.
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
                          NITFBufferList& buffers) const override;

protected:
    /*!
     * Default constructor.  Expectation is that if an inheriting class uses
     * this constructor, the inheriting class will call initialize() later in
     * its constructor.
     */
    CompressedByteProvider() = default;

    /*!
     * \param record Pre-populated NITF record.  All TREs, image subheader, and
     * DES subheader information must be filled out.  Record won't be modified.
     * \param desData Optional DES data (one per DES subheader).  These are
     * pointers to the raw DES binary data itself (just data, not subheader).
     * \param numRowsPerBlock The number of rows per block.  Defaults to no
     * blocking.
     * \param numColsPerBlock The number of columns per block.  Defaults to no
     * blocking.
     */
    void initialize(const Record& record,
            const std::vector<std::vector<size_t> >& bytesPerBlock,
            const std::vector<PtrAndLength>& desData =
                    std::vector<PtrAndLength>(),
            size_t numRowsPerBlock = 0,
            size_t numColsPerBlock = 0);

    size_t countBytesForCompressedImageData(
            size_t seg, size_t startRow, size_t numRowsToWrite) const;

    size_t addImageData(
            size_t seg,
            size_t startRow,
            size_t numRowsToWrite,
            const sys::byte* imageData,
            nitf::Off& fileOffset,
            NITFBufferList& buffers) const;
    size_t addImageData(
            size_t seg,
            size_t startRow,
            size_t numRowsToWrite,
            const std::byte* imageData,
            nitf::Off& fileOffset,
            NITFBufferList& buffers) const;

private:
    types::Range findBlocksToWrite(size_t seg, size_t globalStartRow,
            size_t numRowsToWrite) const;

private:
    std::vector<std::vector<size_t> > mBytesInEachBlock;
};
}

#endif
