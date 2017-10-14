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

#include <sstream>
#include <numeric>

#include <sys/Conf.h>
#include <except/Exception.h>
#include <nitf/ImageBlocker.hpp>

namespace
{
void getBlockInfo(size_t numElements,
                  size_t numElementsPerBlock,
                  size_t& numBlocks,
                  size_t& numPadElementsInFinalBlock)
{
    numBlocks = (numElements / numElementsPerBlock);
    const size_t numLeftovers = numElements % numElementsPerBlock;

    if (numLeftovers == 0)
    {
        numPadElementsInFinalBlock = 0;
    }
    else
    {
        numPadElementsInFinalBlock = numElementsPerBlock - numLeftovers;
        ++numBlocks;
    }
}
}

namespace nitf
{
ImageBlocker::ImageBlocker(const std::vector<size_t>& numRowsPerSegment,
                           size_t numCols,
                           size_t numRowsPerBlock,
                           size_t numColsPerBlock) :
    mStartRow(numRowsPerSegment.size()),
    mNumRows(numRowsPerSegment),
    mTotalNumRows(std::accumulate(numRowsPerSegment.begin(),
                                  numRowsPerSegment.end(),
                                  static_cast<size_t>(0))),
    mNumCols(numCols),
    mNumRowsPerBlock(numRowsPerBlock),
    mNumColsPerBlock(numColsPerBlock),
    mNumBlocksAcrossRows(numRowsPerSegment.size()),
    mNumPadRowsInFinalBlock(numRowsPerSegment.size())
{
    if (numRowsPerSegment.empty())
    {
        throw except::Exception(Ctxt("Must provide at least one segment"));
    }

    if (numRowsPerBlock == 0 || numColsPerBlock == 0)
    {
        throw except::Exception(Ctxt("Dimensions per block must be positive"));
    }

    mStartRow[0] = 0;
    for (size_t seg = 1; seg < mNumRows.size(); ++seg)
    {
        mStartRow[seg] = mStartRow[seg - 1] + mNumRows[seg - 1];
    }

    for (size_t seg = 0; seg < mNumRows.size(); ++seg)
    {
        getBlockInfo(mNumRows[seg], mNumRowsPerBlock,
                     mNumBlocksAcrossRows[seg], mNumPadRowsInFinalBlock[seg]);
    }

    getBlockInfo(mNumCols, mNumColsPerBlock,
                 mNumBlocksAcrossCols, mNumPadColsInFinalBlock);
}

void ImageBlocker::findSegment(size_t row,
                               size_t& segIdx,
                               size_t& rowWithinSegment,
                               size_t& blockWithinSegment) const
{
    for (size_t seg = 0; seg < mStartRow.size(); ++seg)
    {
        const size_t endRowOfSeg = mStartRow[seg] + mNumRows[seg];

        if (endRowOfSeg < row)
        {
            segIdx = seg;
            rowWithinSegment = row - mStartRow[seg];
            blockWithinSegment = rowWithinSegment / mNumRowsPerBlock;
            return;
        }
    }

    std::ostringstream ostr;
    ostr << "Row " << row << " is out of bounds";
    throw except::Exception(Ctxt(ostr.str()));
}

size_t ImageBlocker::getNumBytesRequired(size_t startRow,
                                         size_t numRows,
                                         size_t numBytesPerPixel) const
{
    if (numRows == 0)
    {
        return 0;
    }

    // Figure out which segment we're starting in
    size_t firstSegIdx;
    size_t startRowWithinFirstSeg;
    size_t startBlockWithinFirstSeg;
    findSegment(startRow, firstSegIdx, startRowWithinFirstSeg,
                startBlockWithinFirstSeg);

    if (!isFirstRowInBlock(startRowWithinFirstSeg))
    {
        std::ostringstream ostr;
        ostr << "Start row " << startRow << " is local row "
             << startRowWithinFirstSeg << " within segment " << firstSegIdx
             << ".  The local row must be a multiple of " << mNumRowsPerBlock
             << ".";
        throw except::Exception(Ctxt(ostr.str()));
    }

    // Figure out which segment we're ending in
    const size_t lastRow = startRow + numRows - 1;

    size_t lastSegIdx;
    size_t lastRowWithinLastSeg;
    size_t lastBlockWithinLastSeg;
    findSegment(lastRow, lastSegIdx, lastRowWithinLastSeg,
                lastBlockWithinLastSeg);

    // Make sure we're ending on a full block
    if (!(lastRowWithinLastSeg == mNumRows[lastSegIdx] ||
          isFirstRowInBlock(lastRowWithinLastSeg + 1)))
    {
        std::ostringstream ostr;
        ostr << "Last row " << lastRow << " is local row "
             << lastRowWithinLastSeg << " within segment " << lastSegIdx
             << ".  This must land on a full block.";
        throw except::Exception(Ctxt(ostr.str()));
    }

    // Now count up the blocks
    size_t numRowBlocks;
    if (lastSegIdx == firstSegIdx)
    {
        numRowBlocks = lastBlockWithinLastSeg - startBlockWithinFirstSeg + 1;
    }
    else
    {
        // First seg
        numRowBlocks =
                mNumBlocksAcrossRows[firstSegIdx] - startBlockWithinFirstSeg;

        // Middle segs
        for (size_t seg = firstSegIdx + 1; seg < lastSegIdx; ++seg)
        {
            numRowBlocks += mNumBlocksAcrossRows[seg];
        }

        // Last seg
        numRowBlocks += lastBlockWithinLastSeg + 1;
    }

    const size_t numBytes =
            numRowBlocks * mNumRowsPerBlock *
            mNumBlocksAcrossCols * mNumColsPerBlock *
            numBytesPerPixel;

    return numBytes;
}
}
