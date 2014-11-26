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
#include "scene/Types.h"

template<> 
std::string str::toString(const scene::SideOfTrack& track)
{
    if (track == scene::TRACK_RIGHT)
        return "RIGHT";
    return "LEFT";
}

template<> 
scene::SideOfTrack str::toType<scene::SideOfTrack>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "LEFT")
        return scene::TRACK_LEFT;
    return scene::TRACK_RIGHT;
}

