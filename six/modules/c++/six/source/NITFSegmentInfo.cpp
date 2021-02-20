/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

#include <limits>
#include <algorithm>

#include <nitf/ImageSegmentComputer.h>
#include <six/NITFSegmentInfo.h>

namespace six
{
bool NITFSegmentInfo::isInRange(size_t rangeStartRow,
                                size_t rangeNumRows,
                                size_t& firstGlobalRowInThisSegment,
                                size_t& numRowsInThisSegment) const
{
    return nitf::ImageSegmentComputer::Segment::isInRange(
            getFirstRow(), endRow(), rangeStartRow, rangeNumRows,
            firstGlobalRowInThisSegment, numRowsInThisSegment);
}
}
