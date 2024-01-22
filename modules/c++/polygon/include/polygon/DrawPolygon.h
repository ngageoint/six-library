/* =========================================================================
 * This file is part of polygon-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * polygon-c++ is free software; you can redistribute it and/or modify
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
#ifndef __POLYGON_DRAW_POLYGON_H__
#define __POLYGON_DRAW_POLYGON_H__

#include <vector>
#include <algorithm>
#include <cmath>

#include <types/RowCol.h>

#include <polygon/Intersections.h>

namespace polygon
{
/*!
 * This function will "color in" a polygon in an image/buffer
 *
 * \tparam PointT Data type of points.  Must be a floating type (float/double)
 * to work properly
 * \tparam OutT Output data type
 *
 * \param points Vector specifying the polygon to fill in
 * \param numRows Number of rows in the output image/buffer
 * \param numCols Number of columns in the output image/buffer
 * \param color Value to write over elements inside the polygon
 * \param out Pointer to start of output image/buffer
 * \param invert If set to false, all pixels in the polygon will be set to
 * 'color' and all other pixels (those outside the polygon) are untouched.
 * This is probably what you want.  If set to true, all pixels outside the
 * polygon will be set to 'color' and other pixels (the polygon itself) are
 * untouched.
 * \param offset Number of rows and cols to offset polygon. Positive row
 * value shifts the polygon up (equivalently the frame shifts down), and
 * positive col value shifts the polygon left (equiv. the frame shifts right).
 * Defaults to no offset.
 *
 * See Intersections class for additional details
*/
template <typename PointT, typename OutT>
void drawPolygon(const std::vector<types::RowCol<PointT> >& points,
                 size_t numRows,
                 size_t numCols,
                 OutT color,
                 OutT* out,
                 bool invert = false,
                 types::RowCol<sys::SSize_T> offset =
                         types::RowCol<sys::SSize_T>(0, 0))
{
    if (points.empty())
    {
        // Nothing to do
        return;
    }

    // We need to get all scanline intersections of polygon edges
    const Intersections<PointT> intersections(
            points,
            types::RowCol<size_t>(numRows, numCols),
            offset);

    // Draw all intersection pairs
    std::vector<typename Intersections<PointT>::Intersection> intersectionsVec;
    for (size_t row = 0, rowIdx = 0; row < numRows; ++row, rowIdx += numCols)
    {
        intersections.get(row, intersectionsVec);
        if (intersectionsVec.empty())
        {
            if (invert)
            {
                std::fill_n(out + rowIdx, numCols, color);
            }
        }
        else
        {
            for (size_t pair = 0; pair < intersectionsVec.size(); ++pair)
            {
                const typename Intersections<PointT>::Intersection&
                        intersection = intersectionsVec[pair];

                if (invert)
                {
                    std::fill_n(out + rowIdx, intersection.first, color);

                    std::fill_n(out + rowIdx + intersection.last + 1,
                                numCols - intersection.last - 1,
                                color);
                }
                else
                {
                    std::fill_n(out + rowIdx + intersection.first,
                                intersection.length(),
                                color);
                }
            }
        }
    }
}
}

#endif
