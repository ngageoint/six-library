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
#ifndef SIX_JSON_SIX_h_INCLUDED_
#define SIX_JSON_SIX_h_INCLUDED_

#include <nlohmann/json.hpp>
#include <import/six.h>


namespace nlohmann
{
    template <typename T>
    struct adl_serializer<six::XsElement<T>>
    {
        template<typename BasicJsonType>
        static void from_json(const BasicJsonType& j, six::XsElement<T>& val)
        {
            const auto& item = j.cbegin();
            val.name().setName(item.key());
            val.value() = j[item.key()].template get<T>();
        }
        template<typename BasicJsonType>
        static void to_json(BasicJsonType& j, const six::XsElement<T>& val)
        {
            j[val.name().getName()] = val.value();
        }
    };
}

namespace six
{
    template<typename BasicJsonType>
    void to_json(BasicJsonType& j, const EarthModelType& earthModel)
    {
        j = earthModel.toString();;
    }
    template<typename BasicJsonType>
    void from_json(const BasicJsonType& j, EarthModelType& earthModel)
    {
        earthModel = EarthModelType::toType(j.template get<std::string>());
    }

    template<typename BasicJsonType>
    void to_json(BasicJsonType& j, const Parameter& p)
    {
        j[p.getName()] = p.str();
    }
    template<typename BasicJsonType>
    void from_json(const BasicJsonType& j, Parameter& p)
    {
        const auto& item = j.cbegin();
        p.setName(item.key());
        p.setValue(j[item.key()].template get<std::string>());
    }

    template<typename BasicJsonType>
    void to_json(BasicJsonType& j, const ParameterCollection& pc)
    {
        std::vector<Parameter> params(pc.size());
        for (size_t ii = 0; ii < pc.size(); ii++)
        {
            params[ii] = pc[ii];
        }
        j = params;
    }
    template<typename BasicJsonType>
    void from_json(const BasicJsonType& j, ParameterCollection& pc)
    {
        auto params = j.template get<std::vector<Parameter>>();
        for (const auto& param: params)
        {
            pc.push_back(param);
        }
    }

    template<typename BasicJsonType, typename T>
    void to_json(BasicJsonType& j, const Corners<T>& corners)
    {
        j["frfc"] = corners.upperLeft;
        j["frlc"] = corners.upperRight;
        j["lrlc"] = corners.lowerRight;
        j["lrfc"] = corners.lowerLeft;
    }
    template<typename BasicJsonType, typename T>
    void from_json(const BasicJsonType& j, Corners<T>& corners)
    {
        corners.upperLeft = j["frfc"].template get<T>();
        corners.upperRight = j["frlc"].template get<T>();
        corners.lowerRight = j["lrlc"].template get<T>();
        corners.lowerLeft = j["lrfc"].template get<T>();
    }
    
    template<typename BasicJsonType>
    void to_json(BasicJsonType& j, const six::SCPType& scp)
    {
        j = scp.toString();
    }
    template<typename BasicJsonType>
    void from_json(const BasicJsonType& j, six::SCPType& scp)
    {
        scp = six::SCPType::toType(j.template get<std::string>());
    }

    NLOHMANN_JSON_SERIALIZE_ENUM(CompositeSCP::SCPType, {
        {CompositeSCP::SCPType::RG_AZ, "RG_AZ"},
        {CompositeSCP::SCPType::ROW_COL, "ROW_COL"}
    })

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CollectionInformation, collectorName, illuminatorName, coreName, collectType, radarMode, radarModeID, releaseInfo, countryCodes, parameters)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SCP, ecf, llh)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GeoInfo, name, geoInfos, desc, geometryLatLon)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ReferencePoint, ecef, rowCol, name)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PosVelError, p1, p2, p3, v1, v2, v3, corrCoefs, positionDecorr)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RadarSensor, rangeBias, clockFreqSF, transmitFreqSF, rangeBiasDecorr)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TropoError, tropoRangeVertical, tropoRangeSlant, tropoRangeDecorr)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(IonoError, ionoRangeVertical, ionoRangeRateVertical, ionoRgRgRateCC, ionoRangeVertDecorr)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DecorrType, corrCoefZero, decorrRate)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(CorrCoefs, p1p2, p1p3, p1v1, p1v2, p1v3, p2p3, p2v1, p2v2, p2v3, p3v1, p3v2, p3v3, v1v2, v1v3, v2v3)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Unmodeled::Decorr::Xrow_Ycol, corrCoefZero, decorrRate)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Unmodeled::Decorr, Xrow, Ycol)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Unmodeled, Xrow, Ycol, XrowYcol, unmodeledDecorr)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CompositeSCP, scpType, xErr, yErr, xyErr)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Components, posVelError, radarSensor, tropoError, ionoError)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ErrorStatistics, compositeSCP, components, unmodeled, additionalParameters)
} // namespace six
#endif