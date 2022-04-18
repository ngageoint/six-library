/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#include <six/MatchInformation.h>

namespace six
{
MatchCollect::MatchCollect() :
    matchIndex(Init::undefined<int>())
{
}

MatchType::MatchType() :
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
    types(1) //! One is mandatory
{
}

std::ostream& operator<< (std::ostream& os, const MatchInformation& m)
{
    os << "MatchInformation:: \n"
        << "  Types:: \n";
    for (size_t ii = 0; ii < m.types.size(); ++ii)
    {
        os << "  TypeID           : " << m.types[ii].typeID << "\n";
        if (!six::Init::isUndefined(m.types[ii].currentIndex))
        {
            os << "  CurrentIndex     : " << m.types[ii].currentIndex << "\n";
        }
        os << " NumMatchCollections : " << m.types[ii].matchCollects.size() << "\n";

        for (size_t jj = 0; jj < m.types[ii].matchCollects.size(); ++jj)
        {
            os << "  MatchCollect     : \n"
                << "    CoreName       : " << m.types[ii].matchCollects[jj].coreName << "\n";
            if (!six::Init::isUndefined(m.types[ii].matchCollects[jj].matchIndex))
            {
                os << "    MatchIndex     : " << m.types[ii].matchCollects[jj].matchIndex << "\n";
            }
            for (size_t kk = 0; kk < m.types[ii].matchCollects[jj].parameters.size(); ++kk)
            {
                os << "    Parameter      : "
                    << m.types[ii].matchCollects[jj].parameters[kk].getName()
                    << ": " << m.types[ii].matchCollects[jj].parameters[kk].str() << "\n";
            }
        }
    }
    return os;
}
}
