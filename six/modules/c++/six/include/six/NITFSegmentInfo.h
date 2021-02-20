/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#ifndef __SIX_NITF_SEGMENT_INFO_H__
#define __SIX_NITF_SEGMENT_INFO_H__

#include <algorithm>

#include <six/Types.h>

namespace six
{
/*!
 *  \class NITFSegmentInfo
 *  \brief Book-keeping for NITF segments
 *
 *  This class contains the per-segment information for an image.
 */
struct NITFSegmentInfo
{
    //! First row in the image segment in real space
    size_t firstRow;
    size_t getFirstRow() const { return firstRow; }

    //! Row offset in the CCS (ILOC R)
    //  When you are attached to another segment, ILOC is with respect to that
    //  segment.  Per the spec, we will always attach to the previous segment,
    //  so this will simply be the number of rows in that previous segment.
    size_t rowOffset;
    size_t getRowOffset() const { return rowOffset; }


    //! Number of rows in this segment
    size_t numRows;
    size_t getNumRows() const { return numRows; }

    //! The image segment corner points
    LatLonCorners corners;

    /*!
     * \return The end row (exclusive) of the image segment
     */
    size_t getEndRow() const
    {
        return getFirstRow() + getNumRows();
    }
    size_t endRow() const
    {
        return getEndRow();
    }

    /*!
     * Given a global pixel range of
     * [rangeStartRow, rangeStartRow + rangeNumRows), determines if this range
     * overlaps with this segment and, if so, what rows overlap with it
     *
     * \param rangeStartRow The inclusive global start row of the range of
     * interest
     * \param rangeNumRows The number of rows in the range of interest
     * \param[out] firstGlobalRowInThisSegment If the range does appear in this
     * segment, the first global row of the range that's in the segment (if the
     * range starts prior to this segment, it'll be the first row of the
     * segment.  If the range starts inside the segment, it'll be the first row
     * of the range).  If the range does not appear in this segment, this is
     * set to numeric_limits<size_t>::max().
     * \param[out] numRowsInThisSegment The number of rows of the range which
     * appear in this segment
     *
     * \return True if the range overlaps with this segment, false otherwise
     */
    bool isInRange(size_t rangeStartRow,
                   size_t rangeNumRows,
                   size_t& firstGlobalRowInThisSegment,
                   size_t& numRowsInThisSegment) const;
};
}

#endif
