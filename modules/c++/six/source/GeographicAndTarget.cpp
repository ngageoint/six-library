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
#include "six/GeographicAndTarget.h"

using namespace six;

TargetInformation* TargetInformation::clone() const
{
    return new TargetInformation(*this);
}

GeographicCoverage::~GeographicCoverage()
{
    if (geographicInformation)
        delete geographicInformation;

    for (unsigned int i = 0; i < subRegion.size(); ++i)
    {
        GeographicCoverage* sr = subRegion[i];
        delete sr;
    }
}

GeographicInformation* GeographicInformation::clone() const
{
    return new GeographicInformation(*this);
}

GeographicCoverage::GeographicCoverage(RegionType rt) :
    regionType(rt), geographicInformation(NULL)
{
    //! TODO: add one to all one or more types
    if (regionType == REGION_GEOGRAPHIC_INFO)
        geographicInformation = new GeographicInformation();
}

GeographicCoverage* GeographicCoverage::clone() const
{
    GeographicCoverage* gc = new GeographicCoverage(regionType);
    gc->georegionIdentifiers = georegionIdentifiers;
    gc->footprint = footprint;
    for (unsigned int i = 0; i < subRegion.size(); ++i)
    {
        GeographicCoverage* sr = subRegion[i];
        gc->subRegion.push_back(sr->clone());
    }

    if (gc->geographicInformation)
    {
        delete gc->geographicInformation;
        gc->geographicInformation = NULL;
    }

    if (geographicInformation)
        gc->geographicInformation = geographicInformation->clone();
    return gc;
}

GeographicAndTarget::~GeographicAndTarget()
{
    if (geographicCoverage)
        delete geographicCoverage;
    for (unsigned int i = 0; i < targetInformation.size(); ++i)
    {
        delete targetInformation[i];
    }
}

GeographicAndTarget* GeographicAndTarget::clone()
{
    GeographicAndTarget* g = new GeographicAndTarget(REGION_NOT_SET);
    if (geographicCoverage)
        g->geographicCoverage = geographicCoverage->clone();

    for (unsigned int i = 0; i < targetInformation.size(); ++i)
    {
        TargetInformation* t = targetInformation[i];
        g->targetInformation.push_back(t->clone());
    }
    return g;
}
