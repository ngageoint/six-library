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

#include <sstream>
#include <numeric>
#include <limits>

#include <sys/Conf.h>
#include <except/Exception.h>
#include <nitf/ImageBlocker.hpp>

#undef min
#undef max

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
ImageBlocker::ImageBlocker(size_t numRows,
                           size_t numCols,
                           size_t numRowsPerBlock,
                           size_t numColsPerBlock) :
    mStartRow(1, 0),
    mNumRows(1, numRows),
    mTotalNumRows(numRows),
    mNumCols(numCols),
    mNumRowsPerBlock(1, std::min(numRows, numRowsPerBlock)),
    mNumColsPerBlock(std::min(numCols, numColsPerBlock)),
    mNumBlocksDownRows(1),
    mNumPadRowsInFinalBlock(1)
{
    if (numRowsPerBlock == 0 || numColsPerBlock == 0)
    {
        throw except::Exception(Ctxt("Dimensions per block must be positive"));
    }

    getBlockInfo(numRows, mNumRowsPerBlock[0],
                 mNumBlocksDownRows[0], mNumPadRowsInFinalBlock[0]);

    getBlockInfo(mNumCols, mNumColsPerBlock,
                 mNumBlocksAcrossCols, mNumPadColsInFinalBlock);
}

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
    mNumRowsPerBlock(numRowsPerSegment.size()),
    mNumColsPerBlock(std::min(numCols, numColsPerBlock)),
    mNumBlocksDownRows(numRowsPerSegment.size()),
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

    // For each segment, the # of rows / block is capped at the # of rows in
    // that block (i.e. no point in having a segment with 1 block that's bigger
    // than the # of rows you have)
    for (size_t seg = 0; seg < mNumRows.size(); ++seg)
    {
        mNumRowsPerBlock[seg] = std::min(mNumRows[seg], numRowsPerBlock);
    }

    mStartRow[0] = 0;
    for (size_t seg = 1; seg < mNumRows.size(); ++seg)
    {
        mStartRow[seg] = mStartRow[seg - 1] + mNumRows[seg - 1];
    }

    for (size_t seg = 0; seg < mNumRows.size(); ++seg)
    {
        getBlockInfo(mNumRows[seg], mNumRowsPerBlock[seg],
                     mNumBlocksDownRows[seg], mNumPadRowsInFinalBlock[seg]);
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

        if (row < endRowOfSeg)
        {
            segIdx = seg;
            rowWithinSegment = row - mStartRow[seg];
            blockWithinSegment = rowWithinSegment / mNumRowsPerBlock[seg];
            return;
        }
    }

    std::ostringstream ostr;
    ostr << "Row " << row << " is out of bounds";
    throw except::Exception(Ctxt(ostr.str()));
}

void ImageBlocker::findSegmentRange(size_t startRow,
                                    size_t numRows,
                                    size_t& firstSegIdx,
                                    size_t& startBlockWithinFirstSeg,
                                    size_t& lastSegIdx,
                                    size_t& lastBlockWithinLastSeg) const
{
    if (numRows == 0)
    {
        firstSegIdx = startBlockWithinFirstSeg =
                lastSegIdx = lastBlockWithinLastSeg =
                std::numeric_limits<size_t>::max();
    }
    else
    {
        // Figure out which segment we're starting in
        size_t startRowWithinFirstSeg;
        findSegment(startRow, firstSegIdx, startRowWithinFirstSeg,
                    startBlockWithinFirstSeg);

        if (!isFirstRowInBlock(firstSegIdx, startRowWithinFirstSeg))
        {
            std::ostringstream ostr;
            ostr << "Start row " << startRow << " is local row "
                 << startRowWithinFirstSeg << " within segment " << firstSegIdx
                 << ".  The local row must be a multiple of "
                 << mNumRowsPerBlock[firstSegIdx] << ".";
            throw except::Exception(Ctxt(ostr.str()));
        }

        // Figure out which segment we're ending in
        const size_t lastRow = startRow + numRows - 1;

        size_t lastRowWithinLastSeg;
        findSegment(lastRow, lastSegIdx, lastRowWithinLastSeg,
                    lastBlockWithinLastSeg);
        const size_t endRowWithinLastSeg = lastRowWithinLastSeg + 1;

        // Make sure we're ending on a full block
        if (!(endRowWithinLastSeg == mNumRows[lastSegIdx] ||
              isFirstRowInBlock(lastSegIdx, endRowWithinLastSeg)))
        {
            std::ostringstream ostr;
            ostr << "Last row " << lastRow << " is local row "
                 << lastRowWithinLastSeg << " within segment " << lastSegIdx
                 << ".  This must land on a full block.";
            throw except::Exception(Ctxt(ostr.str()));
        }
    }
}

size_t ImageBlocker::getNumBytesRequired(size_t startRow,
                                         size_t numRows,
                                         size_t numBytesPerPixel) const
{
    if (numRows == 0)
    {
        return 0;
    }

    // Find which segments we're in
    size_t firstSegIdx;
    size_t startBlockWithinFirstSeg;
    size_t lastSegIdx;
    size_t lastBlockWithinLastSeg;
    findSegmentRange(startRow, numRows, firstSegIdx, startBlockWithinFirstSeg,
                     lastSegIdx, lastBlockWithinLastSeg);

    // Now count up the blocks
    size_t totalNumRows;
    if (lastSegIdx == firstSegIdx)
    {
        totalNumRows =
                (lastBlockWithinLastSeg - startBlockWithinFirstSeg + 1) *
                mNumRowsPerBlock[firstSegIdx];
    }
    else
    {
        // First seg
        totalNumRows =
                (mNumBlocksDownRows[firstSegIdx] - startBlockWithinFirstSeg) *
                mNumRowsPerBlock[firstSegIdx];

        // Middle segs
        for (size_t seg = firstSegIdx + 1; seg < lastSegIdx; ++seg)
        {
            totalNumRows += mNumBlocksDownRows[seg] * mNumRowsPerBlock[seg];
        }

        // Last seg
        totalNumRows +=
                (lastBlockWithinLastSeg + 1) * mNumRowsPerBlock[lastSegIdx];
    }

    const size_t numBytes =
            totalNumRows *
            mNumBlocksAcrossCols * mNumColsPerBlock *
            numBytesPerPixel;

    return numBytes;
}

void ImageBlocker::block(const void* input,
                         size_t numBytesPerPixel,
                         size_t numCols,
                         size_t numRowsPerBlock,
                         size_t numColsPerBlock,
                         size_t numValidRowsInBlock,
                         size_t numValidColsInBlock,
                         void* output)
{
    const size_t inStride = numCols * numBytesPerPixel;
    const size_t outNumValidBytes = numValidColsInBlock * numBytesPerPixel;
    auto inputPtr = static_cast<const nitf::byte*>(input);
    auto outputPtr = static_cast<nitf::byte*>(output);

    if (numValidColsInBlock == numColsPerBlock)
    {
        for (size_t row = 0;
             row < numValidRowsInBlock;
             ++row, inputPtr += inStride, outputPtr += outNumValidBytes)
        {
            ::memcpy(outputPtr, inputPtr, outNumValidBytes);
        }
    }
    else
    {
        // Have to deal with pad columns
        const size_t outNumInvalidBytes =
                (numColsPerBlock - numValidColsInBlock) * numBytesPerPixel;

        for (size_t row = 0;
             row < numValidRowsInBlock;
             ++row, inputPtr += inStride)
        {
            ::memcpy(outputPtr, inputPtr, outNumValidBytes);
            outputPtr += outNumValidBytes;

            ::memset(outputPtr, 0, outNumInvalidBytes);
            outputPtr += outNumInvalidBytes;
        }
    }

    // Pad rows on the bottom of the block
    if (numValidRowsInBlock < numRowsPerBlock)
    {
        const size_t numPadRows = numRowsPerBlock - numValidRowsInBlock;

        ::memset(outputPtr, 0,
                 numPadRows * numColsPerBlock * numBytesPerPixel);
    }
}

void ImageBlocker::blockAcrossRow(size_t seg,
                                  const nitf::byte*& input,
                                  size_t numValidRowsInBlock,
                                  size_t numBytesPerPixel,
                                  nitf::byte*& output) const
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

void ImageBlocker::block(const void* input,
                         size_t startRow,
                         size_t numRows,
                         size_t numBytesPerPixel,
                         void* output) const
{
    if (numRows == 0)
    {
        return;
    }

    // Find which segments we're in
    size_t firstSegIdx;
    size_t startBlockWithinFirstSeg;
    size_t lastSegIdx;
    size_t lastBlockWithinLastSeg;
    findSegmentRange(startRow, numRows, firstSegIdx, startBlockWithinFirstSeg,
                     lastSegIdx, lastBlockWithinLastSeg);

    auto inputPtr = static_cast<const nitf::byte*>(input);
    auto outputPtr = static_cast<nitf::byte*>(output);

    for (size_t seg = firstSegIdx; seg <= lastSegIdx; ++seg)
    {
        const size_t overallLastRowBlockOfSegment = mNumBlocksDownRows[seg] - 1;

        const size_t startRowBlockOfSegment = (seg == firstSegIdx) ?
                startBlockWithinFirstSeg : 0;

        const size_t lastRowBlockOfSegment = (seg == lastSegIdx) ?
                lastBlockWithinLastSeg : overallLastRowBlockOfSegment;

        for (size_t rowBlock = startRowBlockOfSegment;
             rowBlock <= lastRowBlockOfSegment;
             ++rowBlock)
        {
            const size_t numPadRowsInBlock =
                    (rowBlock == overallLastRowBlockOfSegment) ?
                            mNumPadRowsInFinalBlock[seg] : 0;

            const size_t numValidRowsInBlock =
                    mNumRowsPerBlock[seg] - numPadRowsInBlock;

            // This increments both pointers
            blockAcrossRow(seg,
                           inputPtr,
                           numValidRowsInBlock,
                           numBytesPerPixel,
                           outputPtr);
        }
    }
}

size_t ImageBlocker::getSegmentFromGlobalBlockRow(size_t blockRow) const
{
    size_t startBlock = 0;
    for (size_t seg = 0; seg < mNumBlocksDownRows.size(); ++seg)
    {
        const size_t nextStartBlock = startBlock + mNumBlocksDownRows[seg];
        if (startBlock <= blockRow && blockRow < nextStartBlock)
        {
            return seg;
        }
        startBlock = nextStartBlock;
    }

    std::ostringstream message;
    message << "Requested block row " << blockRow << ", but this image "
        << "only has " << startBlock << " rows of blocks.";
    throw except::Exception(Ctxt(message.str()));
}
}
