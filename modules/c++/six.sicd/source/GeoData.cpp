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
#include <memory>

#include "six/sicd/GeoData.h"

using namespace six;
using namespace six::sicd;


GeoInfo::~GeoInfo()
{
    for (size_t ii = 0; ii < geoInfos.size(); ++ii)
    {
        delete geoInfos[ii];
    }
}

GeoInfo* GeoInfo::clone() const
{
    std::auto_ptr<GeoInfo> dolly(new GeoInfo());
    dolly->desc = desc;
    dolly->geometryLatLon = geometryLatLon;

    for (size_t ii = 0; ii < geoInfos.size(); ++ii)
    {
        std::auto_ptr<GeoInfo> info(geoInfos[ii]->clone());
        dolly->geoInfos.push_back(info.get());
        info.release();
    }

    GeoInfo * const dollyPtr(dolly.release());
    return dollyPtr;
}

GeoData::~GeoData()
{
    for (size_t ii = 0; ii < geoInfos.size(); ++ii)
    {
        delete geoInfos[ii];
    }
}

GeoData* GeoData::clone()
{
    std::auto_ptr<GeoData> dolly(new GeoData());
    dolly->earthModel = earthModel;
    dolly->scp = scp;
    dolly->imageCorners = imageCorners;
    dolly->validData = validData;
    dolly->validDataIdx = validDataIdx;
    for (size_t ii = 0; ii < geoInfos.size(); ++ii)
    {
        std::auto_ptr<GeoInfo> info(geoInfos[ii]->clone());
        dolly->geoInfos.push_back(info.get());
        info.release();
    }

    GeoData * const dollyPtr(dolly.release());
    return dollyPtr;
}
