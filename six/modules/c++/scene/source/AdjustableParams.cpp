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
#include <scene/AdjustableParams.h>

#include <algorithm>

#include <scene/sys_Conf.h>
#include <except/Exception.h>

namespace scene
{
AdjustableParams::AdjustableParams()
{
    std::fill_n(mParams, static_cast<size_t>(NUM_PARAMS), 0.0);
}

std::string AdjustableParams::name(ParamsEnum param)
{
    switch (param)
    {
    case ARP_RADIAL:
        return "ARP Radial";
    case ARP_IN_TRACK:
        return "ARP In-Track";
    case ARP_CROSS_TRACK:
        return "ARP Cross-Track";
    case ARP_VEL_RADIAL:
        return "Velocity Radial";
    case ARP_VEL_IN_TRACK:
        return "Velocity In-Track";
    case ARP_VEL_CROSS_TRACK:
        return "Velocity Cross-Track";
    case RANGE_BIAS:
        return "Range Bias";
    case NUM_PARAMS:
    default:
        throw except::InvalidFormatException(Ctxt("Invalid enum value"));
    }
}

std::string AdjustableParams::units(ParamsEnum param)
{
    switch (param)
    {
    case ARP_RADIAL:
    case ARP_IN_TRACK:
    case ARP_CROSS_TRACK:
    case RANGE_BIAS:
        return "m";
    case ARP_VEL_RADIAL:
    case ARP_VEL_IN_TRACK:
    case ARP_VEL_CROSS_TRACK:
        return "m/s";
    case NUM_PARAMS:
    default:
        throw except::InvalidFormatException(Ctxt("Invalid enum value"));
    }
}

int AdjustableParams::group(ParamsEnum param)
{
    switch (param)
    {
    case ARP_RADIAL:
    case ARP_IN_TRACK:
    case ARP_CROSS_TRACK:
    case ARP_VEL_RADIAL:
    case ARP_VEL_IN_TRACK:
    case ARP_VEL_CROSS_TRACK:
        return 0;
    case RANGE_BIAS:
        return 1;
    case NUM_PARAMS:
    default:
        return -1;
    }
}
}
