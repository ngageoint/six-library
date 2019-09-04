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

LineType::LineType() :
    numEndpoints(six::Init::undefined<size_t>())
{
}

PolygonType::PolygonType() :
    numVertices(six::Init::undefined<size_t>())
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
    for (size_t i = 0; i < g.desc.size(); ++i)
    {
        os << "  desc name    : " << g.desc[i].getName() << "\n";
        os << "  desc value   : " << g.desc[i].str() << "\n";
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
            << "  Size    : " << g.line[i].numEndpoints << "\n";
            for (size_t j = 0; j < g.line[i].endpoint.size(); ++j)
            {
                os << "    Endpoint index   :" << g.line[i].endpoint[j].index << "\n"
                    << "      lat    : " << g.line[i].endpoint[j].getLat() << "\n"
                    << "      lon    : " << g.line[i].endpoint[j].getLon() << "\n";
            }
    }
    for (size_t i = 0; i < g.polygon.size(); ++i)
    {
        os << "  polygon::\n"
            << "  Size    : " << g.polygon[i].numVertices << "\n";
            for (size_t j = 0; j < g.polygon[i].vertex.size(); ++j)
            {
                os << "    Vertex index   :" << g.polygon[i].vertex[j].index << "\n"
                    << "      lat    : " << g.polygon[i].vertex[j].getLat() << "\n"
                    << "      lon    : " << g.polygon[i].vertex[j].getLon() << "\n";
            }
    }
    for (size_t i = 0; i < g.geoInfo.size(); ++i)
    {
        os << "  geoInfo  : " << g.geoInfo[i] << "\n";
    }
    return os;
}
}


