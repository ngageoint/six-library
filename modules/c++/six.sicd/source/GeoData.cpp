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
#include "six/sicd/GeoData.h"

using namespace six;
using namespace six::sicd;


GeoInfo::~GeoInfo()
{
    for (unsigned int i = 0; i < geoInfos.size(); ++i)
    {
        GeoInfo* info = geoInfos[i];
        geoInfos.erase(geoInfos.begin());
        delete info;
    }

}
GeoInfo* GeoInfo::clone() const
{
    GeoInfo* g = new GeoInfo();
    g->desc = desc;
    g->geometryLatLon = geometryLatLon;

    for (unsigned int i = 0; i < geoInfos.size(); ++i)
    {
        GeoInfo* info = geoInfos[i];
        g->geoInfos.push_back(info->clone());
    }
    return g;
}
GeoData::~GeoData()
{
    for (unsigned int i = 0; i < geoInfos.size(); ++i)
    {
        GeoInfo* info = geoInfos[i];
        geoInfos.erase(geoInfos.begin());
        delete info;
    }
}

GeoData* GeoData::clone()
{
    GeoData* g = new GeoData();
    g->earthModel = earthModel;
    g->scp = scp;
    g->imageCorners = imageCorners;
    g->validData = validData;
    for (unsigned int i = 0; i < geoInfos.size(); ++i)
    {
        GeoInfo* info = geoInfos[i];
        g->geoInfos.push_back(info->clone());
    }
    return g;
}
