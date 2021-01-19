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
#include <scene/FrameType.h>

#include <nitf/coda-oss.hpp>
#include <except/Exception.h>

namespace scene
{
FrameType::FrameType(FrameTypesEnum value) :
    mValue(value)
{
}

FrameType::FrameType(const std::string& str) :
    mValue(fromString(str))
{
}

FrameType::FrameTypesEnum FrameType::fromString(const std::string& str)
{
    if (str == "ECF")
    {
        return ECF;
    }
    else if (str == "RIC_ECF")
    {
        return RIC_ECF;
    }
    else if (str == "RIC_ECI")
    {
        return RIC_ECI;
    }
    else if (str == "NOT_SET")
    {
        return NOT_SET;
    }
    else
    {
        throw except::InvalidFormatException(Ctxt(
                "Invalid enum value: " + str));
    }
}

std::string FrameType::toString() const
{
    switch (mValue)
    {
    case ECF:
        return "ECF";
    case RIC_ECF:
        return "RIC_ECF";
    case RIC_ECI:
        return "RIC_ECI";
    case NOT_SET:
        return "NOT_SET";
    default:
        throw except::InvalidFormatException(Ctxt("Invalid enum value"));
    }
}
}
