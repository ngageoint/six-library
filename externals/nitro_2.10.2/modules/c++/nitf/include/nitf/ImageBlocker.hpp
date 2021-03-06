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

#ifndef __NITF_IMAGE_BLOCKER_HPP__
#define __NITF_IMAGE_BLOCKER_HPP__
#pragma once

#include <stddef.h>
#include <string.h>
#include <vector>

#include "nitf/coda-oss.hpp"
#include "nitf/System.hpp"

namespace nitf
{
/*!
 * \class ImageBlocker
 * \brief Supports efficiently rearranging an image to fulfill NITF blocking
 * conventions, including pad rows and columns (NITF spec specifies that full
 * blocks will always be written).  Only supports layouts where image segments
 * are stacked vertically.  Following convention of other areas of the code that
 * write NITFs, will limit the block size to the size of the dimension (which
 * means for multi-segment cases, there may be a different num rows / block in
 * each segment).
 */
struct ImageBlocker /*final*/   // no "final", SWIG doesn't like it
{
    ImageBlocker(const ImageBlocker&) = delete;
    ImageBlocker& operator=(const ImageBlocker&) = delete;

    /*!
     * Use when there's a single image segment
     *
     * \param numRows Number of rows
     * \param numCols Number of columns
     * \param numRowsPerBlock Number of rows per block
     * \param numColsPerBlock Number of columns per block
     */
    ImageBlocker(size_t numRows,
                 size_t numCols,
                 size_t numRowsPerBlock,
                 size_t numColsPerBlock);

    /*!
     * Use when there are multiple image segments
     *
     * \param numRowsPerSegment Number of rows in each segment
     * \param numCols Number of columns
     * \param numRowsPerBlock Number of rows per block
     * \param numColsPerBlock Number of columns per block
     */
    ImageBlocker(const std::vector<size_t>& numRowsPerSegment,
                 size_t numCols,
                 size_t numRowsPerBlock,
                 size_t numColsPerBlock);

    /*!
     * \tparam DataT Data type of image pixels
     *
     * \param startRow Start row.  This must start on a block boundary (within
     * a segment).
     * \param numRows Number of rows.  This must be a multiple of the block size
     * unless it's at the end of a segment.
     *
     * \return The number of bytes required to store this AOI of the image in
     * blocked format, including pad rows and columns
     */
    template <typename DataT>
    size_t getNumBytesRequired(size_t startRow,
                               size_t numRows) const
    {
        return getNumBytesRequired(startRow, numRows, sizeof(DataT));
    }

    /*!
     * \param startRow Start row.  This must start on a block boundary (within
     * a segment).
     * \param numRows Number of rows.  This must be a multiple of the block size
     * unless it's at the end of a segment.
     * \param numBytesPerPixel Number of bytes per pixel of image pixels
     *
     * \return The number of bytes required to store this AOI of the image in
     * blocked format, including pad rows and columns
     */
    size_t getNumBytesRequired(size_t startRow,
                               size_t numRows,
                               size_t numBytesPerPixel) const;

    /*!
     * \param input Input image of size 'numRows' x numCols (from constructor)
     * \param startRow Start row in the global image that 'input' points to.
     * This must start on a block boundary (within a segment).
     * \param numRows Number of rows.  This must be a multiple of the block size
     * unless it's at the end of a segment.
     * \param numBytesPerPixel Number of bytes per pixel of 'input'
     * \param[out] output Blocked representation of 'input', including pad rows
     * and columns
     */
    void block(const void* input,
               size_t startRow,
               size_t numRows,
               size_t numBytesPerPixel,
               void* output) const;

    /*!
     * \tparam DataT Data type of 'input' and 'output'
     *
     * \param input Input image of size 'numRows' x numCols (from constructor)
     * \param startRow Start row in the global image that 'input' points to.
     * This must start on a block boundary (within a segment).
     * \param numRows Number of rows.  This must be a multiple of the block size
     * unless it's at the end of a segment.
     * \param[out] output Blocked representation of 'input', including pad rows
     * and columns
     */
    template <typename DataT>
    void block(const DataT* input,
               size_t startRow,
               size_t numRows,
               DataT* output) const noexcept
    {
        block(input, startRow, numRows, sizeof(DataT), output);
    }

    /*!
     * \param input Input image of width 'numCols'
     * \param numBytesPerPixel Number of bytes/pixel in 'input' and 'output'
     * \param numCols Number of columns in 'input'
     * \param numRowsPerBlock Number of rows per block
     * \param numColsPerBlock Number of columns per block
     * \param numValidRowsInBlock Number of valid rows in block (this is less
     * than numRowsPerBlock for the bottom row of blocks if
     * numRows % numRowsPerBlock != 0)
     * \param numValidColsInBlock Number of valid columnss in block (this is
     * less than numColsPerBlock for the rightmost column of blocks if
     * numCols % numColsPerBlock != 0)
     * \param[out] output Output image.  Will contain this single block of
     * 'input'.  Must be at least numRowsPerBlock * numColsPerBlock pixels.
     * If the number of valid rows/columns is less than the rows/columns in the
     * block, these pixels will be zero-filled.
     */
    static
    void block(const void* input,
               size_t numBytesPerPixel,
               size_t numCols,
               size_t numRowsPerBlock,
               size_t numColsPerBlock,
               size_t numValidRowsInBlock,
               size_t numValidColsInBlock,
               void* output) noexcept;

    //! \return The number of columns of blocks
    size_t getNumColsOfBlocks() const noexcept
    {
        return mNumBlocksAcrossCols;
    }

    /*!
     * \param seg Segment
     *
     * \return The number of rows of blocks for the specified segment.
     */
    size_t getNumRowsOfBlocks(size_t seg) const
    {
        return mNumBlocksDownRows.at(seg);
    }

    /*!
     * \param seg Segment
     *
     * \return The number of pad rows in the final block of the specified
     * segment
     */
    size_t getNumPadRowsInFinalBlock(size_t seg) const
    {
        return mNumPadRowsInFinalBlock.at(seg);
    }

    /*!
     * \param seg Segment
     *
     * \return The global start row of the specified segment
     */
    size_t getStartRow(size_t seg) const
    {
        return mStartRow.at(seg);
    }

    /*!
     * \param seg Segment
     *
     * \return The number of rows of the specified segment
     */
    size_t getNumRows(size_t seg) const
    {
        return mNumRows.at(seg);
    }

    //! \return The number of segments
    size_t getNumSegments() const noexcept
    {
        return mNumBlocksDownRows.size();
    }

    /*!
     * \return The number of rows per block for each segment.  If a segment
     * contains more rows than numRowsPerBlock, this will be numRowsPerBlock.
     * Otherwise it will be the number of rows in the segment.
     */
    std::vector<size_t> getNumRowsPerBlock() const
    {
        return mNumRowsPerBlock;
    }

    /*!
     * \return The number of columns per block (always the same value for all
     * segments).  If the number of columns in the image are less than the
     * constructed number of columns per block, this will be the number of
     * columns in the image (i.e. no reason to bother to create blocks larger
     * than the image).
     */
    size_t getNumColsPerBlock() const noexcept
    {
        return mNumColsPerBlock;
    }

    /*
     * \return The segment the block-row index falls in
     * \throws If blockRow exceeds the total block rows in the image
     */
    size_t getSegmentFromGlobalBlockRow(size_t blockRow) const;

private:
    void findSegment(size_t row,
                     size_t& segIdx,
                     size_t& rowWithinSegment,
                     size_t& blockWithinSegment) const;

    bool isFirstRowInBlock(size_t seg, size_t rowWithinSeg) const noexcept
    {
        return (rowWithinSeg % mNumRowsPerBlock[seg] == 0);
    }

    void findSegmentRange(size_t startRow,
                          size_t numRows,
                          size_t& firstSegIdx,
                          size_t& startBlockWithinFirstSeg,
                          size_t& lastSegIdx,
                          size_t& lastBlockWithinLastSeg) const;

    void blockImpl(size_t seg,
                   const sys::byte* input,
                   size_t numValidRowsInBlock,
                   size_t numValidColsInBlock,
                   size_t numBytesPerPixel,
                   sys::byte* output) const noexcept
    {
        block(input, numBytesPerPixel, mNumCols, mNumRowsPerBlock[seg],
              mNumColsPerBlock, numValidRowsInBlock, numValidColsInBlock,
              output);
    }
    void blockImpl(size_t seg,
                   const std::byte* input,
                   size_t numValidRowsInBlock,
                   size_t numValidColsInBlock,
                   size_t numBytesPerPixel,
                   std::byte* output) const noexcept
    {
        block(input, numBytesPerPixel, mNumCols, mNumRowsPerBlock[seg],
              mNumColsPerBlock, numValidRowsInBlock, numValidColsInBlock,
              output);
    }

    template<typename T>
    void blockAcrossRowImpl(size_t seg,
                        const T*& input,
                        size_t numValidRowsInBlock,
                        size_t numBytesPerPixel,
                       T*& output) const noexcept
    {
        const size_t outStride =
            mNumRowsPerBlock[seg] * mNumColsPerBlock * numBytesPerPixel;
        const size_t lastColBlock = mNumBlocksAcrossCols - 1;

        for (size_t colBlock = 0;
            colBlock < mNumBlocksAcrossCols;
            ++colBlock, output += outStride)
        {
            const size_t numPadColsInBlock = (colBlock == lastColBlock) ?
                mNumPadColsInFinalBlock : 0;

            const size_t numValidColsInBlock = mNumColsPerBlock - numPadColsInBlock;

            blockImpl(seg,
                input,
                numValidRowsInBlock,
                numValidColsInBlock,
                numBytesPerPixel,
                output);

            input += numValidColsInBlock * numBytesPerPixel;
        }

        // At the end of this, we've incremented the input pointer an entire row
        // We need to increment it the remaining rows in the block
        input += mNumCols * numBytesPerPixel * (numValidRowsInBlock - 1);
    }
    void blockAcrossRow(size_t seg,
                        const sys::byte*& input,
                        size_t numValidRowsInBlock,
                        size_t numBytesPerPixel,
                       sys::byte*& output) const noexcept
    {
        blockAcrossRowImpl(seg, input, numValidRowsInBlock, numBytesPerPixel, output);
    }
    void blockAcrossRow(size_t seg,
                        const std::byte*& input,
                        size_t numValidRowsInBlock,
                        size_t numBytesPerPixel,
                       std::byte*& output) const noexcept
    {
        blockAcrossRowImpl(seg, input, numValidRowsInBlock, numBytesPerPixel, output);
    }

private:
    // Vectors all indexed by segment
    std::vector<size_t> mStartRow;
    const std::vector<size_t> mNumRows;
    const size_t mTotalNumRows;
    const size_t mNumCols;
    std::vector<size_t> mNumRowsPerBlock;
    const size_t mNumColsPerBlock;

    std::vector<size_t> mNumBlocksDownRows;
    std::vector<size_t> mNumPadRowsInFinalBlock;

    size_t mNumBlocksAcrossCols;
    size_t mNumPadColsInFinalBlock;
};
}

#endif
