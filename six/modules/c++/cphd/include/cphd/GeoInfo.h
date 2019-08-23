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


#ifndef __CPHD_GEO_INFO_H__
#define __CPHD_GEO_INFO_H__

#include <ostream>
#include <vector>
#include <stddef.h>

#include <cphd/Enums.h>
#include <cphd/Types.h>


namespace cphd
{

struct LatLonIndexType : LatLon
{
public:

    bool operator==(const LatLonIndexType& other) const
    {
        return index == other.index && mLat == other.mLat &&
                mLon == other.mLon;
    }

    bool operator!=(const LatLonIndexType& other) const
    {
        return !((*this) == other);
    }

    size_t index;
};

struct LineType
{
    LineType();

    bool operator==(const LineType& other) const
    {
        return numEndpoints == other.numEndpoints && 
                endpoint == other.endpoint;
    }

    bool operator!=(const LineType& other) const
    {
        return !((*this) == other);
    }


    size_t numEndpoints;
    std::vector<LatLonIndexType> endpoint;

};

struct PolygonType
{
    PolygonType();

    bool operator==(const PolygonType& other) const
    {
        return numVertices == other.numVertices && 
                vertex == other.vertex;
    }

    bool operator!=(const PolygonType& other) const
    {
        return !((*this) == other);
    }


    size_t numVertices;
    std::vector<LatLonIndexType> vertex;
};


struct GeoInfo
{
    GeoInfo();

    GeoInfo(std::string);

    bool operator==(const GeoInfo& other) const
    {
        return desc == other.desc && point == other.point &&
                line == other.line && polygon == other.polygon &&
                geoInfo == other.geoInfo;
    }

    bool operator!=(const GeoInfo& other) const
    {
        return !((*this) == other);
    }

    inline std::string getName() const
    {
        return mName;
    }

    inline void setName(const std::string name)
    {
        mName = name;
    }

    six::ParameterCollection desc;
    std::vector<LatLon> point;
    std::vector<LineType> line;
    std::vector<PolygonType> polygon;
    std::vector<GeoInfo> geoInfo;

private:
    std::string mName;
};

std::ostream& operator<< (std::ostream& os, const GeoInfo& g);
}

#endif
