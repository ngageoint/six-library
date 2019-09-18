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
MatchInfo::MatchInfo()
{
}

MatchType::MatchType():
    typeID(six::Init::undefined<std::string>()),
    index(six::Init::undefined<size_t>()),
    currentIndex(six::Init::undefined<size_t>())
{
}

MatchCollection::MatchCollection():
    coreName(six::Init::undefined<std::string>()),
    index(six::Init::undefined<size_t>()),
    matchIndex(six::Init::undefined<size_t>())
{
}

std::ostream& operator<< (std::ostream& os, const MatchCollection& m)
{
    os << "MatchCollections::\n"
        << "  coreName             : " << m.coreName << "\n";
    if(!six::Init::isUndefined(m.matchIndex))
    {
        os << "  matchIndex        : " << m.matchIndex << "\n";
    }
    for (size_t ii = 0; ii < m.parameter.size(); ++ii)
    {
        os << "  parameter name    :" << m.parameter[ii].getName() << "\n";
        os << "  parameter value   :" << m.parameter[ii].str() << "\n";
    }
    return os;
}

std::ostream& operator<< (std::ostream& os, const MatchType& m)
{
    os << "MatchType::\n"
        << "  typeID      : " << m.typeID << "\n";
    if(!six::Init::isUndefined(m.currentIndex))
    {
        os << "  currentIndex    : " << m.currentIndex << "\n";
    }
    os << "  numMatchCollection    : " << m.matchCollection.size() << "\n";

    for (size_t ii = 0; ii < m.matchCollection.size(); ++ii)
    {
        os << "  " << m.matchCollection[ii] << "\n";
    }
    return os;
}

std::ostream& operator<< (std::ostream& os, const MatchInfo& m)
{
    os << "MatchInfo::\n"
       << "  numMatchTypes       : " << m.matchType.size() << "\n";
    for (size_t ii = 0; ii < m.matchType.size(); ++ii)
    {
        os << "  " << m.matchType[ii] << "\n";
    }
    return os;
}
}
