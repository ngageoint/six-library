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
#ifndef NITF_ByteProvider_hpp_INCLUDED_
#define NITF_ByteProvider_hpp_INCLUDED_
#pragma once

#include <vector>
#include <utility>
#include <memory>
#include <std/span>
#include <std/cstddef>

#include <nitf/coda-oss.hpp>
#include <nitf/System.hpp>
#include <nitf/Record.hpp>
#include <nitf/ImageBlocker.hpp>
#include <nitf/NITFBufferList.hpp>
#include <nitf/ImageSegmentComputer.h>
#include "nitf/exports.hpp"

namespace nitf
{
/*!
 * \class ByteProvider
 * \brief Used to provide corresponding raw NITF bytes (including NITF headers)
 * when provided with some AOI of the pixel data.  The idea is that if
 * getBytes() is called multiple times, eventually for the entire image, the
 * raw bytes provided back will be the entire NITF file.  This abstraction is
 * useful if separate threads, processes, or even machines have only portions of
 * the NITF pixel data and are all trying to write out a single file; in
 * that scenario, this class provides all the raw bytes corresponding to the
 * caller's AOI, including NITF headers if necessary.  The caller does not need
 * to understand anything about the NITF file layout in order to write out the
 * file.  The bytes are intentionally provided back as a series of pointers
 * rather than one contiguous block of memory in order to minimize the number of
 * copies.
 *
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
struct NITRO_NITFCPP_API ByteProvider
{
    typedef std::pair<const void*, size_t> PtrAndLength;
    using PtrAndLength_t = std::span<const std::byte>;

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
    ByteProvider(Record& record,
                 const std::vector<PtrAndLength>& desData =
                            std::vector<PtrAndLength>(),
                    size_t numRowsPerBlock = 0,
                    size_t numColsPerBlock = 0);
    ByteProvider(Record& record,
        const std::vector<PtrAndLength_t>& desData,
        size_t numRowsPerBlock = 0,
        size_t numColsPerBlock = 0);

    /*!
     * Destructor.  No virtual methods but this is virtual in case it's useful
     * to inherit from this class and use it polymorphically.
     */
    virtual ~ByteProvider() = default;

    //! \return The total number of bytes in the NITF
    nitf::Off getFileNumBytes() const noexcept
    {
        return mFileNumBytes;
    }

    //! \return The raw file header bytes
    const std::vector<nitf::byte>& getFileHeader() const noexcept
    {
        return mFileHeader;
    }
    void getFileHeader(std::span<const std::byte>&) const noexcept;

    /*!
     * \return The raw bytes for each image subheader.  Vector size matches the
     * number of image segments.
     */
    const std::vector<std::vector<nitf::byte> >& getImageSubheaders() const noexcept
    {
        return mImageSubheaders;
    }
    void getImageSubheaders(std::vector<std::span<const std::byte>>&) const;

    /*!
     * \return The raw bytes for each DES (subheader immediately followed by
     * raw DES data).  Vector size matches the number of data extension segments.
     */
    const std::vector<nitf::byte>& getDesSubheaderAndData() const noexcept
    {
        return mDesSubheaderAndData;
    }
    void getDesSubheaderAndData(std::span<const std::byte>&) const noexcept;

    /*!
     * \return The file offset for each image subheader.  Vector size matches
     * the number of image segments.
     */
    const std::vector<nitf::Off>& getImageSubheaderFileOffsets() const noexcept
    {
        return mImageSubheaderFileOffsets;
    }

    /*!
     * \return The file offset for the first DES subheader.
     */
    nitf::Off getDesSubheaderFileOffset() const noexcept
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
    virtual nitf::Off getNumBytes(size_t startRow, size_t numRows) const;

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
     * \param imageData The image data pixels to write. Must be in big endian
     * order and blocked if the NITF is.
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
    virtual void getBytes(const void* imageData,
                          size_t startRow,
                          size_t numRows,
                          nitf::Off& fileOffset,
                          NITFBufferList& buffers) const;

    /*!
     * \return ImageBlocker with settings in sync with how the image will be
     * blocked in the NITF
     */
    mem::auto_ptr<const ImageBlocker> getImageBlocker() const;

protected:
    /*!
     * Default constructor.  Expectation is that if an inheriting class uses
     * this constructor, the inheriting class will call initialize() later in
     * its constructor.
     */
    ByteProvider() = default;

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
                    const std::vector<PtrAndLength>& desData =
                            std::vector<PtrAndLength>(),
                    size_t numRowsPerBlock = 0,
                    size_t numColsPerBlock = 0);
    void initialize(const Record& record,
        const std::vector<PtrAndLength_t>& desData,
        size_t numRowsPerBlock = 0, size_t numColsPerBlock = 0);

    static void copyFromStreamAndClear(io::ByteStream& stream,
                                       std::vector<nitf::byte>& rawBytes);
    static void copyFromStreamAndClear(io::ByteStream& stream,
                                       std::vector<std::byte>& rawBytes);

    size_t countPadRows(
            size_t seg, size_t numRowsToWrite,
            size_t imageDataEndRow) const noexcept;

    void addImageData(
            size_t seg,
            size_t numRowsToWrite,
            size_t startRow,
            size_t imageDataEndRow,
            size_t startGlobalRowToWrite,
            const void* imageData,
            size_t& numPadRowsSoFar,
            nitf::Off& fileOffset,
            NITFBufferList& buffers) const;

    size_t countBytesForHeaders(size_t seg, size_t startRow) const noexcept;
    size_t countBytesForDES(size_t seg, size_t imageDataEndRow) const noexcept;

    void addHeaders(size_t seg, size_t startRow,
            nitf::Off& fileOffset,
            NITFBufferList& buffers) const;

    /*
     * These functions assume that we've already checked
     * we're writing in a range which includes seg
     */
    bool shouldAddHeader(size_t seg, size_t startRow) const noexcept;
    bool shouldAddSubheader(size_t seg, size_t startRow) const noexcept;
    bool shouldAddDES(size_t seg, size_t imageDataEndRow) const noexcept;

    void addDES(size_t seg, size_t imageDataEndRow,
                NITFBufferList& buffers) const;

    void getFileLayout(const nitf::Record& inRecord,
                       const std::vector<PtrAndLength>& desData);
    void getFileLayout(const nitf::Record & inRecord,
        const std::vector<PtrAndLength_t>&desData);

    std::vector<size_t> mImageDataLengths;

    void checkBlocking(size_t seg,
                       size_t startGlobalRowToWrite,
                       size_t numRowsToWrite) const;

    void initializeImpl(
            const Record& record,
            const std::vector<PtrAndLength>& desData,
            size_t numRowsPerBlock,
            size_t numColsPerBlock);
    void initializeImpl(
        const Record & record,
        const std::vector<PtrAndLength_t>&desData,
        size_t numRowsPerBlock,
        size_t numColsPerBlock);

    // Represents the row information for a NITF image segment
    struct SegmentInfo
    {
        SegmentInfo() = default;

        size_t endRow() const noexcept
        {
            return (firstRow + numRows);
        }

        bool isInRange(size_t rangeStartRow,
                       size_t rangeNumRows,
                       size_t& firstGlobalRowInThisSegment,
                       size_t& numRowsInThisSegment) const noexcept
        {
            return ImageSegmentComputer::Segment::isInRange(
                    firstRow, endRow(), rangeStartRow, rangeNumRows,
                    firstGlobalRowInThisSegment, numRowsInThisSegment);
        }

        size_t firstRow = 0;
        size_t numRows = 0;
    };

    size_t mNumCols = 0;
    size_t mOverallNumRowsPerBlock = 0;

    std::vector<size_t> mNumRowsPerBlock; // Per segment
    size_t mNumColsPerBlock = 0;
    size_t mNumBytesPerRow = 0;
    size_t mNumBytesPerPixel = 0;

    std::vector<SegmentInfo> mImageSegmentInfo; // Per segment

    std::vector<nitf::byte> mFileHeader;
    std::vector<std::vector<nitf::byte> > mImageSubheaders; // Per segment

    // All DES subheaders and data together contiguously
    std::vector<nitf::byte> mDesSubheaderAndData;

    std::vector<nitf::Off> mImageSubheaderFileOffsets; // Per segment
    nitf::Off mDesSubheaderFileOffset = 0;
    nitf::Off mFileNumBytes = 0;

    private:
        template<typename TPtrAndLength>
        void getFileLayout_(const nitf::Record& inRecord,
            const std::vector<TPtrAndLength>& desData);

        template<typename TPtrAndLength>
        void initializeImpl_(
            const Record& record,
            const std::vector<TPtrAndLength>& desData,
            size_t numRowsPerBlock,
            size_t numColsPerBlock);
};
}

#endif // NITF_ByteProvider_hpp_INCLUDED_
