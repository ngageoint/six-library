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
SIX_Enum_BEGIN_DEFINE(ARPFlag)
    //! The enumerations allowed
    SIX_Enum_BEGIN_enum
        REALTIME = 0,
        PREDICTED = 1,
        POST_PROCESSED = 2,
    SIX_Enum_END_enum

    SIX_Enum_BEGIN_string_to_value
            SIX_Enum_map_entry_(REALTIME),
            SIX_Enum_map_entry_(PREDICTED),
            {"POST PROCESSED", POST_PROCESSED },
    SIX_Enum_END_string_to_value
SIX_Enum_END_DEFINE(ARPFlag);

/*!
*  \struct BandEqualizationAlgorithm
*
*  Enumeration used to represent BandEqualizationAlgorithms
*/
SIX_Enum_BEGIN_DEFINE(BandEqualizationAlgorithm)
    //! The enumerations allowed
    SIX_Enum_BEGIN_enum
        LUT_1D = 0,
    SIX_Enum_END_enum

    SIX_Enum_BEGIN_string_to_value
            {"LUT 1D", LUT_1D },
    SIX_Enum_END_string_to_value
SIX_Enum_END_DEFINE(BandEqualizationAlgorithm);

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
SIX_Enum_BEGIN_DEFINE(DownsamplingMethod)
    //! The enumerations allowed
    SIX_Enum_BEGIN_enum
        DECIMATE = 0,
        MAX_PIXEL = 1,
        AVERAGE = 2,
        NEAREST_NEIGHBOR = 3,
        BILINEAR = 4,
        LAGRANGE = 5,
    SIX_Enum_END_enum

    SIX_Enum_BEGIN_string_to_value
            SIX_Enum_map_entry_(DECIMATE),
            { "MAX PIXEL", MAX_PIXEL },
            SIX_Enum_map_entry_(AVERAGE),
            { "NEAREST NEIGHBOR", NEAREST_NEIGHBOR },
            SIX_Enum_map_entry_(BILINEAR),
            SIX_Enum_map_entry_(LAGRANGE),
    SIX_Enum_END_string_to_value
SIX_Enum_END_DEFINE(DownsamplingMethod);


/*!
*  \struct FilterDatabaseName
*
*  Enumeration used to represent FilterDatabaseNames
*/
SIX_Enum_BEGIN_DEFINE(FilterDatabaseName)
    //! The enumerations allowed
    SIX_Enum_BEGIN_enum
        BILINEAR = 0,
        CUBIC = 1,
        LAGRANGE = 2,
        NEAREST_NEIGHBOR = 3,
    SIX_Enum_END_enum

    SIX_Enum_BEGIN_string_to_value
            SIX_Enum_map_entry_(BILINEAR),
            SIX_Enum_map_entry_(CUBIC),
            SIX_Enum_map_entry_(LAGRANGE),
            { "NEAREST NEIGHBOR", NEAREST_NEIGHBOR },
    SIX_Enum_END_string_to_value
SIX_Enum_END_DEFINE(FilterDatabaseName);

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
SIX_Enum_BEGIN_DEFINE(RenderingIntent)
    //! The enumerations allowed
    SIX_Enum_BEGIN_enum
        PERCEPTUAL = 0,
        SATURATION = 1,
        RELATIVE_INTENT = 2,
        ABSOLUTE_INTENT = 3,
    SIX_Enum_END_enum

    SIX_Enum_BEGIN_string_to_value
            SIX_Enum_map_entry_(PERCEPTUAL),
            SIX_Enum_map_entry_(SATURATION),
            { "RELATIVE INTENT", RELATIVE_INTENT },
            { "ABSOLUTE INTENT", ABSOLUTE_INTENT },
    SIX_Enum_END_string_to_value
SIX_Enum_END_DEFINE(RenderingIntent);

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

