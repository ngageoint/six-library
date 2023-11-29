/* =========================================================================
 * This file is part of scene-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * scene-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SCENE_UTILITIES_H__
#define __SCENE_UTILITIES_H__

#include "scene/Types.h"
#include "scene/Exports.h"

namespace scene
{
    struct GeographicGridECEFTransform;

struct SIX_SCENE_API Utilities final
{
    /*!
     *  Convert a lat/lon to ECEF. This method returns a
     *  three-dimensional vector, which can be indexed
     *  starting at zero.
     *
     *  \param latLon The input lat lon
     *  \return A vector representing the ECEF coordinate
     *
     */
    static Vector3 latLonToECEF(LatLonAlt latLon);
    static Vector3 latLonToECEF(LatLon latLon);

    /*!
     *  Convert a vector representing an ECEF coordinate
     *  into a lat/lon on the ellipsoid.
     *
     *  \param vec The ECEF X, Y and Z coordinate
     *  \return A lat/lon for the ECEF value.
     *
     */
    static LatLonAlt ecefToLatLon(Vector3 vec);
    static LatLonAlt ecefToLatLon(const GeographicGridECEFTransform&, size_t row, size_t col);

    /*!
     *  Remaps angles into [0:360]
     *
     *  Others ranges are explicitly unsupported because
     *  of remapping issues, and that angles outside those
     *  ranges generally indicate errors. For instance a
     *  Latitude value outside of the [-90:90] range would
     *  be considered an error.
     *
     *  Unsupported ranges are:
     *      [0:90]
     *      [-90:90]
     *      [-180:180]
     */
    static double remapZeroTo360(double degree);
};

}

#endif

