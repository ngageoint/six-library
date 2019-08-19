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


#ifndef __CPHD_GEOINFO_H__
#define __CPHD_GEOINFO_H__

#include <ostream>
#include <vector>
#include <stddef.h>

#include <cphd/Enums.h>
#include <cphd/Types.h>


namespace cphd
{
struct GeoInfo
{
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

    std::vector<std::string> desc;
    std::vector<LatLon> point;
    std::vector<LatLon> line;
    std::vector<LatLon> polygon;
    std::vector<GeoInfo> geoInfo;
};

std::ostream& operator<< (std::ostream& os, const GeoInfo& g);
}

#endif
