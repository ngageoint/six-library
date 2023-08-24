/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __CPHD_ENUMS_H__
#define __CPHD_ENUMS_H__
#pragma once

#include <scene/sys_Conf.h>
#include <except/Exception.h>
#include <str/Convert.h>

#include <six/Enum.h>

namespace cphd
{
    constexpr auto NOT_SET_VALUE = six::NOT_SET_VALUE;

/*!
 *  \struct SampleType
 *
 *  Enumeration used to represent SampleTypes
 */
SIX_Enum_ENUM_3(SampleType,
    RE32F_IM32F, 1,
    RE16I_IM16I, 2,
    RE08I_IM08I, 3
);

/*!
 *  \struct DomainType
 *
 *  Enumeration used to represent DomainTypes
 */
SIX_Enum_ENUM_3(DomainType,
    FX, 1,
    TOA, 2,
    RE08I_IM08I, 3
);

/*!
 *  \struct PhaseSGN
 *
 *  Enumeration used to represent PhaseSGNs
 */
SIX_Enum_BEGIN_DEFINE(PhaseSGN)
    //! The enumerations allowed
    SIX_Enum_BEGIN_enum
        MINUS_1 = -1,
        PLUS_1 = 1,
    SIX_Enum_END_enum

    SIX_Enum_BEGIN_string_to_value
    { "-1", MINUS_1 },
    { "+1", PLUS_1 },
    { "1", PLUS_1 },
    SIX_Enum_END_string_to_value
SIX_Enum_END_DEFINE(PhaseSGN);

/*!
 *  \struct RefHeight
 *
 *  Enumeration used to represent RefHeights
 */
SIX_Enum_ENUM_2(RefHeight,
    IARP, 1,
    ZERO, 2
);

/*!
 *  \struct EarthModelType
 *
 *  Enumeration used to represent EarthModelType
 *  Distict from SICD's EarthModelType
 *  (underscore vs. no underscore)
 */
SIX_Enum_ENUM_1(EarthModelType,
    WGS_84, 1
);

/*!
 * \struct SignalArrayFormat
 *
 * Enumeration used to represent signal array format
 */
SIX_Enum_ENUM_3(SignalArrayFormat,
    CI2, 1,
    CI4, 2,
    CF8, 3
);

/*!
 *  \struct SRPType
 *
 *  Enumeration used to represent SRPTypes
 */
SIX_Enum_ENUM_4(SRPType,
    FIXEDPT, 1,
    PVTPOLY, 2,
    PVVPOLY, 3,
    STEPPED, 4
);

/*!
 *  \struct PolarizationType
 *
 *  Enumeration used to represent PolarizationTypes
 */
SIX_Enum_BEGIN_DEFINE(PolarizationType)
    //! The enumerations allowed
    SIX_Enum_BEGIN_enum
        X = 1,
        Y = 2,
        V = 3,
        H = 4,
        RHC = 5,
        LHC = 6,
        UNSPECIFIED = 7,
        // Making these new values larger than RHC, LHC, and UNSPECIFIED in case
        // somebody is using the integeral values (5, 6, and 6).
        S = 8, // CPHD 1.1.0
        E = 9, // CPHD 1.1.0
    SIX_Enum_END_enum

    SIX_Enum_BEGIN_string_to_value
        SIX_Enum_map_entry_(X),
        SIX_Enum_map_entry_(Y),
        SIX_Enum_map_entry_(V),
        SIX_Enum_map_entry_(H),
        SIX_Enum_map_entry_(RHC),
        SIX_Enum_map_entry_(LHC),
        SIX_Enum_map_entry_(UNSPECIFIED),
        SIX_Enum_map_entry_(S),
        SIX_Enum_map_entry_(E),
    SIX_Enum_END_string_to_value
SIX_Enum_END_DEFINE(PolarizationType);
}
#endif
