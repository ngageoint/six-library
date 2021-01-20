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

#include <sys/Conf.h>
#include <except/Exception.h>
#include <str/Convert.h>

#include <six/Enum.h>

namespace cphd
{
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
struct PhaseSGN final : public six::details::Enum<PhaseSGN>
{
    //! The enumerations allowed
    enum
    {
        MINUS_1 = -1,
        PLUS_1 = 1,
        NOT_SET = six::NOT_SET_VALUE
    };

    static const map_t& string_to_int_()
    {
        static const map_t retval
        {
            {"-1", MINUS_1},
            {"+1", PLUS_1},
            SIX_Enum_map_entry_NOT_SET
        };
        return retval;
    }

    PhaseSGN() = default;
    PhaseSGN(const std::string& s) : Enum(s) {}
    PhaseSGN(int i) : Enum(i) {}
    PhaseSGN& operator=(const int& o) { value = o; return *this; }
};


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
struct PolarizationType final : public six::details::Enum<PolarizationType>
{
    //! The enumerations allowed
    enum
    {
        X = 1,
        Y = 2,
        V = 3,
        H = 4,
        RHC = 5,
        LHC = 6,
        UNSPECIFIED = 7,
        NOT_SET = six::NOT_SET_VALUE
    };

    static const map_t& string_to_int_()
    {
        static const map_t retval
        {
            SIX_Enum_map_entry_(X),
            SIX_Enum_map_entry_(Y),
            SIX_Enum_map_entry_(V),
            SIX_Enum_map_entry_(H),
            SIX_Enum_map_entry_(RHC),
            SIX_Enum_map_entry_(LHC),
            SIX_Enum_map_entry_(UNSPECIFIED),
            SIX_Enum_map_entry_NOT_SET
        };
        return retval;
    }

    PolarizationType() = default;
    PolarizationType(const std::string & s) : Enum(s) {}
    PolarizationType(int i) : Enum(i) {}
    PolarizationType& operator=(const int& o) { value = o; return *this; }
};
}
#endif
