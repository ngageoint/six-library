/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
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
#ifndef __SIX_TYPES_H__
#define __SIX_TYPES_H__

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
#include "six/Enums.h"

/*!
 *  \macro SIX_LIB_VERSION 
 *
 *  This will be a string containing major and minor
 *  versions of the library.  Micro will probably not be included in
 *  this string.  For those comparisons, use specific number macros
 *
 *  This string should match the number of the FFDD and D&E docs
 */
#define SIX_LIB_VERSION "2.0"

/*!
 *  \macro SIX_MAJOR_VERSION_NUMBER 
 *
 *  This is the major version of the library.  It annotates major
 *  library or specification overhauls, in conjunction with the
 *  specification
 */
#define SIX_MAJOR_VERSION_NUMBER 2

/*!
 *  \macro SIX_MINOR_VERSION_NUMBER 
 *
 *  This is the minor version of the library.  It annotates minor
 *  library changes, in conjunction with updates to the file format
 */

#define SIX_MINOR_VERSION_NUMBER 0

/*!
 *  \macro SIX_MICRO_VERSION_NUMBER 
 *
 *  This is the micro version of the library.  It annotates minimal
 *  library or specification overhauls associated with the file
 *  format
 */
#define SIX_MICRO_VERSION_NUMBER 6

namespace six
{

static const char SCHEMA_PATH[] = "SIX_SCHEMA_PATH";

/*!
 * \class DESValidationException
 * \brief Throwable related to Six schema validation problems.
 */
DECLARE_EXCEPTION(DESValidation);

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

/*!
 *  \struct RangeAzimuth
 *  \brief Range/azimuth pair type
 *
 *
 *  Templated pair storage for  range azimuth values.
 *
 */
template<typename T> struct RangeAzimuth
{
    RangeAzimuth(T r = 0, T a = 0) :
        range(r), azimuth(a)
    {
    }
    T range;
    T azimuth;

    /*!
     *  Compare the types considering that some specializations (e.g., double)
     *  are not exact
     */
    bool operator==(const RangeAzimuth<T>& t) const
    {
        return math::linear::equals(range, t.range)
                && math::linear::equals(azimuth, t.azimuth);
    }

};


// These are heavily used and we don't want any mistakes
typedef types::RowCol<double> RowColDouble;
typedef types::RowCol<sys::SSize_T> RowColInt;

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

    double corrCoefZero;
    double decorrRate;
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
    const static sys::Uint64_T IS_SIZE_MAX; // = 9999999998LL;
    const static sys::Uint64_T GT_SIZE_MAX;

    const static unsigned short GT_XML_KEY;
    const static char GT_XML_TAG[];

    // DESVER is 2 byte BCS-N
    const static sys::Int32_T DES_VERSION;
    const static char DES_VERSION_STR[];

    const static char DES_USER_DEFINED_SUBHEADER_TAG[];
    const static char DES_USER_DEFINED_SUBHEADER_ID[];
    const static size_t DES_USER_DEFINED_SUBHEADER_LENGTH;

    enum
    {
        ILOC_MAX = 99999
    };

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
        switch (type.value)
        {
        case PixelType::RE32F_IM32F:
            return 8;

        case PixelType::RE16I_IM16I:
            return 4;

        case PixelType::MONO8I:
        case PixelType::MONO8LU:
        case PixelType::RGB8LU:
            return 1;
        case PixelType::MONO16I:
            return 2;
        case PixelType::RGB24I:
            return 3;

        default:
            throw except::Exception(Ctxt(FmtX("Unknown pixel type [%d]",
                                              (int) type)));
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
    Vector3 ecf;
    LatLonAlt llh;
};

/*!
 *  \struct LUT
 *  \brief Lookup table
 *
 *  This lookup table object has a number of entries (for pixel ops,
 *  this is usually 256), and an output space (size) which can be anything
 *  (e.g., 2 for short) depending on the type that is being housed
 *
 */
struct LUT
{
    mem::ScopedArray<unsigned char> table;
    size_t numEntries;
    size_t elementSize;

    //!  Initialize with a number of entries and known output space
    LUT(size_t entries, size_t outputSpace) :
        table(new unsigned char[entries * outputSpace]),
        numEntries(entries),
        elementSize(outputSpace)
    {
    }

    //!  Initialize with an existing LUT, which we clone
    LUT(const unsigned char* interleavedLUT,
        size_t entries,
        size_t outputSpace) :
        table(new unsigned char[entries * outputSpace]),
        numEntries(entries),
        elementSize(outputSpace)
    {
        memcpy(table.get(), interleavedLUT, numEntries * outputSpace);
    }

    virtual ~LUT()
    {
    }

    //!  Clone the LUT table
    virtual LUT* clone()
    {
        return new LUT(table.get(), numEntries, elementSize);
    }

    //!  Gives back a pointer at table[i * elementSize]
    unsigned char* operator[](size_t i)
    {
        return &(table[i * elementSize]);
    }

    //!  Gives back a pointer at table[i * elementSize]
    const unsigned char* operator[](size_t i) const
    {
        return &(table[i * elementSize]);
    }
};

/*!
 *  \struct AmplitudeTable
 *  \brief SICD 'AmpTable' parameter
 *
 *  This is a fixed size (256-element) LUT.  For AMP8I_PHS8I data,
 *  the amplitude and phase parts are stored as unsigned 8-bit integers.
 *  If an amplitude table is given, the amplitude component should be
 *  interpreted as an index into the AmpTable, ultimately yielding the
 *  double precision amplitude value
 */
struct AmplitudeTable : public LUT
{
    //!  Constructor.  Creates a 256-entry table
    AmplitudeTable() :
        LUT(256, sizeof(double))
    {
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
 *  \class MissingRequiredException
 *  \brief Throwable related to a required element being null,
 *         undefined, etc.
 */
DECLARE_EXCEPTION(MissingRequired)

}

#endif
