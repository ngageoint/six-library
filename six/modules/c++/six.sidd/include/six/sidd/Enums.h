/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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
#pragma once

#include "six/Enum.h"

namespace six
{
namespace sidd
{

/*!
 *  \struct ARPFlag
 *
 *  Enumeration used to represent ARPFlags
 */
struct ARPFlag final : public six::details::Enum<ARPFlag>
{
    //! The enumerations allowed
    enum
    {
        REALTIME = 0,
        PREDICTED = 1,
        POST_PROCESSED = 2,
        NOT_SET = six::NOT_SET_VALUE
    };

    static const map_t& string_to_int_()
    {
        static const map_t retval
        {
            SIX_Enum_map_entry_(REALTIME),
            SIX_Enum_map_entry_(PREDICTED),
            {"POST PROCESSED", POST_PROCESSED },
            SIX_Enum_map_entry_NOT_SET
        };
        return retval;
    }

    ARPFlag() = default;
    ARPFlag(const std::string& s) : Enum(s) {}
    ARPFlag(int i) : Enum(i) {}
    ARPFlag& operator=(const int& o) { value = o; return *this; }
};

/*!
*  \struct BandEqualizationAlgorithm
*
*  Enumeration used to represent BandEqualizationAlgorithms
*/
struct BandEqualizationAlgorithm final : public six::details::Enum<BandEqualizationAlgorithm>
{
    //! The enumerations allowed
    enum
    {
        LUT_1D = 0,
        NOT_SET = six::NOT_SET_VALUE
    };

    static const map_t& string_to_int_()
    {
        static const map_t retval
        {
            {"LUT 1D", LUT_1D },
            SIX_Enum_map_entry_NOT_SET
        };
        return retval;
    }

    BandEqualizationAlgorithm() = default;
    BandEqualizationAlgorithm(const std::string& s) : Enum(s) {}
    BandEqualizationAlgorithm(int i) : Enum(i) {}
    BandEqualizationAlgorithm& operator=(const int& o) { value = o; return *this; }
};


/*!
*  \struct CoordinateSystemType
*
*  Enumeration used to represent CoordinateSystemTypes
*/
SIX_Enum_ENUM_2(CoordinateSystemType,
    GCS, 0,
    UTM, 1
);

/*!
*  \struct DRAType
*
*  Enumeration used to represent DRATypes
*/
SIX_Enum_ENUM_3(DRAType,
    AUTO, 0,
    MANUAL, 1,
    NONE, 2
);

/*!
*  \struct DownsamplingMethod
*
*  Enumeration used to represent DownsamplingMethods
*/
struct DownsamplingMethod final : public six::details::Enum<DownsamplingMethod>
{
    //! The enumerations allowed
    enum
    {
        DECIMATE = 0,
        MAX_PIXEL = 1,
        AVERAGE = 2,
        NEAREST_NEIGHBOR = 3,
        BILINEAR = 4,
        LAGRANGE = 5,
        NOT_SET = six::NOT_SET_VALUE
    };

    static const map_t& string_to_int_()
    {
        static const map_t retval
        {
            SIX_Enum_map_entry_(DECIMATE),
            { "MAX PIXEL", MAX_PIXEL },
            SIX_Enum_map_entry_(AVERAGE),
            { "NEAREST NEIGHBOR", NEAREST_NEIGHBOR },
            SIX_Enum_map_entry_(BILINEAR),
            SIX_Enum_map_entry_(LAGRANGE),
            SIX_Enum_map_entry_NOT_SET
        };
        return retval;
    }

    DownsamplingMethod() = default;
    DownsamplingMethod(const std::string & s) : Enum(s) {}
    DownsamplingMethod(int i) : Enum(i) {}
    DownsamplingMethod& operator=(const int& o) { value = o; return *this; }
};


/*!
*  \struct FilterDatabaseName
*
*  Enumeration used to represent FilterDatabaseNames
*/
struct FilterDatabaseName final : public six::details::Enum<FilterDatabaseName>
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

    static const map_t& string_to_int_()
    {
        static const map_t retval
        {
            SIX_Enum_map_entry_(BILINEAR),
            SIX_Enum_map_entry_(CUBIC),
            SIX_Enum_map_entry_(LAGRANGE),
            { "NEAREST NEIGHBOR", NEAREST_NEIGHBOR },
            SIX_Enum_map_entry_NOT_SET
        };
        return retval;
    }

    FilterDatabaseName() = default;
    FilterDatabaseName(const std::string& s) : Enum(s) {}
    FilterDatabaseName(int i) : Enum(i) {}
    FilterDatabaseName& operator=(const int& o) { value = o; return *this; }
};

/*!
*  \struct FilterOperation
*
*  Enumeration used to represent FilterOperations
*/
SIX_Enum_ENUM_2(FilterOperation,
    CONVOLUTION, 0,
    CORRELATION, 1
);

/*!
*  \struct RenderingIntent
*
*  Enumeration used to represent RenderingIntents
*/
struct RenderingIntent final : public six::details::Enum<RenderingIntent>
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

    static const map_t& string_to_int_()
    {
        static const map_t retval
        {
            SIX_Enum_map_entry_(PERCEPTUAL),
            SIX_Enum_map_entry_(SATURATION),
            { "RELATIVE INTENT", RELATIVE_INTENT },
            { "ABSOLUTE INTENT", ABSOLUTE_INTENT },
            SIX_Enum_map_entry_NOT_SET
        };
        return retval;
    }

    RenderingIntent() = default;
    RenderingIntent(const std::string& s) : Enum(s) {}
    RenderingIntent(int i) : Enum(i) {}
    RenderingIntent& operator=(const int& o) { value = o; return *this; }
};

/*!
*  \struct ShadowDirection
*
*  Enumeration used to represent ShadowDirections
*/
SIX_Enum_ENUM_5(ShadowDirection,
    UP, 0,
    DOWN, 1,
    LEFT, 2,
    RIGHT, 3,
    ARBITRARY, 4
);

}
}

