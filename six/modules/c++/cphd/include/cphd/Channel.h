/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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

#ifndef __CPHD_CHANNEL_H__
#define __CPHD_CHANNEL_H__

#include <string>
#include <vector>

namespace cphd
{
struct ChannelParameter
{
    bool operator==(const ChannelParameter& other) const
    {
        return identifier == other.identifier &&
               refVectorIndex == other.refVectorIndex &&
               fxFixed == other.fxFixed &&
               toaFixed == other.toaFixed &&
               srpFixed == other.srpFixed &&
               signalNormal == other.signalNormal;
    }

    bool operator!=(const ChannelParameter& other) const
    {
        return !((*this) == other);
    }

    std::string identifier;
    size_t refVectorIndex;
    six::BooleanType fxFixed;
    six::BooleanType toaFixed;
    six::BooleanType srpFixed;
    six::BooleanType signalNormal;
};

struct Channel
{
    bool operator==(const Channel& other) const
    {
        return refChId == other.refChId &&
               fxFixedCphd == other.fxFixedCphd &&
               toaFixedCphd == other.toaFixedCphd &&
               srpFixedCphd == other.srpFixedCphd &&
               parameters == other.parameters;
    }

    bool operator!=(const Channel& other) const
    {
        return !((*this) == other);
    }

    std::string refChId;
    six::BooleanType fxFixedCphd;
    six::BooleanType toaFixedCphd;
    six::BooleanType srpFixedCphd;
    std::vector<ChannelParameter> parameters;
};
}

#endif
