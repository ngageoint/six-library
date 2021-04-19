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
#ifndef __SCENE_FRAME_TYPE_H__
#define __SCENE_FRAME_TYPE_H__
#pragma once

#include <string>
#include <ostream>

namespace scene
{
// NOTE: These enums and strings must exactly match what is in the SIX
//       standard.  Since the scene module needs this concept as well though,
//       it's present here.
struct FrameType final
{
    //! The enumerations allowed
    enum FrameTypesEnum
    {
        ECF = 0,
        RIC_ECF = 1,
        RIC_ECI = 2,
        NOT_SET = 3
    };

    //! Value constructor
    FrameType() = default;
    FrameType(FrameTypesEnum value);

    //! String constructor
    FrameType(const std::string& str);

    //! Returns string representation of the value
    std::string toString() const;

    static
    FrameTypesEnum fromString(const std::string& str);

    bool operator==(FrameTypesEnum rhs) const
    {
        return (mValue == rhs);
    }

    bool operator!=(FrameTypesEnum rhs) const
    {
        return !(*this == rhs);
    }

    bool operator==(const FrameType& rhs) const
    {
        return (mValue == rhs.mValue);
    }

    bool operator!=(const FrameType& rhs) const
    {
        return !(*this == rhs);
    }

    bool operator==(const std::string& rhs) const
    {
        return toString() == rhs;
    }

    bool operator!=(const std::string& rhs) const
    {
        return !(*this == rhs);
    }

    FrameTypesEnum mValue = NOT_SET;

};

inline std::ostream& operator<<(std::ostream& os, const FrameType& e)
{
    os << e.toString();
    return os;
}
}
#endif
