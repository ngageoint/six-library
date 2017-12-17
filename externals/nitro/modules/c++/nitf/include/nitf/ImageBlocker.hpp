/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

#include <stddef.h>
#include <string.h>
#include <vector>

namespace nitf
{
// Only supports layouts where image segments are stacked vertically
class ImageBlocker
{
public:
    ImageBlocker(const std::vector<size_t>& numRowsPerSegment,
                 size_t numCols,
                 size_t numRowsPerBlock,
                 size_t numColsPerBlock);

    // startRow must start on a block boundary (within a segment)
    // numRows must be a multiple of block size unless it's the end of a segment
    template <typename DataT>
    size_t getNumBytesRequired(size_t startRow,
                               size_t numRows) const
    {
        return getNumBytesRequired(startRow, numRows, sizeof(DataT));
    }

    size_t getNumBytesRequired(size_t startRow,
                               size_t numRows,
                               size_t numBytesPerPixel) const;

    // startRow must start on a block boundary (within a segment)
    // numRows must be a multiple of block size unless it's the end of a segment
    void block(const void* input,
               size_t startRow,
               size_t numRows,
               size_t numBytesPerPixel,
               void* output) const;

    template <typename DataT>
    void block(const DataT* input,
               size_t startRow,
               size_t numRows,
               DataT* output) const
    {
        block(input, startRow, numRows, sizeof(DataT), output);
    }

    // TODO: Implement
    //       The idea is that OpT is performed on each pixel
    // TODO: Add an overloading that doesn't take in an OpT that just does
    //       an assignment
    /*
    template <typename DataT, typename OpT>
    void block(const DataT* input,
               size_t startRow,
               size_t numRows,
               const OpT& op,
               DataT* output) const;*/

    // TODO: Want a threaded version

    // TODO: Want something that tells you how you should partition the data
    //       given that you can end up with these partial blocks at the end
    //       of a block.


private:
    void findSegment(size_t row,
                     size_t& segIdx,
                     size_t& rowWithinSegment,
                     size_t& blockWithinSegment) const;

    bool isFirstRowInBlock(size_t rowWithinSeg) const
    {
        return (rowWithinSeg % mNumRowsPerBlock == 0);
    }

    void findSegmentRange(size_t startRow,
                          size_t numRows,
                          size_t& firstSegIdx,
                          size_t& startBlockWithinFirstSeg,
                          size_t& lastSegIdx,
                          size_t& lastBlockWithinLastSeg) const;

    void blockImpl(const sys::byte* input,
                   size_t numValidRowsInBlock,
                   size_t numValidColsInBlock,
                   size_t numBytesPerPixel,
                   sys::byte* output) const;

    void blockAcrossRow(const sys::byte*& input,
                        size_t numValidRowsInBlock,
                        size_t numBytesPerPixel,
                        sys::byte*& output) const;

private:
    // Vectors all indexed by segment
    std::vector<size_t> mStartRow;
    const std::vector<size_t> mNumRows;
    const size_t mTotalNumRows;
    const size_t mNumCols;
    const size_t mNumRowsPerBlock;
    const size_t mNumColsPerBlock;

    std::vector<size_t> mNumBlocksDownRows;
    std::vector<size_t> mNumPadRowsInFinalBlock;

    size_t mNumBlocksAcrossCols;
    size_t mNumPadColsInFinalBlock;
};
}

#endif
