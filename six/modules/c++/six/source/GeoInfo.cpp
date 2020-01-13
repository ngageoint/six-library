/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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

#include <six/GeoInfo.h>

namespace six
{
std::ostream& operator<< (std::ostream& os, const GeoInfo& g)
{
    os << "GeoInfo:: \n"
        << "  Name             : " << g.name << "\n";
    for (size_t ii = 0; ii < g.desc.size(); ++ii)
    {
        os << "  Description      : " << g.desc[ii].getName() << ": " << g.desc[ii].str() << "\n";
    }
    if (g.geometryLatLon.size() == 1)
    {
        os << "  Point            : " << g.geometryLatLon[0].getLat() << ", " << g.geometryLatLon[0].getLon() << "\n";
    }
    else if (g.geometryLatLon.size() == 2)
    {
        os << "  Line             : " << "\n";
        for (size_t ii = 0; ii < g.geometryLatLon.size(); ++ii)
        {
            os << "  Endpoint         : " << g.geometryLatLon[ii].getLat() << ", " << g.geometryLatLon[ii].getLon() << "\n";
        }
    }
    else if (g.geometryLatLon.size() > 2)
    {
        os << "  Polygon          : " << "\n";
        for (size_t ii = 0; ii < g.geometryLatLon.size(); ++ii)
        {
            os << "  Vertex           : " << g.geometryLatLon[ii].getLat() << ", " << g.geometryLatLon[ii].getLon() << "\n";
        }
    }

    for (size_t ii = 0; ii < g.geoInfos.size(); ++ii)
    {
        os << *g.geoInfos[ii] << "\n";
    }
    return os;
}
}
