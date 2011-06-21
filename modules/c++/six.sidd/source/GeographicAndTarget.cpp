/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * six-c++ is free software; you can redistribute it and/or modify
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

using namespace six;
using namespace six::sidd;

TargetInformation* TargetInformation::clone() const
{
    return new TargetInformation(*this);
}

GeographicInformation* GeographicInformation::clone() const
{
    return new GeographicInformation(*this);
}

GeographicCoverage::GeographicCoverage(RegionType rt) :
    regionType(rt)
{
    //! TODO: add one to all one or more types
    if (regionType == RegionType::GEOGRAPHIC_INFO)
        geographicInformation.reset(new GeographicInformation());
}

GeographicCoverage::~GeographicCoverage()
{
    for (size_t ii = 0; ii < subRegion.size(); ++ii)
    {
        delete subRegion[ii];
    }
}

GeographicCoverage* GeographicCoverage::clone() const
{
    std::auto_ptr<GeographicCoverage> gc(new GeographicCoverage(regionType));
    gc->georegionIdentifiers = georegionIdentifiers;
    gc->footprint = footprint;

    gc->subRegion.resize(subRegion.size(), NULL);
    for (size_t ii = 0; ii < subRegion.size(); ++ii)
    {
        gc->subRegion[ii] = subRegion[ii]->clone();
    }

    if (geographicInformation.get())
    {
        gc->geographicInformation.reset(geographicInformation->clone());
    }
    else
    {
        gc->geographicInformation.reset();
    }

    GeographicCoverage* const gcPtr(gc.release());
    return gcPtr;
}

GeographicAndTarget::~GeographicAndTarget()
{
    for (size_t ii = 0; ii < targetInformation.size(); ++ii)
    {
        delete targetInformation[ii];
    }
}

GeographicAndTarget* GeographicAndTarget::clone()
{
    std::auto_ptr<GeographicAndTarget>
        g(new GeographicAndTarget(RegionType::NOT_SET));

    if (geographicCoverage.get())
    {
        g->geographicCoverage.reset(geographicCoverage->clone());
    }

    g->targetInformation.resize(targetInformation.size(), NULL);
    for (size_t ii = 0; ii < targetInformation.size(); ++ii)
    {
        g->targetInformation[ii] = targetInformation[ii]->clone();
    }

    GeographicAndTarget* const gPtr(g.release());
    return gPtr;
}
