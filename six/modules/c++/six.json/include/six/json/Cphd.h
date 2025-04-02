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
#ifndef SIX_JSON_CPHD03_h_INCLUDED_
#define SIX_JSON_CPHD03_h_INCLUDED_

#include <nlohmann/json.hpp>
#include <import/cphd03.h>
#include <import/cphd.h>

namespace cphd03
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DwellTimeParameters, codTimePoly, dwellTimePoly)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AreaPlane, referencePoint, xDirection, yDirection, dwellTime)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ImageArea, acpCorners, plane)
}

namespace cphd
{
    template<typename BasicJsonType>
    void to_json(BasicJsonType& j, const PhaseSGN& phase)
    {
        j = phase.toString();
    }
    template<typename BasicJsonType>
    void from_json(const BasicJsonType& j, PhaseSGN& phase)
    {
        phase = PhaseSGN::toType(j.template get<std::string>());  
    }
}


#endif