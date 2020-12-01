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
#include <limits>
#include <cmath>
#include <algorithm>

#include <sys/Conf.h>
#include <except/Exception.h>
#include <nitf/ImageSegmentComputer.h>

#include "gsl/gsl.h"

#undef min
#undef max

namespace nitf
{
const size_t ImageSegmentComputer::ILOC_MAX = 99999;
const uint64_t ImageSegmentComputer::NUM_BYTES_MAX = 9999999998LL;

std::string ImageSegmentComputer::Segment::getILOC() const
{
    constexpr size_t COL = 0;

    std::ostringstream ostr;
    ostr.fill('0');
    ostr << std::setw(5) << rowOffset << std::setw(5) << COL;

    return ostr.str();
}

bool ImageSegmentComputer::Segment::isInRange(
        size_t firstRow,
        size_t endRow,
        size_t rangeStartRow,
        size_t rangeNumRows,
        size_t& firstGlobalRowInThisSegment,
        size_t& numRowsInThisSegment) noexcept
{
    const size_t startGlobalRow = std::max(firstRow, rangeStartRow);
    const size_t endGlobalRow =
            std::min(endRow, rangeStartRow + rangeNumRows);

    if (endGlobalRow > startGlobalRow)
    {
        firstGlobalRowInThisSegment = startGlobalRow;
        numRowsInThisSegment = endGlobalRow - startGlobalRow;
        return true;
    }
    else
    {
        firstGlobalRowInThisSegment = std::numeric_limits<size_t>::max();
        numRowsInThisSegment = 0;
        return false;
    }
}

bool ImageSegmentComputer::Segment::isInRange(
        size_t rangeStartRow,
        size_t rangeNumRows,
        size_t& firstGlobalRowInThisSegment,
        size_t& numRowsInThisSegment) const noexcept
{
    return isInRange(firstRow, endRow(), rangeStartRow, rangeNumRows,
                     firstGlobalRowInThisSegment, numRowsInThisSegment);
}

ImageSegmentComputer::ImageSegmentComputer(size_t numRows,
                                           size_t numCols,
                                           size_t numBytesPerPixel,
                                           size_t maxRows,
                                           uint64_t maxSize,
                                           size_t rowsPerBlock,
                                           size_t colsPerBlock) :
    mNumRows(numRows),
    mNumCols(numCols),
    mNumBytesPerPixel(numBytesPerPixel),
    mNumRowsLimit(maxRows),
    mNumColsPaddedForBlocking(getActualDim(mNumCols, colsPerBlock)),
    mNumRowsPerBlock(rowsPerBlock),
    mMaxNumBytesPerSegment(maxSize),
    mNumBytesTotal(gsl::narrow<uint64_t>(mNumBytesPerPixel) *
                 getActualDim(mNumRows, rowsPerBlock) *
                 mNumColsPaddedForBlocking)
{
    if (maxRows > ILOC_MAX)
    {
        std::ostringstream ostr;
        ostr << "Max rows was set to " << maxRows
             << " but it cannot be greater than " << ILOC_MAX
             << " per the NITF spec";
        throw except::Exception(Ctxt(ostr.str()));
    }

    if (maxSize > NUM_BYTES_MAX)
    {
        std::ostringstream ostr;
        ostr << "Max image segment size was set to " << maxSize
             << " but it cannot be greater than " << NUM_BYTES_MAX
             << " per the NITF spec";
        throw except::Exception(Ctxt(ostr.str()));
    }

    computeImageInfo();
    computeSegmentInfo();
}

size_t ImageSegmentComputer::getActualDim(size_t dim, size_t numDimsPerBlock) noexcept
{
    if (numDimsPerBlock == 0)
    {
        return dim;
    }
    else
    {
        // If we're blocking then we'll always write full blocks due to how
        // the NITF file layout works
        const size_t numBlocks =
                (dim / numDimsPerBlock) + (dim % numDimsPerBlock != 0);
        return numBlocks * numDimsPerBlock;
    }
}

void ImageSegmentComputer::computeImageInfo()
{
    // Consider possible blocking when determining the maximum number of rows
    const uint64_t bytesPerRow =
        gsl::narrow<uint64_t>(mNumBytesPerPixel) *
            mNumColsPaddedForBlocking;

    const uint64_t maxRowsUint64 =
        gsl::narrow<uint64_t>(mMaxNumBytesPerSegment) / bytesPerRow;
    if (maxRowsUint64 > std::numeric_limits<size_t>::max())
    {
        // This should not be possible
        std::ostringstream ostr;
        ostr << "Image would require " << maxRowsUint64
             << " rows which is too many";
        throw except::Exception(Ctxt(ostr.str()));
    }
    size_t maxRows(gsl::narrow<size_t>(maxRowsUint64));

    if (maxRows == 0)
    {
        std::ostringstream ostr;
        ostr << "maxNumBytesPerSegment [" << mMaxNumBytesPerSegment
             << "] < bytesPerRow [" << bytesPerRow << "]";
        throw except::Exception(Ctxt(ostr.str()));
    }

    // Truncate back to a full block for the maxRows that will actually fit
    if (mNumRowsPerBlock != 0)
    {
        const size_t numBlocksVert = maxRows / mNumRowsPerBlock;
        maxRows = numBlocksVert * mNumRowsPerBlock;

        if (maxRows == 0)
        {
            std::ostringstream ostr;
            ostr << "With a max number of bytes per segment of "
                 << mMaxNumBytesPerSegment
                 << ", bytes per row of " << bytesPerRow
                 << ", and rows per block of " << mNumRowsPerBlock
                 << ", cannot fit an entire block into a segment";
            throw except::Exception(Ctxt(ostr.str()));
        }
    }

    if (mNumRowsLimit > maxRows)
    {
        mNumRowsLimit = maxRows;
    }

    // Ensure that the row limit is a multiple of block size
    // The NITF spec requires that when blocking is used, all blocks are the
    // same size (so when there is a "short" block, pad rows to make it a full
    // block are used).  So, it's silly to not make the row limit a multiple of
    // the block size - if we don't, the overall NITF will needlessly be bigger
    // with pad rows.
    if (mNumRowsPerBlock != 0)
    {
        mNumRowsLimit -= mNumRowsLimit % mNumRowsPerBlock;
    }
}

void ImageSegmentComputer::computeSegmentInfo()
{
    // TODO: other conditions which should trigger segmentation:
    //   NCOLS >= 10E8   column direction segmentation, no numColsLimit logic
    //                   in place
    //   NROWS >= 10E8   most likely would exceed mMaxNumBytesPerSegment
    //                   (ncols < 100 unlikely)
    //                   and segment to mNumRowsLimit anyway
    if (mNumBytesTotal <= mMaxNumBytesPerSegment)
    {
        mSegments.resize(1);
        mSegments[0].numRows = mNumRows;
        mSegments[0].firstRow = 0;
        mSegments[0].rowOffset = 0;
    }

    else
    {
        // NOTE: See header for why rowOffset is always set to mNumRowsLimit
        //       for image segments 1 and above
        const auto numIS = static_cast<size_t>(std::ceil(
                static_cast<double>(mNumRows) / static_cast<double>(mNumRowsLimit)));

        mSegments.resize(numIS);
        mSegments[0].numRows = mNumRowsLimit;
        mSegments[0].firstRow = 0;
        mSegments[0].rowOffset = 0;
        size_t ii;
        for (ii = 1; ii < numIS - 1; ii++)
        {
            mSegments[ii].numRows = mNumRowsLimit;
            mSegments[ii].firstRow = ii * mNumRowsLimit;
            mSegments[ii].rowOffset = mNumRowsLimit;
        }

        mSegments[ii].firstRow = ii * mNumRowsLimit;
        mSegments[ii].rowOffset = mNumRowsLimit;
        mSegments[ii].numRows = mNumRows - (numIS - 1) * mNumRowsLimit;
    }
}
}
