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
/*
 * Extension of LatLon Type that stores an index attribute
 */
struct LatLonIndexType : LatLon
{
    //! Equality operators
    bool operator==(const LatLonIndexType& other) const
    {
        return index == other.index && mLat == other.mLat &&
                mLon == other.mLon;
    }
    bool operator!=(const LatLonIndexType& other) const
    {
        return !((*this) == other);
    }

    //! Index of Lat Lon type
    size_t index;
};

/*
 * Used to specify a linear feature with connected line segments
 * Requires NumEndpoints >= 2
 */
struct LineType
{
    //! Default constructor
    LineType();

    //! Equality operator
    bool operator==(const LineType& other) const
    {
        return endpoint == other.endpoint;
    }
    bool operator!=(const LineType& other) const
    {
        return !((*this) == other);
    }

    //! Line segment enpoints speciried in Lat and Lon
    std::vector<LatLonIndexType> endpoint;

};

/*
 * Used to specify an area with a polygon
 * Requires NumVertices >= 3
 */
struct PolygonType
{
    //! Default constructor
    PolygonType();

    //! Equality operators
    bool operator==(const PolygonType& other) const
    {
        return vertex == other.vertex;
    }
    bool operator!=(const PolygonType& other) const
    {
        return !((*this) == other);
    }

    //! Polygon vertices specified in Lat and Lon.
    std::vector<LatLonIndexType> vertex;
};

/*
 * (Optional) Parameter block that describes a geographic feature.
 * GeoInfo branch may be repeated.
 * See section 5.2.2
 */
struct GeoInfo
{
    //! Default constructor
    GeoInfo();

    //! Custom constructor
    GeoInfo(std::string name);

    //! Equality operators
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

    //! Get name attribute for the geo info block
    inline std::string getName() const
    {
        return mName;
    }

    //! Set name attribute for the geo info block
    inline void setName(const std::string name)
    {
        mName = name;
    }

    //! (Optional) Used to provide a name and/or description of the
    //! geographic feature.
    six::ParameterCollection desc;

    //! (Optional) Used to specific a single point in Lat and Lon.
    //! -90.0 < Lat < 90.0, -180.0 < Lon < 180.0
    std::vector<LatLon> point;

    //! (Optional) Used to specify a “linear” feature with connected
    //! line segments.
    std::vector<LineType> line;

    //! (Optional) Used to specify an area with a polygon
    std::vector<PolygonType> polygon;

    //! (Optional)
    std::vector<GeoInfo> geoInfo;

private:
    //! Geographic feature name attribute
    std::string mName;
};

//! Ostream operators
std::ostream& operator<< (std::ostream& os, const GeoInfo& g);
}

#endif
