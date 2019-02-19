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
#include <sstream>
#include <limits>

#include <sys/Conf.h>
#include <except/Exception.h>
#include <math/ConvexHull.h>

#include <polygon/Intersections.h>

#include <polygon/PolygonMask.h>

namespace polygon
{
PolygonMask::PolygonMask(MarkModesEnum markMode,
                         const types::RowCol<size_t>& dims) :
    mMarkMode(markMode),
    mDims(dims)
{
    if (mMarkMode != MARK_ALL_TRUE && mMarkMode != MARK_ALL_FALSE)
    {
        throw except::Exception(Ctxt("Invalid mark mode"));
    }
}

PolygonMask::PolygonMask(const bool* mask,
                         const types::RowCol<size_t>& dims) :
    mMarkMode(MARK_USING_POINTS),
    mRanges(new types::Range[dims.row]),
    mDims(dims)
{
    // TODO: Could thread this but if we really care about performance,
    //       shouldn't be using this constructor anyway

    for (size_t row = 0, rowIdx = 0; row < dims.row; ++row, rowIdx += dims.col)
    {
        // Find the first valid col in this row, searching left to right
        size_t start(std::numeric_limits<size_t>::max());
        for (size_t col = 0; col < dims.col; ++col)
        {
            if (mask[rowIdx + col])
            {
                start = col;
                break;
            }
        }

        if (start == std::numeric_limits<size_t>::max())
        {
            // There were no valid pixels in this entire row
            mRanges[row] = types::Range();
        }
        else
        {
            // Find the last valid col in this row, searching right to left
            size_t last(start);
            for (size_t col = dims.col - 1; ; --col)
            {
                if (mask[rowIdx + col])
                {
                    last = col;
                    break;
                }
            }

            mRanges[row] = types::Range(start, last - start + 1);
        }
    }

    checkForAllTrueOrFalseRanges();
}

PolygonMask::PolygonMask(const std::vector<types::RowCol<double> >& points,
                         const types::RowCol<size_t>& dims,
                         types::RowCol<sys::SSize_T> offset) :
    mMarkMode(MARK_USING_POINTS),
    mDims(dims)
{
    // Determine intersections
    if (points.empty())
    {
        mDims.row = 0;
    }
    else
    {
        // Need to get the convex hull of the input points,
        // because the code currently cannot handle more
        // than two intersections (left and right) per row.
        std::vector<types::RowCol<double> > rawPoints = points;
        std::vector<types::RowCol<double> > convexHullPoints;
        math::ConvexHull<double> convexHull(rawPoints, convexHullPoints);
            
        const Intersections<double>
                intersections(convexHullPoints, mDims, offset);
        mRanges.reset(new types::Range[mDims.row]);

        std::vector<Intersections<double>::Intersection> intersectionsVec;
        for (size_t row = 0; row < mDims.row; ++row)
        {
            // We know we have a convex polygon
            intersections.get(row, intersectionsVec);
            if (intersectionsVec.empty())
            {
                mRanges[row] = types::Range(); // Empty range
            }
            else if (intersectionsVec.size() == 1)
            {
                mRanges[row] = types::Range(intersectionsVec[0].first,
                                            intersectionsVec[0].length());
            }
            else
            {
                // We are using a convex polygon so this should never
                // happen.
                std::ostringstream ostr;
                ostr << "Requires a convex polygon but these points produced "
                     << intersectionsVec.size() << " intersections for row "
                     << row;
                throw except::Exception(Ctxt(ostr.str()));
            }
        }

        checkForAllTrueOrFalseRanges();
    }
}

void PolygonMask::checkForAllTrueOrFalseRanges()
{
    bool allRangesAreEmpty = true;
    bool allRangesAreFull = true;

    for (size_t row = 0; row < mDims.row; ++row)
    {
        const types::Range& range(mRanges[row]);
        if (allRangesAreEmpty && !range.empty())
        {
            allRangesAreEmpty = false;
            if (!allRangesAreFull)
            {
                break;
            }
        }

        if (allRangesAreFull &&
            (range.mStartElement != 0 ||
             range.mNumElements != mDims.col))
        {
            allRangesAreFull = false;
            if (!allRangesAreEmpty)
            {
                break;
            }
        }
    }

    if (allRangesAreEmpty)
    {
        mMarkMode = MARK_ALL_FALSE;
        mRanges.reset();
    }
    else if (allRangesAreFull)
    {
        mMarkMode = MARK_ALL_TRUE;
        mRanges.reset();
    }
}

size_t PolygonMask::getNumMaskedPixels() const
{
    switch (mMarkMode)
    {
    case MARK_ALL_TRUE:
        return mDims.area();
    case MARK_ALL_FALSE:
        return 0;
    case MARK_USING_POINTS:
    {
        size_t numMaskedPixels(0);
        for (size_t row = 0; row < mDims.row; ++row)
        {
            numMaskedPixels += mRanges[row].mNumElements;
        }

        return numMaskedPixels;
    }
    default:
        throw except::Exception(Ctxt("Unexpected mark mode"));
    }
}
}
