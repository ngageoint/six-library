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
#ifndef __SIX_TYPEDEFS_H__
#define __SIX_TYPEDEFS_H__

#include <import/except.h>
#include <import/mem.h>
#include <import/str.h>
#include <import/sys.h>
#include <import/math/linear.h>
#include <import/math/poly.h>
#include <import/nitf.hpp>
#include <import/io.h>
#include <limits>
#include "scene/Types.h"
#include "scene/FrameType.h"
#include "six/Enums.h"

namespace six
{
//! Vector types
typedef math::linear::VectorN<3> Vector3;

//! Date type
typedef nitf::DateTime DateTime;

typedef unsigned char UByte;

//! POLY type
typedef math::poly::OneD<double> Poly1D;

//! 2D_POLY
typedef math::poly::TwoD<double> Poly2D;

//! XYZ_POLY
typedef math::poly::OneD<Vector3> PolyXYZ;

//! Angle Magnitude
typedef scene::AngleMagnitude AngleMagnitude;

//! Lat Lon
typedef scene::LatLon LatLon;

//! Lat Lon Alt
typedef scene::LatLonAlt LatLonAlt;

typedef except::Context ValidationContext;

// These are heavily used and we don't want any mistakes
typedef types::RowCol<double> RowColDouble;
typedef types::RowCol<sys::SSize_T> RowColInt;

typedef scene::FrameType FrameType;

/*!
 *  2-D polynomial pair
 */

typedef types::RowCol<Poly2D> RowColPoly2D;

/*!
 *  2-D lat-lon sample spacing (Required for SIDD 0.1.1)
 */
typedef types::RowCol<LatLon> RowColLatLon;

/*!
 *  \struct DecorrType
 *  \brief Reuse type for ErrorStatistics
 *
 *  Just a simple pair for error stats
 */
struct DecorrType
{
    DecorrType(double ccz = 0, double dr = 0) :
        corrCoefZero(ccz), decorrRate(dr)
    {
    }

    DecorrType(const DecorrType& dt) :
        corrCoefZero(dt.corrCoefZero), decorrRate(dt.decorrRate)
    {
    }

    bool operator==(const DecorrType& rhs) const
    {
        return (corrCoefZero == rhs.corrCoefZero &&
                decorrRate == rhs.decorrRate);
    }

    bool operator!=(const DecorrType& rhs) const
    {
        return !(*this == rhs);
    }

    double corrCoefZero;
    double decorrRate;
};


/*!
 *  \struct ReferencePoint
 *  \brief Information grouping for a reference point
 *
 *  This object contains a vector (in ECEF, IOW tail from center of spheroid),
 *  and row-column position for a point.
 *
 */
struct ReferencePoint
{
    //!  ECEF location of point
    Vector3 ecef;

    //!  Row col pixel location of point
    RowColDouble rowCol;

    //!  (Optional) name.  Leave it blank if you don't need it
    std::string name;

    //!  Construct, init all fields at once (except optional name)
    ReferencePoint(double x = 0, double y = 0, double z = 0, double row = 0,
                   double col = 0) :
        rowCol(row, col)
    {
        ecef[0] = x;
        ecef[1] = y;
        ecef[2] = z;
    }
    //!  Alternate construct, still init all fields at once
    ReferencePoint(Vector3 xyz, RowColDouble rcd) :
        ecef(xyz), rowCol(rcd)
    {
    }

    //!  Are two points the same
    bool operator==(const ReferencePoint& x) const
    {
        return ecef == x.ecef && rowCol == x.rowCol;
    }

    //! Are two points different
    bool operator!=(const ReferencePoint& x) const
    {
        return !((*this) == x);
    }
};

/*!
*  \struct Corners
*  \brief Image corners
*
*  This represents the four image corners.  It's used rather than a vector
*  of LatLon's to make explicit which corner is which rather than assuming
*  they're stored in clockwise order.
*/
template <typename LatLonT>
struct Corners
{
    static const size_t NUM_CORNERS = 4;

    //! These can be used with getCorner() below
    static const size_t UPPER_LEFT = 0;
    static const size_t FIRST_ROW_FIRST_COL = UPPER_LEFT;
    static const size_t UPPER_RIGHT = 1;
    static const size_t FIRST_ROW_LAST_COL = UPPER_RIGHT;

    static const size_t LOWER_RIGHT = 2;
    static const size_t LAST_ROW_LAST_COL = LOWER_RIGHT;

    static const size_t LOWER_LEFT = 3;
    static const size_t LAST_ROW_FIRST_COL = LOWER_LEFT;

    //! Returns the corners in clockwise order
    const LatLonT& getCorner(size_t idx) const
    {
        switch (idx)
        {
        case UPPER_LEFT:
            return upperLeft;
        case UPPER_RIGHT:
            return upperRight;
        case LOWER_RIGHT:
            return lowerRight;
        case LOWER_LEFT:
            return lowerLeft;
        default:
            throw except::Exception(Ctxt("Invalid index " +
                                             str::toString(idx)));
        }
    }

    //! Returns the corners in clockwise order
    LatLonT& getCorner(size_t idx)
    {
        switch (idx)
        {
        case UPPER_LEFT:
            return upperLeft;
        case UPPER_RIGHT:
            return upperRight;
        case LOWER_RIGHT:
            return lowerRight;
        case LOWER_LEFT:
            return lowerLeft;
        default:
            throw except::Exception(Ctxt("Invalid index " +
                                             str::toString(idx)));
        }
    }

    bool operator==(const Corners& rhs) const
    {
        return (upperLeft == rhs.upperLeft &&
            upperRight == rhs.upperRight &&
            lowerRight == rhs.lowerRight &&
            lowerLeft == rhs.lowerLeft);
    }

    bool operator!=(const Corners& rhs) const
    {
        return !(*this == rhs);
    }

    LatLonT upperLeft;
    LatLonT upperRight;
    LatLonT lowerRight;
    LatLonT lowerLeft;
};

template <typename LatLonT> const size_t Corners<LatLonT>::NUM_CORNERS;
template <typename LatLonT> const size_t Corners<LatLonT>::UPPER_LEFT;
template <typename LatLonT> const size_t Corners<LatLonT>::FIRST_ROW_FIRST_COL;
template <typename LatLonT> const size_t Corners<LatLonT>::UPPER_RIGHT;
template <typename LatLonT> const size_t Corners<LatLonT>::FIRST_ROW_LAST_COL;
template <typename LatLonT> const size_t Corners<LatLonT>::LOWER_RIGHT;
template <typename LatLonT> const size_t Corners<LatLonT>::LAST_ROW_LAST_COL;
template <typename LatLonT> const size_t Corners<LatLonT>::LOWER_LEFT;
template <typename LatLonT> const size_t Corners<LatLonT>::LAST_ROW_FIRST_COL;

typedef Corners<LatLon> LatLonCorners;
typedef Corners<LatLonAlt> LatLonAltCorners;

}

#endif
