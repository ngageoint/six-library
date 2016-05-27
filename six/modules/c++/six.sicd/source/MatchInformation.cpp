/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
#include "six/sicd/MatchInformation.h"

namespace six
{
namespace sicd
{

MatchCollect::MatchCollect() :
    coreName(Init::undefined<std::string>()),
    matchIndex(Init::undefined<int>())
{
}

MatchType::MatchType() :
    collectorName(Init::undefined<std::string>()),
    illuminatorName(Init::undefined<std::string>()),
    typeID(Init::undefined<std::string>()),
    currentIndex(Init::undefined<int>()),
    matchCollects(1)
{
}

bool MatchType::operator==(const MatchType& rhs) const
{
    return (collectorName == rhs.collectorName &&
        illuminatorName == rhs.illuminatorName &&
        matchType == rhs.matchType &&
        typeID == rhs.typeID &&
        currentIndex == rhs.currentIndex &&
        matchCollects == rhs.matchCollects);
}

//! One is maditory
MatchInformation::MatchInformation() :
    types(1)
{
    types[0].reset(new MatchType());
}

}
}

