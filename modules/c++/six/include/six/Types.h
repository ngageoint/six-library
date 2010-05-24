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
typedef scene::LatLonAlt LatLon;
typedef scene::LatLonAlt LatLonAlt;

typedef except::Context ValidationContext;



enum DataType
{
    TYPE_UNKNOWN = 0, TYPE_COMPLEX, TYPE_DERIVED
};

enum DataClass
{
    DATA_UNKNOWN = 0, DATA_COMPLEX, DATA_DERIVED
};

enum PixelType
{
    PIXEL_TYPE_NOT_SET = 0,
    RE32F_IM32F,
    RE16I_IM16I,
    MONO8I,
    MONO16I,
    MONO8LU,
    RGB8LU,
    RGB24I
};

// For reads/writes
enum ByteSwapping
{
    BYTE_SWAP_OFF = 0, BYTE_SWAP_ON, BYTE_SWAP_AUTO
};

enum RowColEnum
{
    RC_ROW = 0, RC_COL = 1
};
enum XYZEnum
{
    XYZ_X = 0, XYZ_Y, XYZ_Z
};
enum MagnificationMethod
{
    MAG_NOT_SET, MAG_NEAREST_NEIGHBOR, MAG_BILINEAR
};
enum DecimationMethod
{
    DEC_NOT_SET,
    DEC_NEAREST_NEIGHBOR,
    DEC_BILINEAR,
    DEC_BRIGHTEST_PIXEL,
    DEC_LAGRANGE
};

enum PolarizationType
{
    POL_NOT_SET, POL_OTHER, POL_V, POL_H, POL_RHC, POL_LHC
};

enum DualPolarizationType
{
    DUAL_POL_NOT_SET,
    DUAL_POL_OTHER,
    DUAL_POL_V_V,
    DUAL_POL_V_H,
    DUAL_POL_H_V,
    DUAL_POL_H_H,
    DUAL_POL_RHC_RHC,
    DUAL_POL_RHC_LHC,
    DUAL_POL_LHC_LHC
};

enum EarthModelType
{
    EARTH_NOT_SET, EARTH_WGS84
};

enum FFTSign
{
    FFT_SIGN_NOT_SET = 0, FFT_SIGN_NEG = -1, FFT_SIGN_POS = 1
};

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
        return math::linear::equals(range, t.range) && 
            math::linear::equals(azimuth, t.azimuth);
    }


};

/*!
 *  \struct RowCol
 *  \brief Row/Col pair type
 *
 *  Templated pair storage for row/col values.  This is typedef'd
 *  below to prevent incorrect type assigment (e.g., RowCol<int> where
 *  type should be RowCol<long>)
 */


template<typename T> struct RowCol
{
    T row;
    T col;
    
    RowCol(T r = (T) 0.0, T c = (T) 0.0) :
        row(r), col(c) {}

    template<typename Other_T> RowCol(const RowCol<Other_T>& p)
    {
        row = p.row;
        col = p.col;
    }

    RowCol(const std::pair<T, T>& p)
    {
        row = p.first;
        col = p.second;
    }

    template<typename Other_T> RowCol& operator=(const RowCol<Other_T>& p)
    {
        if (this != &p)
        {
            row = (T)p.row;
            col = (T)p.col;
        }
        return *this;
    }

    RowCol& operator=(const std::pair<T, T>& p)
    {
        row = p.first;
        col = p.second;
        return *this;
    }

    
    template<typename Other_T> RowCol& operator+=(const RowCol<Other_T>& p)
    {
        row += (T)p.row;
        col += (T)p.col;
        return *this;
    }
    
    template<typename Other_T> RowCol operator+(const RowCol<Other_T>& p)
    {
        RowCol copy(*this);
        return copy += p;
    }
    
    template<typename Other_T> RowCol& operator-=(const RowCol<Other_T>& p)
    {
        row -= (T)p.row;
        col -= (T)p.col;
        return *this;
    }
    
    template<typename Other_T> RowCol operator-(const RowCol<Other_T>& p)
    {
        RowCol copy(*this);
        return copy -= p;
    }

    RowCol& operator+=(T scalar)
    {
        row += scalar;
        col += scalar;
        return *this;
    }

    RowCol operator+(T scalar)
    {
        RowCol copy(*this);
        return copy += scalar;
    }
    
    RowCol& operator-=(T scalar)
    {
        row -= scalar;
        col -= scalar;
        return *this;
    }

    RowCol operator-(T scalar)
    {
        RowCol copy(*this);
        return copy -= scalar;
    }

    RowCol& operator*=(T scalar)
    {
        row *= scalar;
        col *= scalar;
        return *this;
    }

    RowCol operator*(T scalar)
    {
        RowCol copy(*this);
        return copy *= scalar;
    }
    
    RowCol& operator/=(T scalar)
    {
        row /= scalar;
        col /= scalar;
        return *this;
    }

    RowCol operator/(T scalar)
    {
        RowCol copy(*this);
        return copy /= scalar;
    }
    
    /*!
     *  Compare the types considering that some
     *  specializations (e.g., double)
     *  are not exact
     */
    bool operator==(const RowCol<T>& p) const
    {
        return math::linear::equals(row, p.row) && 
            math::linear::equals(col, p.col);
    }


    bool operator!=(const RowCol<T>& p) const
    {
        return ! (RowCol::operator==(p));
    }
        
};

// These are heavily used and we dont want any mistakes
typedef RowCol<double> RowColDouble;
typedef RowCol<long> RowColInt;

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

typedef RowCol<Poly2D> RowColPoly2D;

/*!
 *  2-D lat-lon sample spacing (Required for SIDD 0.1.1)
 */
typedef RowCol<LatLon> RowColLatLon;

// Collection type
enum CollectType
{
    COLLECT_NOT_SET, COLLECT_MONOSTATIC, COLLECT_BISTATIC
};

enum ProjectionType
{
    PROJECTION_NOT_SET, 
    PROJECTION_PLANE, 
    PROJECTION_GEOGRAPHIC, 
    PROJECTION_CYLINDRICAL,
    PROJECTION_POLYNOMIAL
};

enum RegionType
{
    REGION_NOT_SET, REGION_SUB_REGION, REGION_GEOGRAPHIC_INFO
};

// Radar mode
enum RadarModeType
{
    MODE_INVALID,
    MODE_NOT_SET,
    MODE_SPOTLIGHT,
    MODE_STRIPMAP,
    MODE_DYNAMIC_STRIPMAP
};

enum SideOfTrackType
{
    SIDE_NOT_SET = 0,
    SIDE_LEFT = -1,
    SIDE_RIGHT = 1
};

enum BooleanType
{
    BOOL_NOT_SET = -1, BOOL_FALSE = 0, BOOL_TRUE = 1
};

enum AppliedType
{
    APPLIED_NOT_SET = -1, APPLIED_TRUE = 0, APPLIED_FALSE = 1
};

enum ComplexImagePlaneType
{
    PLANE_OTHER, PLANE_SLANT, PLANE_GROUND
};
enum ComplexImageGridType
{
    GRID_RGAZIM, GRID_RGZERO, GRID_CARTESIAN
};
enum DemodType
{
    DEMOD_NOT_SET, DEMOD_STRETCH, DEMOD_CHIRP
};
enum DisplayType
{
    DISPLAY_NOT_SET, DISPLAY_COLOR, DISPLAY_MONO
};
enum ImageFormationType
{
    IF_OTHER, IF_PFA, IF_RMA, IF_CSA, IF_RDA
};

enum SlowTimeBeamCompensationType
{
    SLOW_TIME_BEAM_NO, SLOW_TIME_BEAM_GLOBAL, SLOW_TIME_BEAM_SV
};
enum ImageBeamCompensationType
{
    IMAGE_BEAM_NO, IMAGE_BEAM_SV
};

enum AutofocusType
{
    AUTOFOCUS_NO, AUTOFOCUS_GLOBAL, AUTOFOCUS_SV
};

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

    enum GTKeys
    {
        GT_SICD_KEY = 52766,
        GT_SIDD_KEY = 58543
    };

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
        switch (type)
        {
        case RE32F_IM32F:
            return 8;

        case RE16I_IM16I:
            return 4;

        case MONO8I:
        case MONO8LU:
        case RGB8LU:
            return 1;
        case MONO16I:
            return 2;
        case RGB24I:
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
    RowCol<double> rowCol;

    //!  (Optional) name.  Leave it blank if you dont need it
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
    //!  Alternate construct, sitll init all fields at once
    ReferencePoint(Vector3 xyz, RowCol<double> rcd) :
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
 *  Since the spec doesnt order the corners
 *  like you might think, its a good idea to
 *  use these indices
 */
enum CornerIndex
{
    FIRST_ROW_FIRST_COL = 0,
    FIRST_ROW_LAST_COL,
    LAST_ROW_LAST_COL,
    LAST_ROW_FIRST_COL
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
    unsigned char* operator[](unsigned int i)
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
    AmplitudeTable() : LUT(256, sizeof(double)) {}

    //!  Destructor.  Relies on base class for LUT deletion
    virtual ~AmplitudeTable() {}
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


}

//std::ostream& operator<<(std::ostream& os, const six::Corners& corners);

#endif
