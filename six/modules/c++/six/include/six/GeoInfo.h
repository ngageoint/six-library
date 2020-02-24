/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
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

#ifndef __SIX_GEO_INFO_H__
#define __SIX_GEO_INFO_H__

#include <string>
#include <vector>

#include <six/Types.h>
#include <six/ParameterCollection.h>
#include <mem/ScopedCopyablePtr.h>

namespace six
{
/*!
 *  \struct GeoInfo
 *  \brief (Optional) SICD/SIDD GeoInfo parameters
 *
 *  Parameters describing geographic features.
 *  Note: the GeoInfo block may be used as a block within itself
 *
 *  Compiler-generated copy constructor and assignment operator are sufficient
 */
class GeoInfo
{
public:
    //!  Constructor
    GeoInfo()
    {
    }

    /*!  Destructor (this may be required by some old compilers that otherwise
     *   can't handle a ScopedCopyablePtr of the same object being a member
     *   variable)
     */
    ~GeoInfo()
    {
    }

    //!  Name of a geographic feature
    std::string name;

    //! (Optional) sub-nodes
    std::vector<mem::ScopedCopyablePtr<GeoInfo> > geoInfos;

    //! (Optional) description of geographic feature
    ParameterCollection desc;

    /*!
     *  This could be a point if one, a line if 2, or a polygon if > 2
     *  In other words, this simultaneously represents SICD's
     *  GeoInfo/Point, GeoInfo/Line, and GeoInfo/Polygon
     */
    std::vector<LatLon> geometryLatLon;

    bool operator==(const GeoInfo& rhs) const
    {
        return (name == rhs.name && geoInfos == rhs.geoInfos &&
            desc == rhs.desc && geometryLatLon == rhs.geometryLatLon);
    }
    bool operator!=(const GeoInfo& rhs) const
    {
        return !(*this == rhs);
    }
};

//! Ostream operator
std::ostream& operator<< (std::ostream& os, const GeoInfo& g);
}

#endif
