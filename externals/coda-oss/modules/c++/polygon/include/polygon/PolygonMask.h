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
#ifndef __POLYGON_POLYGON_MASK_H__
#define __POLYGON_POLYGON_MASK_H__

#include <vector>

#include <sys/Conf.h>
#include <mem/ScopedArray.h>
#include <types/RowCol.h>
#include <types/Range.h>

namespace polygon
{
/*!
 * \class PolygonMask
 * \brief Acts as a mask for a convex polygon without actually allocating a
 * bool buffer to draw it.
 */
struct PolygonMask final
{
    enum MarkModesEnum
    {
        MARK_ALL_TRUE = 0,
        MARK_ALL_FALSE,
        MARK_USING_POINTS
    };

    /*!
     * Use this for a polygon that should always report true or false
     *
     * \param markMode Whether each pixel should always report true or false
     * \param dims Dimensions.  Pixels outside of these dimensions will still
     * get reported as outside the polygon.
     */
    PolygonMask(MarkModesEnum markMode,
                const types::RowCol<size_t>& dims);

    /*!
     * \param mask An existing polygon mask where true means a valid pixel
     * and false means an invalid pixel.  Mask is assumed to form a convex
     * polygon (this is NOT checked).  Note that it is more efficient to
     * use the constructor with a list of points than it is to take that list
     * of points, call drawPolygon() to create a mask, then call this
     * constructor.
     * \param dims Dimensions the polygon should be considered over.  Pixels
     * outside of these dimensions will get reported as outside the polygon.
     */
    PolygonMask(const bool* mask,
                const types::RowCol<size_t>& dims);

    /*!
     * \param points Vector specifying the convex polygon.
     * \param dims Dimensions the polygon should be considered over.  Pixels
     * outside of these dimensions will get reported as outside the polygon.
     * \param offset Number of rows and cols to offset polygon. Positive row
     * value shifts the polygon up (equivalently the frame shifts down), and
     * positive col value shifts the polygon left (equiv. the frame shifts
     * right).  Defaults to no offset.
     *
     * \throws Exception if polygon is concave
     */
    PolygonMask(const std::vector<types::RowCol<double> >& points,
                const types::RowCol<size_t>& dims,
                types::RowCol<sys::SSize_T> offset =
                        types::RowCol<sys::SSize_T>(0, 0));


    PolygonMask(const PolygonMask&) = delete;
    PolygonMask& operator=(const PolygonMask&) = delete;
    PolygonMask(PolygonMask&&) = delete;
    PolygonMask& operator=(PolygonMask&&) = delete;

    /*!
     * \param row Row to query
     *
     * \return The range for this row that is inside the polygon
     */
    types::Range getRange(size_t row) const
    {
        if (row >= mDims.row || mMarkMode == MARK_ALL_FALSE)
        {
            return types::Range(); // Empty range
        }
        else
        {
            if (mMarkMode == MARK_ALL_TRUE)
            {
                return types::Range(0, mDims.col);
            }
            else
            {
                return mRanges[row];
            }
        }
    }

    /*!
     * \param point Point to query
     *
     * \return True if the point is inside the polygon, false otherwise
     */
    bool isInPolygon(const types::RowCol<size_t>& point) const
    {
        return getRange(point.row).contains(point.col);
    }

    /*!
     * \param row Row to query
     * \param col Column to query
     *
     * \return True if the point is inside the polygon, false otherwise
     */
    bool isInPolygon(size_t row, size_t col) const
    {
        return isInPolygon(types::RowCol<size_t>(row, col));
    }

    //! \return The mark mode
    MarkModesEnum getMarkMode() const
    {
        return mMarkMode;
    }

    /*!
     * \return The number of masked pixels in the specified dimensions
     */
    size_t getNumMaskedPixels() const;

    /*!
     * \return The number of unmasked pixels in the specified dimensions
     */
    size_t getNumUnmaskedPixels() const
    {
        return mDims.area() - getNumMaskedPixels();
    }

private:
    void checkForAllTrueOrFalseRanges();

private:
    MarkModesEnum mMarkMode;
    mem::ScopedArray<types::Range> mRanges;
    types::RowCol<size_t> mDims;
};
}

#endif
