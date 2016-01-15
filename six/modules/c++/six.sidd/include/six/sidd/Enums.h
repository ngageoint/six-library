/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_SIDD_ENUMS_H__
#define __SIX_SIDD_ENUMS_H__

#include <import/except.h>
#include <import/str.h>
#include <import/sys.h>

// -----------------------------------------------------------------------------
// This file is auto-generated - please do NOT edit directly
// -----------------------------------------------------------------------------

namespace six
{
namespace sidd
{
const int NOT_SET_VALUE = 2147483647; //std::numeric_limits<int>::max()

/*!
 *  \struct BandEqualizationAlgorithm 
 *
 *  Enumeration used to represent BandEqualizationAlgorithms
 */
struct BandEqualizationAlgorithm
{
    //! The enumerations allowed
    enum
    {
        LUT_1D = 0,
        NOT_SET = six::NOT_SET_VALUE
    };

    //! Default constructor
    BandEqualizationAlgorithm(){ value = NOT_SET; }

    //! string constructor
    BandEqualizationAlgorithm(std::string s)
    {
        if (s == "LUT_1D")
            value = LUT_1D;
        else if (s == "NOT_SET")
            value = NOT_SET;
        else
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %s", s.c_str())));
    }

    //! int constructor
    BandEqualizationAlgorithm(int i)
    {
        switch(i)
        {
        case 0:
            value = LUT_1D;
            break;
        case six::NOT_SET_VALUE:
            value = NOT_SET;
            break;
        default:
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", i)));
        }
    }

    //! destructor
    ~BandEqualizationAlgorithm(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return std::string("LUT_1D");
        case six::NOT_SET_VALUE:
            return std::string("NOT_SET");
        default:
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
        }
    }

    //! assignment operator
    BandEqualizationAlgorithm& operator=(const BandEqualizationAlgorithm& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const BandEqualizationAlgorithm& o) const { return value == o.value; }
    bool operator!=(const BandEqualizationAlgorithm& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    BandEqualizationAlgorithm& operator=(const int& o) { value = o; return *this; }
    bool operator<(const BandEqualizationAlgorithm& o) const { return value < o.value; }
    bool operator>(const BandEqualizationAlgorithm& o) const { return value > o.value; }
    bool operator<=(const BandEqualizationAlgorithm& o) const { return value <= o.value; }
    bool operator>=(const BandEqualizationAlgorithm& o) const { return value >= o.value; }
    operator int() const { return value; }
    operator std::string() const { return toString(); }

    static size_t size() { return 2; }

    int value;

};


/*!
 *  \struct CoordinateSystemType 
 *
 *  Enumeration used to represent CoordinateSystemTypes
 */
struct CoordinateSystemType
{
    //! The enumerations allowed
    enum
    {
        GCS = 0,
        UTM = 1,
        NOT_SET = six::NOT_SET_VALUE
    };

    //! Default constructor
    CoordinateSystemType(){ value = NOT_SET; }

    //! string constructor
    CoordinateSystemType(std::string s)
    {
        if (s == "GCS")
            value = GCS;
        else if (s == "UTM")
            value = UTM;
        else if (s == "NOT_SET")
            value = NOT_SET;
        else
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %s", s.c_str())));
    }

    //! int constructor
    CoordinateSystemType(int i)
    {
        switch(i)
        {
        case 0:
            value = GCS;
            break;
        case 1:
            value = UTM;
            break;
        case six::NOT_SET_VALUE:
            value = NOT_SET;
            break;
        default:
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", i)));
        }
    }

    //! destructor
    ~CoordinateSystemType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return std::string("GCS");
        case 1:
            return std::string("UTM");
        case six::NOT_SET_VALUE:
            return std::string("NOT_SET");
        default:
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
        }
    }

    //! assignment operator
    CoordinateSystemType& operator=(const CoordinateSystemType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const CoordinateSystemType& o) const { return value == o.value; }
    bool operator!=(const CoordinateSystemType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    CoordinateSystemType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const CoordinateSystemType& o) const { return value < o.value; }
    bool operator>(const CoordinateSystemType& o) const { return value > o.value; }
    bool operator<=(const CoordinateSystemType& o) const { return value <= o.value; }
    bool operator>=(const CoordinateSystemType& o) const { return value >= o.value; }
    operator int() const { return value; }
    operator std::string() const { return toString(); }

    static size_t size() { return 3; }

    int value;

};


/*!
 *  \struct DRAType 
 *
 *  Enumeration used to represent DRATypes
 */
struct DRAType
{
    //! The enumerations allowed
    enum
    {
        AUTO = 0,
        MANUAL = 1,
        NONE = 2,
        NOT_SET = six::NOT_SET_VALUE
    };

    //! Default constructor
    DRAType(){ value = NOT_SET; }

    //! string constructor
    DRAType(std::string s)
    {
        if (s == "AUTO")
            value = AUTO;
        else if (s == "MANUAL")
            value = MANUAL;
        else if (s == "NONE")
            value = NONE;
        else if (s == "NOT_SET")
            value = NOT_SET;
        else
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %s", s.c_str())));
    }

    //! int constructor
    DRAType(int i)
    {
        switch(i)
        {
        case 0:
            value = AUTO;
            break;
        case 1:
            value = MANUAL;
            break;
        case 2:
            value = NONE;
            break;
        case six::NOT_SET_VALUE:
            value = NOT_SET;
            break;
        default:
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", i)));
        }
    }

    //! destructor
    ~DRAType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return std::string("AUTO");
        case 1:
            return std::string("MANUAL");
        case 2:
            return std::string("NONE");
        case six::NOT_SET_VALUE:
            return std::string("NOT_SET");
        default:
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
        }
    }

    //! assignment operator
    DRAType& operator=(const DRAType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const DRAType& o) const { return value == o.value; }
    bool operator!=(const DRAType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    DRAType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const DRAType& o) const { return value < o.value; }
    bool operator>(const DRAType& o) const { return value > o.value; }
    bool operator<=(const DRAType& o) const { return value <= o.value; }
    bool operator>=(const DRAType& o) const { return value >= o.value; }
    operator int() const { return value; }
    operator std::string() const { return toString(); }

    static size_t size() { return 4; }

    int value;

};


/*!
 *  \struct DownsamplingMethod 
 *
 *  Enumeration used to represent DownsamplingMethods
 */
struct DownsamplingMethod
{
    //! The enumerations allowed
    enum
    {
        DECIMATE = 0,
        MAX_PIXEL = 1,
        AVERAGE = 2,
        NOT_SET = six::NOT_SET_VALUE
    };

    //! Default constructor
    DownsamplingMethod(){ value = NOT_SET; }

    //! string constructor
    DownsamplingMethod(std::string s)
    {
        if (s == "DECIMATE")
            value = DECIMATE;
        else if (s == "MAX_PIXEL")
            value = MAX_PIXEL;
        else if (s == "AVERAGE")
            value = AVERAGE;
        else if (s == "NOT_SET")
            value = NOT_SET;
        else
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %s", s.c_str())));
    }

    //! int constructor
    DownsamplingMethod(int i)
    {
        switch(i)
        {
        case 0:
            value = DECIMATE;
            break;
        case 1:
            value = MAX_PIXEL;
            break;
        case 2:
            value = AVERAGE;
            break;
        case six::NOT_SET_VALUE:
            value = NOT_SET;
            break;
        default:
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", i)));
        }
    }

    //! destructor
    ~DownsamplingMethod(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return std::string("DECIMATE");
        case 1:
            return std::string("MAX_PIXEL");
        case 2:
            return std::string("AVERAGE");
        case six::NOT_SET_VALUE:
            return std::string("NOT_SET");
        default:
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
        }
    }

    //! assignment operator
    DownsamplingMethod& operator=(const DownsamplingMethod& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const DownsamplingMethod& o) const { return value == o.value; }
    bool operator!=(const DownsamplingMethod& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    DownsamplingMethod& operator=(const int& o) { value = o; return *this; }
    bool operator<(const DownsamplingMethod& o) const { return value < o.value; }
    bool operator>(const DownsamplingMethod& o) const { return value > o.value; }
    bool operator<=(const DownsamplingMethod& o) const { return value <= o.value; }
    bool operator>=(const DownsamplingMethod& o) const { return value >= o.value; }
    operator int() const { return value; }
    operator std::string() const { return toString(); }

    static size_t size() { return 4; }

    int value;

};


/*!
 *  \struct KernelDatabaseName 
 *
 *  Enumeration used to represent KernelDatabaseNames
 */
struct KernelDatabaseName
{
    //! The enumerations allowed
    enum
    {
        BILINEAR = 0,
        CUBIC = 1,
        LAGRANGE = 2,
        NEAREST_NEIGHBOR = 3,
        NOT_SET = six::NOT_SET_VALUE
    };

    //! Default constructor
    KernelDatabaseName(){ value = NOT_SET; }

    //! string constructor
    KernelDatabaseName(std::string s)
    {
        if (s == "BILINEAR")
            value = BILINEAR;
        else if (s == "CUBIC")
            value = CUBIC;
        else if (s == "LAGRANGE")
            value = LAGRANGE;
        else if (s == "NEAREST_NEIGHBOR")
            value = NEAREST_NEIGHBOR;
        else if (s == "NOT_SET")
            value = NOT_SET;
        else
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %s", s.c_str())));
    }

    //! int constructor
    KernelDatabaseName(int i)
    {
        switch(i)
        {
        case 0:
            value = BILINEAR;
            break;
        case 1:
            value = CUBIC;
            break;
        case 2:
            value = LAGRANGE;
            break;
        case 3:
            value = NEAREST_NEIGHBOR;
            break;
        case six::NOT_SET_VALUE:
            value = NOT_SET;
            break;
        default:
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", i)));
        }
    }

    //! destructor
    ~KernelDatabaseName(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return std::string("BILINEAR");
        case 1:
            return std::string("CUBIC");
        case 2:
            return std::string("LAGRANGE");
        case 3:
            return std::string("NEAREST_NEIGHBOR");
        case six::NOT_SET_VALUE:
            return std::string("NOT_SET");
        default:
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
        }
    }

    //! assignment operator
    KernelDatabaseName& operator=(const KernelDatabaseName& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const KernelDatabaseName& o) const { return value == o.value; }
    bool operator!=(const KernelDatabaseName& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    KernelDatabaseName& operator=(const int& o) { value = o; return *this; }
    bool operator<(const KernelDatabaseName& o) const { return value < o.value; }
    bool operator>(const KernelDatabaseName& o) const { return value > o.value; }
    bool operator<=(const KernelDatabaseName& o) const { return value <= o.value; }
    bool operator>=(const KernelDatabaseName& o) const { return value >= o.value; }
    operator int() const { return value; }
    operator std::string() const { return toString(); }

    static size_t size() { return 5; }

    int value;

};


/*!
 *  \struct KernelOperation 
 *
 *  Enumeration used to represent KernelOperations
 */
struct KernelOperation
{
    //! The enumerations allowed
    enum
    {
        CONVOLUTION = 0,
        CORRELATION = 1,
        NOT_SET = six::NOT_SET_VALUE
    };

    //! Default constructor
    KernelOperation(){ value = NOT_SET; }

    //! string constructor
    KernelOperation(std::string s)
    {
        if (s == "CONVOLUTION")
            value = CONVOLUTION;
        else if (s == "CORRELATION")
            value = CORRELATION;
        else if (s == "NOT_SET")
            value = NOT_SET;
        else
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %s", s.c_str())));
    }

    //! int constructor
    KernelOperation(int i)
    {
        switch(i)
        {
        case 0:
            value = CONVOLUTION;
            break;
        case 1:
            value = CORRELATION;
            break;
        case six::NOT_SET_VALUE:
            value = NOT_SET;
            break;
        default:
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", i)));
        }
    }

    //! destructor
    ~KernelOperation(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return std::string("CONVOLUTION");
        case 1:
            return std::string("CORRELATION");
        case six::NOT_SET_VALUE:
            return std::string("NOT_SET");
        default:
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
        }
    }

    //! assignment operator
    KernelOperation& operator=(const KernelOperation& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const KernelOperation& o) const { return value == o.value; }
    bool operator!=(const KernelOperation& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    KernelOperation& operator=(const int& o) { value = o; return *this; }
    bool operator<(const KernelOperation& o) const { return value < o.value; }
    bool operator>(const KernelOperation& o) const { return value > o.value; }
    bool operator<=(const KernelOperation& o) const { return value <= o.value; }
    bool operator>=(const KernelOperation& o) const { return value >= o.value; }
    operator int() const { return value; }
    operator std::string() const { return toString(); }

    static size_t size() { return 3; }

    int value;

};


/*!
 *  \struct RenderingIntent 
 *
 *  Enumeration used to represent RenderingIntents
 */
struct RenderingIntent
{
    //! The enumerations allowed
    enum
    {
        PERCEPTUAL = 0,
        SATURATION = 1,
        RELATIVE_INTENT = 2,
        ABSOLUTE_INTENT = 3,
        NOT_SET = six::NOT_SET_VALUE
    };

    //! Default constructor
    RenderingIntent(){ value = NOT_SET; }

    //! string constructor
    RenderingIntent(std::string s)
    {
        if (s == "PERCEPTUAL")
            value = PERCEPTUAL;
        else if (s == "SATURATION")
            value = SATURATION;
        else if (s == "RELATIVE_INTENT")
            value = RELATIVE_INTENT;
        else if (s == "ABSOLUTE_INTENT")
            value = ABSOLUTE_INTENT;
        else if (s == "NOT_SET")
            value = NOT_SET;
        else
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %s", s.c_str())));
    }

    //! int constructor
    RenderingIntent(int i)
    {
        switch(i)
        {
        case 0:
            value = PERCEPTUAL;
            break;
        case 1:
            value = SATURATION;
            break;
        case 2:
            value = RELATIVE_INTENT;
            break;
        case 3:
            value = ABSOLUTE_INTENT;
            break;
        case six::NOT_SET_VALUE:
            value = NOT_SET;
            break;
        default:
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", i)));
        }
    }

    //! destructor
    ~RenderingIntent(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return std::string("PERCEPTUAL");
        case 1:
            return std::string("SATURATION");
        case 2:
            return std::string("RELATIVE_INTENT");
        case 3:
            return std::string("ABSOLUTE_INTENT");
        case six::NOT_SET_VALUE:
            return std::string("NOT_SET");
        default:
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
        }
    }

    //! assignment operator
    RenderingIntent& operator=(const RenderingIntent& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const RenderingIntent& o) const { return value == o.value; }
    bool operator!=(const RenderingIntent& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    RenderingIntent& operator=(const int& o) { value = o; return *this; }
    bool operator<(const RenderingIntent& o) const { return value < o.value; }
    bool operator>(const RenderingIntent& o) const { return value > o.value; }
    bool operator<=(const RenderingIntent& o) const { return value <= o.value; }
    bool operator>=(const RenderingIntent& o) const { return value >= o.value; }
    operator int() const { return value; }
    operator std::string() const { return toString(); }

    static size_t size() { return 5; }

    int value;

};


/*!
 *  \struct ShadowsDirection
 *
 *  Enumeration used to represent ShadowsDirections
 */
struct ShadowsDirection
{
    //! The enumerations allowed
    enum
    {
        UP = 0,
        DOWN = 1,
        LEFT = 2,
        RIGHT = 3,
        ARBITRARY = 4,
        NOT_SET = six::NOT_SET_VALUE
    };

    //! Default constructor
    ShadowsDirection(){ value = NOT_SET; }

    //! string constructor
    ShadowsDirection(std::string s)
    {
        if (s == "UP")
            value = UP;
        else if (s == "DOWN")
            value = DOWN;
        else if (s == "LEFT")
            value = LEFT;
        else if (s == "RIGHT")
            value = RIGHT;
        else if (s == "ARBITRARY")
            value = ARBITRARY;
        else if (s == "NOT_SET")
            value = NOT_SET;
        else
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %s", s.c_str())));
    }

    //! int constructor
    ShadowsDirection(int i)
    {
        switch(i)
        {
        case 0:
            value = UP;
            break;
        case 1:
            value = DOWN;
            break;
        case 2:
            value = LEFT;
            break;
        case 3:
            value = RIGHT;
            break;
        case 4:
            value = ARBITRARY;
            break;
        case six::NOT_SET_VALUE:
            value = NOT_SET;
            break;
        default:
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", i)));
        }
    }

    //! destructor
    ~ShadowsDirection(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return std::string("UP");
        case 1:
            return std::string("DOWN");
        case 2:
            return std::string("LEFT");
        case 3:
            return std::string("RIGHT");
        case 4:
            return std::string("ARBITRARY");
        case six::NOT_SET_VALUE:
            return std::string("NOT_SET");
        default:
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
        }
    }

    //! assignment operator
    ShadowsDirection& operator=(const ShadowsDirection& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const ShadowsDirection& o) const { return value == o.value; }
    bool operator!=(const ShadowsDirection& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    ShadowsDirection& operator=(const int& o) { value = o; return *this; }
    bool operator<(const ShadowsDirection& o) const { return value < o.value; }
    bool operator>(const ShadowsDirection& o) const { return value > o.value; }
    bool operator<=(const ShadowsDirection& o) const { return value <= o.value; }
    bool operator>=(const ShadowsDirection& o) const { return value >= o.value; }
    operator int() const { return value; }
    operator std::string() const { return toString(); }

    static size_t size() { return 6; }

    int value;

};

// code auto-generated 2016-01-14 08:15:28.495616
}
}

#endif
