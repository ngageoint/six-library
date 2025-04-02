/* =========================================================================
 * This file is part of six.json-c++
 * =========================================================================
 *
 * (C) Copyright 2025 ARKA Group, L.P. All rights reserved
 *
 * six.json-c++ is free software; you can redistribute it and/or modify
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
 */

#pragma once
#ifndef SIX_JSON_SIX_SICD_h_INCLUDED_
#define SIX_JSON_SIX_SICD_h_INCLUDED_

#include <nlohmann/json.hpp>
#include <import/six/sicd.h>

namespace nitf
{
    template<typename BasicJsonType>
    void to_json(BasicJsonType& j, const DateTime& dt)
    {
        j = dt.format("%Y-%m-%dT%H:%M:%SZ");
    }
    template<typename BasicJsonType>
    void from_json(const BasicJsonType& j, DateTime& dt)
    {
        dt = DateTime(j.template get<std::string>(), "%Y-%m-%dT%H:%M:%SZ");
    }
}

namespace six
{
namespace sicd
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GeoData, earthModel, scp, imageCorners, validData, geoInfos)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TimelineSet, tStart, tEnd, interPulsePeriodStart, interPulsePeriodEnd, interPulsePeriodPoly)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InterPulsePeriod, sets)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Timeline, collectStart, collectDuration, interPulsePeriod)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AreaDirectionParameters, unitVector, spacing, elements, first)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ElectricalBoresight, dcxPoly, dcyPoly, useEBPVP)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HalfPowerBeamwidths, dcx, dcy)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GainAndPhasePolys, gainPoly, phasePoly, antGPId)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AntennaParameters, xAxisPoly, yAxisPoly,
        frequencyZero, electricalBoresight, halfPowerBeamwidths, array, element,
        gainBSPoly, electricalBoresightFrequencyShift, mainlobeFrequencyDilation)
} // namespace sicd
} // namespace six
#endif
