/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#define SIX_LIB_VERSION "0.1"

/*!
 *  \macro SIX_MAJOR_VERSION_NUMBER 
 *
 *  This is the major version of the library.  It annotates major
 *  library or specification overhauls, in conjunction with the
 *  specification
 */
#define SIX_MAJOR_VERSION_NUMBER 0

/*!
 *  \macro SIX_MINOR_VERSION_NUMBER 
 *
 *  This is the minor version of the library.  It annotates minor
 *  library changes, in conjunction with updates to the file format
 */

#define SIX_MINOR_VERSION_NUMBER 1

/*!
 *  \macro SIX_MICRO_VERSION_NUMBER 
 *
 *  This is the micro version of the library.  It annotates minimal
 *  library or specification overhauls associated with the file
 *  format
 */
#define SIX_MICRO_VERSION_NUMBER 1

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
typedef scene::RowCol<double> RowColDouble;
typedef scene::RowCol<long> RowColInt;

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

typedef scene::RowCol<Poly2D> RowColPoly2D;

/*!
 *  2-D lat-lon sample spacing (Required for SIDD 0.1.1)
 */
typedef scene::RowCol<LatLon> RowColLatLon;

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
    unsigned char* table;
    unsigned int numEntries;
    unsigned int elementSize;

    //!  Initialize with a number of entries and known output space
    LUT(int entries, int outputSpace)
    {
        numEntries = entries;
        elementSize = outputSpace;
        table = new unsigned char[numEntries * outputSpace];
    }

    //!  Initialize with an existing LUT, which we clone
    LUT(unsigned char* interleavedLUT, int entries, int outputSpace)
    {
        numEntries = entries;
        elementSize = outputSpace;
        table = new unsigned char[numEntries * outputSpace];
        memcpy(table, interleavedLUT, numEntries * outputSpace);
    }
    //!  Destroys the LUT table
    virtual ~LUT()
    {
        if (table)
        {
            delete[] table;
        }
    }
    //!  Clone the LUT table
    virtual LUT* clone()
    {
        LUT* lut = new LUT(numEntries, elementSize);
        for (unsigned int i = 0; i < numEntries * elementSize; ++i)
        {
            lut->table[i] = table[i];
        }
        return lut;
    }

    //!  Gives back a pointer at table[i * elementSize]
    unsigned char* operator[](unsigned int i) const
    {
        return &(table[i * elementSize]);
    }

};

/*!
 *  \struct AmplitudeTable
 *  \brief SICD 'AmpTable' parameter
 *
 *  This is a fixed size (256-element) LUT.  For AMP8I_PHS8I data,
 *  the amplitude and phase parts are store das unsigned 8-bit integers.
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

    //!  Destructor.  Relies on base class for LUT deletion
    virtual ~AmplitudeTable()
    {
    }
    //!  Clone (again)
    LUT* clone()
    {
        AmplitudeTable* lut = new AmplitudeTable();
        for (unsigned int i = 0; i < numEntries * elementSize; ++i)
        {
            lut->table[i] = table[i];
        }
        return lut;

    }
};

/*!
 *  \class MissingRequiredException
 *  \brief Throwable related to a required element being null,
 *         undefined, etc.
 */
DECLARE_EXCEPTION(MissingRequired)

}

//std::ostream& operator<<(std::ostream& os, const six::Corners& corners);

#endif
