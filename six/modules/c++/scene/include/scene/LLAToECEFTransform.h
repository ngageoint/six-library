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
#ifndef __SCENE_LLA_TO_ECEF_TRANSFORM_H__
#define __SCENE_LLA_TO_ECEF_TRANSFORM_H__

#include <sstream>

#include "scene/CoordinateTransform.h"
#include "scene/Exports.h"

namespace scene
{

struct SIX_SCENE_API LLAToECEFTransform : public CoordinateTransform
{
    /**
     * This constructor just calls the base class constructor
     */
    LLAToECEFTransform() = default;

    /**
     * This constructor just calls the base class constructor
     */
    LLAToECEFTransform(const EllipsoidModel *initVals);

    ~LLAToECEFTransform() = default;

    /**
     * This function returns a pointer to a clone of the LLAToECEFTransform
     * object.
     */
    LLAToECEFTransform* clone() const;

    /**
     * This function transforms an LatLonAlt to an Vector3.
     *
     * @param lla   The lla coordinate to transform
     * @return      A Vector3
     */
    Vector3 transform(const LatLonAlt& lla) const;
private:

    double computeRadius(const LatLonAlt& lla) const;
    double computeLatitude(const double lat) const;


};

}
#endif

