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

LineType::LineType()
{
}

PolygonType::PolygonType()
{
}

GeoInfo::GeoInfo() :
    mName(six::Init::undefined<std::string>())
{
}

GeoInfo::GeoInfo(std::string name) :
    mName(name)
{
}

std::ostream& operator<< (std::ostream& os, const GeoInfo& g)
{
    os << "GeoInfo::\n";
    if(!six::Init::isUndefined(g.getName()))
    {
        os << "  name         : " << g.getName() << "\n";
    }
    for (size_t ii = 0; ii < g.desc.size(); ++ii)
    {
        os << "  desc name    : " << g.desc[ii].getName() << "\n";
        os << "  desc value   : " << g.desc[ii].str() << "\n";
    }
    for (size_t ii = 0; ii < g.point.size(); ++ii)
    {
        os << "  point::\n"
            << "    lat    : " << g.point[ii].getLat() << "\n"
            << "    lon    : " << g.point[ii].getLon() << "\n";
    }
    for (size_t ii = 0; ii < g.line.size(); ++ii)
    {
        os << "  line::\n"
            << "  Size    : " << g.line[ii].endpoint.size() << "\n";
            for (size_t jj = 0; jj < g.line[ii].endpoint.size(); ++jj)
            {
                os << "    Endpoint index   :" << g.line[ii].endpoint[jj].index << "\n"
                    << "      lat    : " << g.line[ii].endpoint[jj].getLat() << "\n"
                    << "      lon    : " << g.line[ii].endpoint[jj].getLon() << "\n";
            }
    }
    for (size_t ii = 0; ii < g.polygon.size(); ++ii)
    {
        os << "  polygon::\n"
            << "  Size    : " << g.polygon[ii].vertex.size() << "\n";
            for (size_t jj = 0; jj < g.polygon[ii].vertex.size(); ++jj)
            {
                os << "    Vertex index   :" << g.polygon[ii].vertex[jj].index << "\n"
                    << "      lat    : " << g.polygon[ii].vertex[jj].getLat() << "\n"
                    << "      lon    : " << g.polygon[ii].vertex[jj].getLon() << "\n";
            }
    }
    for (size_t ii = 0; ii < g.geoInfo.size(); ++ii)
    {
        os << "  geoInfo  : " << g.geoInfo[ii] << "\n";
    }
    return os;
}
}


