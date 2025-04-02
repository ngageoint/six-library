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
#ifndef SIX_JSON_ENUMS_h_INCLUDED_
#define SIX_JSON_ENUMS_h_INCLUDED_

#include <nlohmann/json.hpp>
#include <six/Enums.h>

namespace six
{
    template<typename BasicJsonType>
    void to_json(BasicJsonType& j, const CollectType& collectType)
    {
        j = collectType.toString();
    }
    template<typename BasicJsonType>
    void from_json(const BasicJsonType& j, CollectType& collectType)
    {
        collectType = CollectType::toType(j.template get<std::string>());
    }

    template<typename BasicJsonType>
    void to_json(BasicJsonType& j, const RadarModeType& radarMode)
    {
        j = radarMode.toString();
    }
    template<typename BasicJsonType>
    void from_json(const BasicJsonType& j, RadarModeType& radarMode)
    {
        radarMode = RadarModeType::toType(j.template get<std::string>());
    }

    template<typename BasicJsonType>
    void to_json(BasicJsonType& j, const BooleanType& boolType)
    {
        j = boolType.toString();
    }
    template<typename BasicJsonType>
    void from_json(const BasicJsonType& j, BooleanType& boolType)
    {
        boolType = BooleanType::toType(j.template get<std::string>());
    }

    template<typename BasicJsonType>
    void to_json(BasicJsonType& j, const DualPolarizationType& pol)
    {
        j = pol.toString();
    }
    template<typename BasicJsonType>
    void from_json(const BasicJsonType& j, DualPolarizationType& pol)
    {
        pol = DualPolarizationType::toType(j.template get<std::string>());
    }
} // namespace six
#endif