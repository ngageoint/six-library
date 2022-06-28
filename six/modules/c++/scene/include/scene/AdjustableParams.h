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
#ifndef __SCENE_ADJUSTABLE_PARAMS_H__
#define __SCENE_ADJUSTABLE_PARAMS_H__

#include <string>

#include <scene/Types.h>

namespace scene
{
// These should be in RIC_ECF
struct AdjustableParams
{
    enum ParamsEnum
    {
        ARP_RADIAL = 0,
        ARP_IN_TRACK = 1,
        ARP_CROSS_TRACK = 2,
        ARP_VEL_RADIAL = 3,
        ARP_VEL_IN_TRACK = 4,
        ARP_VEL_CROSS_TRACK = 5,
        RANGE_BIAS = 6,
        NUM_PARAMS = 7,
        NUM_CORR_GROUPS = 2
    };

    AdjustableParams() = default;

    static std::string name(ParamsEnum param);

    static std::string units(ParamsEnum param);

    static int group(ParamsEnum param);

    /// The returned Vector3 is a reference to values in the object's mParams.
    Vector3 getARPVector() const
    {
        return Vector3(&mParams[ARP_RADIAL]);
    }

    /// The returned Vector3 is a reference to values in the object's mParams.
    Vector3 getARPVelocityVector() const
    {
        return Vector3(&mParams[ARP_VEL_RADIAL]);
    }

    double operator[](std::ptrdiff_t idx) const
    {
        return mParams[idx];
    }

    double mParams[NUM_PARAMS]{ 0.0 };
};
}

#endif
