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
#define SIX_MICRO_VERSION_NUMBER 0

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
    XYZ_X, XYZ_Y, XYZ_Z
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
 *  Pair storage for range azimuth
 *
 */
struct RangeAzimuth
{
    RangeAzimuth(double r = 0, double a = 0) :
        range(r), azimuth(a)
    {
    }
    double range;
    double azimuth;
};

/*!
 *  \struct RowColInt
 *  \brief Row/Col pair type
 *
 *  Pair storage for row/col unsigned integer.
 *
 */

struct RowColInt
{
    RowColInt(unsigned long r = 0, unsigned long c = 0) :
        row(r), col(c)
    {
    }
    unsigned long row;
    unsigned long col;
};

/*!
 *  \struct RowColDouble
 *  \brief Row/Col pair type
 *
 *  Pair storage for row/col double
 */
struct RowColDouble
{
    RowColDouble(double r = 0, double c = 0) :
        row(r), col(c)
    {
    }
    RowColDouble(const RowColDouble & rc) :
        row(rc.row), col(rc.col)
    {
    }
    double row;
    double col;

    bool operator==(const RowColDouble& x) const
    {
        return row == x.row && col == x.col;
    }

};
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
 *  \struct RowColPoly2D
 *  \brief Pair for 2D polys
 *
 *  2-Dimensional polynomial pair
 */
struct RowColPoly2D
{
    RowColPoly2D()
    {
    }

    /*     RowColPoly2D(const Poly2D& r, const Poly2D& c) : */
    /*         row(r), col(c) */
    /*     { */
    /*     } */
    /*     RowColPoly2D(const RowColPoly2D & rc) : */
    /*         row(rc.row), col(rc.col) */
    /*     { */
    /*     } */
    Poly2D row;
    Poly2D col;
};

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
    PROJECTION_CYLINDRICAL
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
    const static sys::Uint64_T IS_SIZE_MAX;// = 9999999998LL;
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
    RowColDouble rowCol;

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
    ReferencePoint(Vector3 xyz, RowColDouble rcd) :
        ecef(xyz), rowCol(rcd)
    {
    }
    
    //!  Are two points the same
    bool operator==(const ReferencePoint& x) const
    {
        return ecef[0] == x.ecef[0] && ecef[1] == x.ecef[1] && ecef[2]
                == x.ecef[2] && rowCol == x.rowCol;
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
 *  \struct Corners
 *  \brief Generic cross-format model for geo-corners
 *
 *  This model object is used by both SICD and SIDD reader/writers
 *  but frees the user from the details of the implementation of the
 *  XML.  Note that the enumeration defines the winding order (clockwise)
 *
 */
struct Corners
{
    scene::LatLonAlt corner[4];

    /*!
     *  Since the spec doesnt order the corners
     *  like you might think, its a good idea to
     *  use these indices
     */
    enum
    {
        FIRST_ROW_FIRST_COL = 0,
        FIRST_ROW_LAST_COL,
        LAST_ROW_LAST_COL,
        LAST_ROW_FIRST_COL
    };

    //!  Default constructor
    Corners()
    {
    }

    /*!
     *  Constructor that allows the four corners
     *  to be set up front.
     *
     */
    Corners(scene::LatLonAlt firstRowFirstCol,
            scene::LatLonAlt firstRowLastCol, scene::LatLonAlt lastRowLastCol,
            scene::LatLonAlt lastRowFirstCol)
    {
        corner[0] = firstRowFirstCol;
        corner[1] = firstRowLastCol;
        corner[2] = lastRowLastCol;
        corner[3] = lastRowFirstCol;
    }
    ~Corners()
    {
    }

    /*!
     *  Get latitude of corner i in degrees.  The 
     *  enums above may be used for the parameter
     */
    double getLat(int i) const
    {
        return corner[i].getLat();
    }

    /*!
     *  Get longitude of corner i in degrees.  The 
     *  enums above may be used for the parameter
     */
    double getLon(int i) const
    {
        return corner[i].getLon();
    }

    /*!
     *  Get latitude of corner i in radians.  The 
     *  enums above may be used for the parameter
     */
    double getLatRadians(int i) const
    {
        return corner[i].getLatRadians();
    }

    /*!
     *  Get longitude of corner i in radians.  The 
     *  enums above may be used for the parameter
     */
    double getLonRadians(int i) const
    {
        return corner[i].getLonRadians();
    }

    /*!
     *  Set latitude of corner i in degrees.  The 
     *  enums above may be used for the parameter
     */
    void setLat(int i, double lat)
    {
        corner[i].setLat(lat);
    }

    /*!
     *  Set longitude of corner i in degrees.  The 
     *  enums above may be used for the parameter
     */
    void setLon(int i, double lon)
    {
        corner[i].setLon(lon);
    }

    /*!
     *  Set latitude of corner i in radians.  The 
     *  enums above may be used for the parameter
     */
    void setLatRadians(int i, double lat)
    {
        corner[i].setLatRadians(lat);
    }

    /*!
     *  Set longitude of corner i in radians.  The 
     *  enums above may be used for the parameter
     */
    void setLonRadians(int i, double lon)
    {
        corner[i].setLonRadians(lon);
    }

    //!  Are these corners the same as this?
    bool operator==(const Corners& x) const
    {
        return corner[0] == x.corner[0] && corner[1] == x.corner[1]
                && corner[2] == x.corner[2] && corner[3] == x.corner[3];
    }

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

//std::ostream& operator<<(std::ostream& os, const RowColType& rc);
std::ostream& operator<<(std::ostream& os, const scene::LatLonAlt& latLonAlt);
std::ostream& operator<<(std::ostream& os, const six::Corners& corners);

#endif
