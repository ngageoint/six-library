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

#include <cphd/GeoInfo.h>
#include <six/Init.h>

namespace cphd
{

std::ostream& operator<< (std::ostream& os, const GeoInfo& g)
{
    os << "GeoInfo::\n";
    for (size_t i = 0; i < g.desc.size(); ++i)
    {
        os << "  desc     : " << g.desc[i] << "\n";
    }
    for (size_t i = 0; i < g.point.size(); ++i)
    {
        os << "  point::\n"
            << "    lat    : " << g.point[i].getLat() << "\n"
            << "    lon    : " << g.point[i].getLon() << "\n";
    }
    for (size_t i = 0; i < g.line.size(); ++i)
    {
        os << "  line::\n"
            << "    lat    : " << g.line[i].getLat() << "\n"
            << "    lon    : " << g.line[i].getLon() << "\n";
    }
    for (size_t i = 0; i < g.polygon.size(); ++i)
    {
        os << "  polygon::\n"
            << "    lat    : " << g.polygon[i].getLat() << "\n"
            << "    lon    : " << g.polygon[i].getLon() << "\n";
    }
    for (size_t i = 0; i < g.geoInfo.size(); ++i)
    {
        os << "  geoInfo  : " << g.geoInfo[i] << "\n";
    }
    return os;

}


}
