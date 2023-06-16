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
#pragma once
#ifndef SIX_six_Types_h_INCLUDED_
#define SIX_six_Types_h_INCLUDED_

#include <stdint.h>

#include <vector>
#include <limits>
#include <string>
#include <stdexcept>
#include <memory>

#include <types/Complex.h>
#include <import/except.h>
#include <import/mem.h>
#include <import/str.h>
#include <import/sys.h>
#include <import/math/linear.h>
#include <import/math/poly.h>
#include <import/io.h>

#include <import/nitf.hpp>

#include <scene/sys_Conf.h>
#include "scene/Types.h"
#include "scene/FrameType.h"
#include "six/Enums.h"

namespace six
{
    using zfloat = scene::zfloat;
    using zdouble = scene::zdouble;
    #if CODA_OSS_types_unique_ComplexInteger
    using zint16_t = std::complex<int16_t>; // TODO: types::zint16_t;
    #else
    using zint16_t = types::zint16_t;
    #endif
}
#include "six/AmplitudeTable.h"

namespace six
{
static const char SCHEMA_PATH[] = "SIX_SCHEMA_PATH";
// Gets the value of SIX_SCHEMA_PATH, or the compiled default if not set.
// If the resulting path doesn't exist, an exception is thrown.
extern std::string getSchemaPath(std::vector<std::string>&, bool tryToExpandIfNotFound = false);

/*!
 * \class DESValidationException
 * \brief Throwable related to Six schema validation problems.
 */
DECLARE_EXCEPTION(DESValidation);

//! Vector types
typedef math::linear::VectorN<3> Vector3;
typedef math::linear::VectorN<2> Vector2;

//! Date type
typedef nitf::DateTime DateTime;

using UByte = unsigned char;

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
typedef types::RowCol<ptrdiff_t> RowColInt;

typedef scene::FrameType FrameType;

/*!
 *  \struct DecorrType
 *  \brief Reuse type for ErrorStatistics
 *
 *  Just a simple pair for error stats
 */
struct DecorrType
{
    DecorrType() = default;
        DecorrType(double ccz, double dr = 0.0) :
        corrCoefZero(ccz), decorrRate(dr)
    {
    }

    DecorrType(const DecorrType& dt) : 
        DecorrType(dt.corrCoefZero, dt.decorrRate)
    {
    }

    bool operator==(const DecorrType& rhs) const
    {
        return (corrCoefZero == rhs.corrCoefZero &&
                decorrRate == rhs.decorrRate);
    }

    double corrCoefZero = 0.0;
    double decorrRate = 0.0;
};

/*!
 *  2-D polynomial pair
 */

typedef types::RowCol<Poly2D> RowColPoly2D;

/*!
 *  2-D lat-lon sample spacing (Required for SIDD 0.1.1)
 */
typedef types::RowCol<LatLon> RowColLatLon;

/*!
 *  \struct Constants
 *  \brief Provide some enums for constants
 *
 *  Static class to provide constant enums.
 */
struct Constants
{
    //!  This is the upper bound of a NITF segment
    static const uint64_t IS_SIZE_MAX; // = 9999999998LL;
    static const uint64_t GT_SIZE_MAX;
    static const size_t ILOC_MAX;

    static const unsigned short GT_XML_KEY;
    static const char GT_XML_TAG[];

    // DESVER is 2 byte BCS-N
    static const int32_t DES_VERSION;
    static const char DES_VERSION_STR[];

    static const char DES_USER_DEFINED_SUBHEADER_TAG[];
    static const char DES_USER_DEFINED_SUBHEADER_ID[];
    static const uint64_t DES_USER_DEFINED_SUBHEADER_LENGTH;

    static const char SICD_DESSHSI[];
    static const char SIDD_DESSHSI[];

    static const double EXCESS_BANDWIDTH_FACTOR;

    /*!
     *  If we have the PixelType, we know exactly how many bytes
     *  per pixel that should be.
     *
     *  \param type
     *  \return The number of bytes per pixel for this type
     *  \throw Exception if type unknown
     */
    inline static int getNumBytesPerPixel(PixelType type)
    {
        switch (type)
        {
        case PixelType::RE32F_IM32F:
        {
            // Each pixel is stored as a pair of numbers that represent the realand imaginary
            // components. Each component is stored in a 32-bit IEEE floating point format (4
            // bytes per component, 8 bytes per pixel).
            static_assert(sizeof(six::zfloat) == 8, "RE32F_IM32F should be two floats");
            return 8;
        }

        case PixelType::RE16I_IM16I:
        {
            // Each pixel is stored as a pair of numbers that represent the real and imaginary 
            // components. Each component is stored in a 16-bit signed integer in 2's 
            // complement format (2 bytes per component, 4 bytes per pixel). 
            static_assert(sizeof(six::zint16_t) == 4, "RE16I_IM16I should be two 16-bit integers");
            return 4;
        }

        case PixelType::AMP8I_PHS8I:
        {
            // Each pixel is stored as a pair of numbers that represent the amplitude and phase
            // components. Each component is stored in an 8-bit unsigned integer (1 byte per 
            // component, 2 bytes per pixel). 
            static_assert(sizeof(std::pair<uint8_t, uint8_t>) == 2, "AMP8I_PHS8I should be two 8-bit integers");
            return 2;
        }

        case PixelType::MONO8I:
        case PixelType::MONO8LU:
        case PixelType::RGB8LU:
            return 1;
        case PixelType::MONO16I:
            return 2;
        case PixelType::RGB24I:
            return 3;

        default:
            throw except::Exception(Ctxt(FmtX("Unknown pixel type [%d]", (int) type)));
        }
    }

};

/*!
 *  \struct ReferencePoint
 *  \brief Information grouping for a reference point
 *
 *  This object contains a vector (in ECEF, IOW tail from center of spheroid),
 *  and row-column position for a point.
 *
 */
struct ReferencePoint final
{
    //!  ECEF location of point
    Vector3 ecef;

    //!  Row col pixel location of point
    RowColDouble rowCol{ 0.0, 0.0 };

    //!  (Optional) name.  Leave it blank if you don't need it
    std::string name;

    //!  Construct, init all fields at once (except optional name)
    ReferencePoint(double x = 0, double y = 0, double z = 0, double row = 0,
                   double col = 0) noexcept :
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

    //! Are two point different
    bool operator!=(const ReferencePoint& x) const
    {
        return !((*this) == x);
    }
};

/*!
 *  \struct SCP
 *  \brief SICD Scene Center Point (SCP)
 *
 *  The SCP in ECEF (ecf, to match SICD convention) and LLH (or LatLonAlt
 *  as we refer to it).
 */
struct SCP
{
    SCP();

    Vector3 ecf;
    LatLonAlt llh;

    bool operator==(const SCP& rhs) const
    {
        return ecf == rhs.ecf && llh == rhs.llh;
    }

    bool operator!=(const SCP& rhs) const
    {
        return !(*this == rhs);
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
    Corners();
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
                                             std::to_string(idx)));
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
                                             std::to_string(idx)));
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

/*!
 *   \enum ImageMode
 *
 *   Enumeration used to represent frame vs. scan mode.  Note that this is a
 *   simpler version of RadarModeType.
 */
enum ImageMode
{
    FRAME_MODE = 0,
    SCAN_MODE
};

/*!
 * \param radarMode The radar mode type
 *
 * \return Whether this corresponds to frame or scan mode
 */
ImageMode getImageMode(RadarModeType radarMode);

/*!
 *  \class MissingRequiredException
 *  \brief Throwable related to a required element being null,
 *         undefined, etc.
 */
DECLARE_EXCEPTION(MissingRequired);
}

#endif // SIX_six_Types_h_INCLUDED_
