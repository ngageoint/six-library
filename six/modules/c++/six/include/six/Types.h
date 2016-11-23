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
#include "scene/FrameType.h"
#include "six/Enums.h"
#include "six/Init.h"
#include "six/Typedefs.h"

namespace six
{
static const char SCHEMA_PATH[] = "SIX_SCHEMA_PATH";

/*!
 * \class DESValidationException
 * \brief Throwable related to Six schema validation problems.
 */
DECLARE_EXCEPTION(DESValidation)

/*!
 *  \struct Constants
 *  \brief Provide some enums for constants
 *
 *  Static class to provide constant enums.
 */
struct Constants
{
    //!  This is the upper bound of a NITF segment
    static const sys::Uint64_T IS_SIZE_MAX; // = 9999999998LL;
    static const sys::Uint64_T GT_SIZE_MAX;

    static const unsigned short GT_XML_KEY;
    static const char GT_XML_TAG[];

    // DESVER is 2 byte BCS-N
    static const sys::Int32_T DES_VERSION;
    static const char DES_VERSION_STR[];

    static const char DES_USER_DEFINED_SUBHEADER_TAG[];
    static const char DES_USER_DEFINED_SUBHEADER_ID[];
    static const sys::Uint64_T DES_USER_DEFINED_SUBHEADER_LENGTH;

    static const char SICD_DESSHSI[];
    static const char SIDD_DESSHSI[];

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

    SCP() :
       ecf(Init::undefined<Vector3>()),
       llh(Init::undefined<LatLonAlt>())
    {
    }

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
    size_t numEntries;
    size_t elementSize;

    //!  Initialize with a number of entries and known output space
    LUT(size_t entries, size_t outputSpace) :
        numEntries(entries),
        elementSize(outputSpace),
        table(new unsigned char[entries * outputSpace])
    {
    }

    //!  Initialize with an existing LUT, which we clone
    LUT(const unsigned char* interleavedLUT,
        size_t entries,
        size_t outputSpace) :
        numEntries(entries),
        elementSize(outputSpace),
        table(new unsigned char[entries * outputSpace])
    {
        memcpy(table.get(), interleavedLUT, numEntries * outputSpace);
    }

    virtual ~LUT()
    {
    }

    bool operator==(const LUT& rhs) const
    {
        return (numEntries == rhs.numEntries &&
                elementSize == rhs.elementSize &&
                std::equal(table.get(),
                           table.get() + numEntries * elementSize,
                           rhs.table.get()));
    }

    //!  Clone the LUT table
    virtual LUT* clone() const
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

    unsigned char* getTable()
    {
        return table.get();
    }

    const unsigned char* getTable() const
    {
        return table.get();
    }

protected:
    mem::ScopedArray<unsigned char> table;

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

    bool operator==(const AmplitudeTable& rhs) const
    {
        return *(dynamic_cast<const LUT*>(this)) == *(dynamic_cast<const LUT*>(&rhs));
    }
    bool operator!=(const AmplitudeTable& rhs) const
    {
        return !(*this == rhs);
    }
    AmplitudeTable* clone() const
    {
        AmplitudeTable* ret = new AmplitudeTable();
        for (size_t ii = 0; ii < numEntries; ++ii)
        {
            *(double*)(*ret)[ii] = *(double*)(*this)[ii];
        }
        return ret;
    }
};

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
DECLARE_EXCEPTION(MissingRequired)
}

#endif
