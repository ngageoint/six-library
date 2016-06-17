/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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
#include "six/sidd/GeographicAndTarget.h"

namespace six
{
namespace sidd
{
GeographicCoverage::GeographicCoverage(RegionType rt) :
    regionType(rt)
{
    //! TODO: add one to all one or more types
    if (regionType == RegionType::GEOGRAPHIC_INFO)
        geographicInformation.reset(new GeographicInformation());
}

bool GeographicCoverage::operator==(const GeographicCoverage& rhs) const
{
    return (regionType == rhs.regionType &&
        georegionIdentifiers == rhs.georegionIdentifiers &&
        footprint == rhs.footprint &&
        subRegion == rhs.subRegion &&
        geographicInformation == rhs.geographicInformation);
}

bool GeographicAndTarget::operator==(const GeographicAndTarget& rhs) const
{
    return (geographicCoverage == rhs.geographicCoverage &&
        targetInformation == rhs.targetInformation && 
        earthModel == rhs.earthModel &&
        imageCorners == rhs.imageCorners &&
        validData == rhs.validData &&
        geoInfos == rhs.geoInfos);
}

}
}
