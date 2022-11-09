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
#ifndef __POLYGON_INTERSECTIONS_H__
#define __POLYGON_INTERSECTIONS_H__

#include <vector>
#include <algorithm>
#include <cmath>

#include <types/RowCol.h>

namespace polygon
{
/*!
 * \class Intersections
 * \brief Given a list of points, draws lines between them to create a
 * polygon.  Then, given a row, reports the columns that the resulting
 * polygon intersects that row at.
 * \tparam PointT Data type of points.  Must be a floating type (float/double)
 * to work properly
 *
 * \note Implementation uses horizontal "scan lines" to determine
 * whether or not an element is inside the polygon's edges.  As a result,
 * polygon vertices cannot be exactly on the scanlines.  The polygon vertices
 * are bumped "down" if they have integral row indices to avoid falling
 * directly on a scanline.  Since the fill algorithm is conservative,
 * horizontal-top-edges will typically not be filled in.  For instance, a
 * square will have 1 pixel of its top edge missing.
 */
template <typename PointT>
class Intersections
{
public:
    /*!
     * \class Intersection
     * \brief Represents an intersection
     */
    struct Intersection
    {
        //! First (inclusive) column
        size_t first;

        //! Last (inclusive) column
        size_t last;

        //! \return Number of columns
        size_t length() const
        {
            return last - first + 1;
        }
    };

    /*!
     * \param points List of points
     * \param dims Dimensions to compute a polygon over.  If the provided
     * points are not completely within these dimensions, the polygon will
     * simply be truncated at the dimension boundary.
     * \param offset Offset to apply to all input points.  Positive row
     * value shifts the polygon up (equivalently the frame shifts down), and
     * positive col value shifts the polygon left (equiv. the frame shifts
     * right).  Defaults to no offset.
     */
    Intersections(const std::vector<types::RowCol<PointT> >& points,
                  const types::RowCol<size_t>& dims,
                  types::RowCol<sys::SSize_T> offset =
                          types::RowCol<sys::SSize_T>(0, 0)) :
        mDims(dims)
    {
        if (!points.empty())
        {
            computeIntersections(points, dims, offset);
        }
    }

    /*!
     * \param row Row to get intersections for
     * \param[out] intersections Polygon intersections for this row.  If the
     * vector is empty, that means the polygon does not intersect at this row.
     */
    void get(size_t row, std::vector<Intersection>& intersections) const
    {
        // Clear out any old intersections in the vector we're given
        intersections.clear();

        if (row >= mIntersections.size())
        {
            // Could be because of no points or this is a row greater than
            // we computed for
            return;
        }

        const std::vector<PointT>& interRow(mIntersections[row]);
        if (interRow.empty() || interRow.size() % 2 != 0)
        {
            // No intersections on this row
            return;
        }

        const size_t numPairs = interRow.size() / 2;
        for (size_t pair = 0, idx = 0; pair < numPairs; ++pair)
        {
            double first = static_cast<double>(interRow[idx++]);
            double last = static_cast<double>(interRow[idx++]);

            // If the pair of intersections lies outside of the image,
            // there is no intersection for this pair.
            const auto lastCol = static_cast<double>(mDims.col - 1);
            if ((first < 0.0 && last < 0.0) ||
                (first > lastCol && last > lastCol))
            {
                continue;
            }

            // Clamp the intersections to the image boundary
            first = std::max(0.0, std::min(lastCol, first));
            last = std::max(0.0, std::min(lastCol, last));

            Intersection intersection;
            intersection.first = static_cast<size_t>(std::ceil(first));
            intersection.last = static_cast<size_t>(std::floor(last));

            if(intersection.last > intersection.first)
            {
                intersections.push_back(intersection);
            }
            else
            {
                if (first < last)
                {
                    // This happens when first = 55.01, last = 55.99
                    // Then intersection.first = 56, intersection.last = 55
                    // We should count 55 as an intersection
                    intersection.first = intersection.last;
                    intersections.push_back(intersection);
                }
            }
        }
    }

private:
    void orderPoints(PointT& r0, PointT& c0, PointT& r1, PointT& c1)
    {
        if (r0 > r1)
        {
            std::swap(r0, r1);
            std::swap(c0, c1);
        }
    }

    void computeIntersections(
            const std::vector<types::RowCol<PointT> >& points,
            const types::RowCol<size_t>& dims,
            types::RowCol<sys::SSize_T> offset)
    {
        std::vector<types::RowCol<PointT> > shiftedPoints(points);
        for (size_t ii = 0; ii < shiftedPoints.size(); ++ii)
        {
            // Get the polygon points with respect to the offset
            shiftedPoints[ii].row -= offset.row;
            shiftedPoints[ii].col -= offset.col;

            // TODO: The original implementation did this (plus subtracted 0.5
            //       from each row and col which we're not doing here).
            //       Without this small delta, if you have points that are
            //       right on a row, we skip drawing some rows
            //       (test_draw_polygon will illustrate this). I wonder if we
            //       could tweak the sl0 and sl1 logic to avoid this.
            PointT& rowPoint(shiftedPoints[ii].row);
            if (std::floor(rowPoint) == rowPoint)
            {
                // Add small amount to move it off the scan line
                rowPoint += 0.0001;
            }
        }

        // We need to get all scanline intersections of polygon edges
        mIntersections.resize(dims.row);

        const sys::SSize_T lastRow = static_cast<sys::SSize_T>(dims.row) - 1;

        for (size_t ii = 0; ii < shiftedPoints.size(); ++ii)
        {
            PointT r1(shiftedPoints[ii].row);
            PointT c1(shiftedPoints[ii].col);

            const size_t idx = (ii == 0) ? shiftedPoints.size() - 1 : ii - 1;
            PointT r0(shiftedPoints[idx].row);
            PointT c0(shiftedPoints[idx].col);

            // Skip horizontal lines
            if (r1 == r0)
            {
                continue;
            }

            // Make sure r0 < r1
            orderPoints(r0, c0, r1, c1);

            // Find first and last scan line that we cross
            sys::SSize_T sl0 = static_cast<sys::SSize_T>(std::ceil(
                    static_cast<double>(r0)));

            sys::SSize_T sl1 = static_cast<sys::SSize_T>(std::floor(
                    static_cast<double>(r1)));

            // Skip rows where the edge intersects scan line outside of image
            // Have to do this carefully though... we can't just restrict both
            // sl0 and sl1 to be in the [0, lastRow] range because if they were
            // both negative or both > lastRow, we'd end up capping them both
            // in-bounds and going through the for loop below one time, putting
            // on a bogus point which is going to mess us up later.
            // Instead, first we do a sanity check to see if both points are out
            // of bounds... if so, we skip to the next set of points.
            if (sl0 > lastRow || sl1 < 0)
            {
                continue;
            }

            // Now that we know that at least some of our data is in bounds
            // we CAN cap to [0, lastRow].
            if (sl0 < 0)
            {
                sl0 = 0;
            }
            if (sl1 > lastRow)
            {
                sl1 = lastRow;
            }

            // Calculate rate of change
            const PointT dcdr = (c1 - c0) / (r1 - r0);

            // Calculate intersections
            // We do not clamp to the image dimensions at this time -- this is
            // done inside get(), and allows us to determine if intersection
            // pairs lie fully outside of the image. Clamping here ambiguates
            // that situation.
            for (sys::SSize_T row = sl0; row <= sl1; ++row)
            {
                const PointT delt = row - r0;
                const PointT sli = c0 + delt * dcdr;
                mIntersections[row].push_back(sli);
            }
        }

        // We're going to need these sorted eventually - might as well do it
        // now
        for (size_t row = 0; row < mIntersections.size(); ++row)
        {
            std::vector<PointT>& interRow(mIntersections[row]);
            if (interRow.size() % 2 == 0)
            {
                std::sort(interRow.begin(), interRow.end());
            }
        }
    }

private:
    const types::RowCol<size_t> mDims;
    std::vector<std::vector<PointT> > mIntersections;
};
}

#endif
