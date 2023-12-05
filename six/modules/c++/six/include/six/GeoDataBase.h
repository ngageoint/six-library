/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
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
#pragma once
#ifndef __SIX_GEO_DATA_BASE_H__
#define __SIX_GEO_DATA_BASE_H__

#include "six/Types.h"
#include "six/Init.h"
#include "six/GeoInfo.h"
#include "six/Parameter.h"
#include "six/ParameterCollection.h"
#include "six/Exports.h"
#include "scene/ProjectionModel.h"
#include <mem/ScopedCloneablePtr.h>
#include <logging/Logger.h>

namespace six
{
/*!
 *  \struct GeoDataBase
 *  \brief General GeoDataBase block
 *
 *  This block describes the geographic coordinates of the region
 *  covered by the image
 *
 *  Compiler-generated copy constructor and assignment operator are sufficient
 */
struct SIX_SIX_API GeoDataBase
{
    GeoDataBase() = default;

    /*!
     *  Identifies the earth model used for
     *  latitude, longitude and height parameters.  All
     *  height values are Height Above Ellipsoid (HAE)
     */
    EarthModelType earthModel = EarthModelType::WGS84; // force WGS84, since spec does.

    /*!
     *  Parameters apply to image corners of the
     *  product projected to the same height as the SCP.
     *  These corners are an approximate geographic location
     *  not intended for analytical use
     */
    LatLonCorners imageCorners;

    /*!
     *  (Optional) indicates the full image includes both
     *  valid data and some zero-filled pixels.  Vector size
     *  is the number of vertices.
     */
    std::vector<LatLon> validData;

    /*!
     *  (Optional) Parameters that describe geographic features.
     *  Note that this may be used as a block inside of a block.
     */
    std::vector<mem::ScopedCopyablePtr<GeoInfo> > geoInfos;

    bool operator==(const GeoDataBase& rhs) const;
    bool operator!=(const GeoDataBase& rhs) const
    {
        return !(*this == rhs);
    }
};
}

#endif
