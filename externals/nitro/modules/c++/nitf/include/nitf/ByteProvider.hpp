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

#include <vector>
#include <utility>
#include <memory>

#include <sys/Conf.h>
#include <nitf/System.hpp>
#include <nitf/Record.hpp>
#include <nitf/ImageBlocker.hpp>
#include <nitf/NITFBufferList.hpp>
#include <nitf/ImageSegmentComputer.h>

namespace nitf
{
// For multi-segment cases, only handles vertical stacking
// where # cols matches (checked)
// # bytes/pixel matches (checked)
// Handles image segments and DESs
// Does not handle graphics or text

/*!
 * Provides the contents of all of the NITF headers as well as their offsets
 * in the file.  Does not handle graphics or text segments.
 *
 * The NITF layout is
 * (lastSeg = mImageSubheaderFileOffsets.size() - 1):
 *
 * Offset 0
 * ========
 * fileHeader
 *
 * Offset mImageSubheaderFileOffsets[0]
 * ===================================
 * mImageSubheaders[0]
 *
 * Offset mImageSubheaderFileOffsets[0] + mImageSubheaders[0].size()
 * ===============================================================
 * <Raw pixel data for first image segment.  Must be written in Big Endian
 * format.>
 *
 * ...
 *
 * Offset mImageSubheaderFileOffsets[lastSeg]
 * ===================================
 * mImageSubheaders[lastSeg]
 *
 * Offset mImageSubheaderFileOffsets[lastSeg] + mImageSubheaders[lastSeg].size()
 * ===========================================================================
 * <Raw pixel data for last image segment.  Must be written in Big Endian
 * format.>
 *
 * Offset desSubheaderFileOffset
 * =============================
 * desSubheaderAndData
 *
 */

class ByteProvider
{
public:
    typedef std::pair<const void*, size_t> PtrAndLength;

    ByteProvider(Record& record,
                 const std::vector<PtrAndLength>& desData =
                            std::vector<PtrAndLength>(),
                    size_t numRowsPerBlock = 0,
                    size_t numColsPerBlock = 0);

    virtual ~ByteProvider();

    /*!
     * \return The total number of bytes in the NITF
     */
    nitf::Off getFileNumBytes() const
    {
        return mFileNumBytes;
    }

    const std::vector<sys::byte>& getFileHeader() const
    {
        return mFileHeader;
    }

    const std::vector<std::vector<sys::byte> >& getImageSubheaders() const
    {
        return mImageSubheaders;
    }

    const std::vector<sys::byte>& getDesSubheaderAndData() const
    {
        return mDesSubheaderAndData;
    }

    const std::vector<nitf::Off>& getImageSubheaderFileOffsets() const
    {
        return mImageSubheaderFileOffsets;
    }

    nitf::Off getDesSubheaderFileOffset() const
    {
        return mDesSubheaderFileOffset;
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
                  NITFBufferList& buffers) const;

    /*!
     * \return ImageBlocker with settings in sync with how the image will be
     * blocked in the NITF
     */
    std::auto_ptr<const ImageBlocker> getImageBlocker() const;

protected:
    ByteProvider();

    void initialize(Record& record,
                    const std::vector<PtrAndLength>& desData =
                            std::vector<PtrAndLength>(),
                    size_t numRowsPerBlock = 0,
                    size_t numColsPerBlock = 0);

private:
    void getFileLayout(nitf::Record& inRecord,
                       const std::vector<PtrAndLength>& desData);

    void checkBlocking(size_t seg,
                       size_t startGlobalRowToWrite,
                       size_t numRowsToWrite) const;

private:
    struct SegmentInfo
    {
        SegmentInfo() :
            firstRow(0),
            numRows(0)
        {
        }

        size_t endRow() const
        {
            return (firstRow + numRows);
        }

        bool isInRange(size_t rangeStartRow,
                       size_t rangeNumRows,
                       size_t& firstGlobalRowInThisSegment,
                       size_t& numRowsInThisSegment) const
        {
            return ImageSegmentComputer::Segment::isInRange(
                    firstRow, endRow(), rangeStartRow, rangeNumRows,
                    firstGlobalRowInThisSegment, numRowsInThisSegment);
        }

        size_t firstRow;
        size_t numRows;
    };

    size_t mNumCols;
    size_t mOverallNumRowsPerBlock;

    // Per segment
    std::vector<size_t> mNumRowsPerBlock;
    size_t mNumColsPerBlock;
    size_t mNumBytesPerRow;
    size_t mNumBytesPerPixel;

    // Per segment
    std::vector<SegmentInfo> mImageSegmentInfo;

    std::vector<sys::byte> mFileHeader;

    // Per segment
    std::vector<std::vector<sys::byte> > mImageSubheaders;

    // All DES subheaders and data together contiguously
    std::vector<sys::byte> mDesSubheaderAndData;

    // Per segment
    std::vector<nitf::Off> mImageSubheaderFileOffsets;
    nitf::Off mDesSubheaderFileOffset;
    nitf::Off mFileNumBytes;
};
}
