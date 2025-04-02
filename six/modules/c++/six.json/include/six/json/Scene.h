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
#ifndef SIX_JSON_SCENE_h_INCLUDED_
#define SIX_JSON_SCENE_h_INCLUDED_

#include <nlohmann/json.hpp>
#include <import/scene.h>

namespace scene
{
    template<typename BasicJsonType>
    void to_json(BasicJsonType& j, const LatLon& ll)
    {
        j["lat"] = ll.getLat();
        j["lon"] = ll.getLon();
    }
    template<typename BasicJsonType>
    void from_json(const BasicJsonType& j, LatLon& ll)
    {
        ll.setLat(j["lat"].template get<double>());
        ll.setLon(j["lon"].template get<double>());
    }

    template<typename BasicJsonType>
    void to_json(BasicJsonType& j, const LatLonAlt& lla)
    {
        j["lat"] = lla.getLat();
        j["lon"] = lla.getLon();
        j["alt"] = lla.getAlt();
    }
    template<typename BasicJsonType>
    void from_json(const BasicJsonType& j, LatLonAlt& lla)
    {
        lla.setLat(j["lat"].template get<double>());
        lla.setLon(j["lon"].template get<double>());
        lla.setAlt(j["alt"].template get<double>());
    }

    template<typename BasicJsonType>
    void to_json(BasicJsonType& j, const FrameType& frame)
    {
        j = frame.toString();
    }
    template<typename BasicJsonType>
    void from_json(const BasicJsonType& j, FrameType& frame)
    {
        frame = FrameType::fromString(j.template get<std::string>());
    }
} // namespace scene
#endif