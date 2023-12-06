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
#ifndef __SCENE_ECEF_TO_LLA_TRANSFORM_H__
#define __SCENE_ECEF_TO_LLA_TRANSFORM_H__

#include "scene/CoordinateTransform.h"
#include "scene/Exports.h"

namespace scene
{

struct SIX_SCENE_API ECEFToLLATransform : public CoordinateTransform
{
    /**
     * This constructor just calls the base class constructor
     */
    ECEFToLLATransform() = default;

    /**
     * This constructor just calls the base class constructor
     */
    ECEFToLLATransform(const EllipsoidModel *initVals);

    virtual ~ECEFToLLATransform(){}

    /**
     * This function returns a pointer to a clone of the ECEFToLLATransform 
     * object.
     */
    ECEFToLLATransform* clone() const;

    /**
     * This function transforms an Vector3 to an LatLonAlt.
     *
     * @param ecef  The ecef coordinate to transform
     * @return      A LatLonAlt
     */
    LatLonAlt transform(const Vector3& ecef) const;

private:
    static double computeLongitude(const Vector3& ecef);
    double computeAltitude(const Vector3& ecef, double latitude) const;
    double getInitialLatitude(const Vector3& ecef) const;
    double computeReducedLatitude(double latitude) const;
    double computeLatitude(const Vector3& ecef, double reducedLatitude) const;
};

}
#endif

