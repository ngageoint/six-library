/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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

#include <cphd/MatchInfo.h>
#include <six/Init.h>

namespace cphd
{
MatchInfo::MatchInfo():
    numMatchType(six::Init::undefined<size_t>())
{
}

MatchType::MatchType():
    typeId(six::Init::undefined<std::string>()),
    currentIndex(six::Init::undefined<size_t>()),
    numMatchCollections(six::Init::undefined<size_t>())
{
}

MatchCollections::MatchCollections():
    coreName(six::Init::undefined<std::string>()),
    matchIndex(six::Init::undefined<size_t>())
{
}

std::ostream& operator<< (std::ostream& os, const MatchCollections& m)
{
    os << "MatchCollections::\n"
        << "  coreName      : " << m.coreName << "\n";
    if(!six::Init::isUndefined(m.matchIndex))
    {
        os << "  matchIndex    : " << m.matchIndex << "\n";
    }
    for (size_t i = 0; i < m.parameter.size(); ++i)
    {
        os << "  parameter     :" << m.parameter[i] << "\n";
    }
    return os;
}

std::ostream& operator<< (std::ostream& os, const MatchType& m)
{
    os << "MatchType::\n"
        << "  typeId      : " << m.typeId << "\n";
    if(!six::Init::isUndefined(m.currentIndex))
    {
        os << "  currentIndex    : " << m.currentIndex << "\n";
    }
    os << "  numMatchCollections    : " << m.numMatchCollections << "\n";

    for (size_t i = 0; i < m.matchCollections.size(); ++i)
    {
        os << "  matchCollections     :" << m.matchCollections[i] << "\n";
    }
    return os;
}


std::ostream& operator<< (std::ostream& os, const MatchInfo& m)
{
    os << "MatchInfo::\n"
       << "  numMatchType       : " << m.numMatchType << "\n";
    for (size_t i = 0; i < m.matchType.size(); ++i)
    {
        os << "  matchType     :" << m.matchType[i] << "\n";
    }
    return os;
}


}
