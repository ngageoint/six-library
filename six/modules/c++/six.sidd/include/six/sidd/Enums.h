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
    SIX_Enum_BEGIN_enum(ARPFlag)
        REALTIME = 0,
        PREDICTED = 1,
        POST_PROCESSED = 2,
    SIX_Enum_END_enum
SIX_Enum_END_DEFINE(ARPFlag);
#undef SIX_Enum_map_entry_name
#define SIX_Enum_map_entry_name ARPFlag
SIX_Enum_BEGIN_string_to_value(ARPFlag)
        SIX_Enum_map_entry(REALTIME),
        SIX_Enum_map_entry(PREDICTED),
        {"POST PROCESSED", ARPFlag::POST_PROCESSED },
SIX_Enum_END_string_to_value

/*!
*  \struct BandEqualizationAlgorithm
*
*  Enumeration used to represent BandEqualizationAlgorithms
*/
SIX_Enum_BEGIN_DEFINE(BandEqualizationAlgorithm)
    //! The enumerations allowed
    SIX_Enum_BEGIN_enum(BandEqualizationAlgorithm)
        LUT_1D = 0,
    SIX_Enum_END_enum
SIX_Enum_END_DEFINE(BandEqualizationAlgorithm);
SIX_Enum_BEGIN_string_to_value(BandEqualizationAlgorithm)
        {"LUT 1D", BandEqualizationAlgorithm::LUT_1D },
SIX_Enum_END_string_to_value

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
    SIX_Enum_BEGIN_enum(DownsamplingMethod)
        DECIMATE = 0,
        MAX_PIXEL = 1,
        AVERAGE = 2,
        NEAREST_NEIGHBOR = 3,
        BILINEAR = 4,
        LAGRANGE = 5,
    SIX_Enum_END_enum
SIX_Enum_END_DEFINE(DownsamplingMethod);
#undef SIX_Enum_map_entry_name
#define SIX_Enum_map_entry_name DownsamplingMethod
SIX_Enum_BEGIN_string_to_value(DownsamplingMethod)
        SIX_Enum_map_entry(DECIMATE),
        { "MAX PIXEL", DownsamplingMethod::MAX_PIXEL },
        SIX_Enum_map_entry(AVERAGE),
        { "NEAREST NEIGHBOR", DownsamplingMethod::NEAREST_NEIGHBOR },
        SIX_Enum_map_entry(BILINEAR),
        SIX_Enum_map_entry(LAGRANGE),
SIX_Enum_END_string_to_value

/*!
*  \struct FilterDatabaseName
*
*  Enumeration used to represent FilterDatabaseNames
*/
SIX_Enum_BEGIN_DEFINE(FilterDatabaseName)
    //! The enumerations allowed
    SIX_Enum_BEGIN_enum(FilterDatabaseName)
        BILINEAR = 0,
        CUBIC = 1,
        LAGRANGE = 2,
        NEAREST_NEIGHBOR = 3,
    SIX_Enum_END_enum
SIX_Enum_END_DEFINE(FilterDatabaseName);
#undef SIX_Enum_map_entry_name
#define SIX_Enum_map_entry_name FilterDatabaseName
SIX_Enum_BEGIN_string_to_value(FilterDatabaseName)
        SIX_Enum_map_entry(BILINEAR),
        SIX_Enum_map_entry(CUBIC),
        SIX_Enum_map_entry(LAGRANGE),
        { "NEAREST NEIGHBOR", FilterDatabaseName::NEAREST_NEIGHBOR },
SIX_Enum_END_string_to_value

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
    SIX_Enum_BEGIN_enum(RenderingIntent)
        PERCEPTUAL = 0,
        SATURATION = 1,
        RELATIVE_INTENT = 2,
        ABSOLUTE_INTENT = 3,
    SIX_Enum_END_enum
SIX_Enum_END_DEFINE(RenderingIntent);
#undef SIX_Enum_map_entry_name
#define SIX_Enum_map_entry_name RenderingIntent
SIX_Enum_BEGIN_string_to_value(RenderingIntent)
        SIX_Enum_map_entry(PERCEPTUAL),
        SIX_Enum_map_entry(SATURATION),
        { "RELATIVE INTENT", RenderingIntent::RELATIVE_INTENT },
        { "ABSOLUTE INTENT", RenderingIntent::ABSOLUTE_INTENT },
SIX_Enum_END_string_to_value


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

