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
#ifndef __SCENE_FRAME_TYPE_H__
#define __SCENE_FRAME_TYPE_H__

#include <string>
#include <ostream>

#include "scene/Exports.h"

namespace scene
{
// NOTE: These enums and strings must exactly match what is in the SIX
//       standard.  Since the scene module needs this concept as well though,
//       it's present here.
struct SIX_SCENE_API FrameType final
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

    bool operator==(const FrameType& rhs) const
    {
        return value() == rhs.value();
    }
    bool operator!=(const FrameType& rhs) const
    {
        return !(*this == rhs);
    }

    FrameTypesEnum value() const { return mValue; }
    FrameTypesEnum mValue = NOT_SET;
};

inline bool operator==(const FrameType& lhs, FrameType::FrameTypesEnum rhs)
{
    return (lhs.value() == rhs);
}
inline bool operator!=(const FrameType& lhs, FrameType::FrameTypesEnum rhs)
{
    return !(lhs == rhs);
}

inline bool operator==(const FrameType& lhs, const std::string& rhs)
{
    return lhs.toString() == rhs;
}
inline bool operator!=(const FrameType& lhs, const std::string& rhs)
{
    return !(lhs == rhs);
}

inline std::ostream& operator<<(std::ostream& os, const FrameType& e)
{
    os << e.toString();
    return os;
}
}
#endif
