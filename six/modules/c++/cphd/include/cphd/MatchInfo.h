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


#ifndef __CPHD_MATCH_INFO_H__
#define __CPHD_MATCH_INFO_H__

#include <ostream>
#include <vector>
#include <stddef.h>

#include <cphd/Enums.h>
#include <cphd/Types.h>


namespace cphd
{

struct MatchCollections
{

    MatchCollections();

    bool operator==(const MatchCollections& other) const
    {
        return coreName == other.coreName && matchIndex == other.matchIndex &&
                parameter == other.parameter;
    }

    bool operator!=(const MatchCollections& other) const
    {
        return !((*this) == other);
    }

    std::string coreName;
    size_t matchIndex;
    std::vector <std::string> parameter;
};

struct MatchType
{

    MatchType();

    bool operator==(const MatchType& other) const
    {
        return typeId == other.typeId && currentIndex == other.currentIndex &&
                numMatchCollections == other.numMatchCollections &&
                matchCollections == other.matchCollections;
    }

    bool operator!=(const MatchType& other) const
    {
        return !((*this) == other);
    }

    std::string typeId;
    size_t currentIndex;
    size_t numMatchCollections;
    std::vector<MatchCollections> matchCollections;
};



struct MatchInfo
{

    MatchInfo();

    bool operator==(const MatchInfo& other) const
    {
        return numMatchType == other.numMatchType &&
                matchType == other.matchType;
    }

    bool operator!=(const MatchInfo& other) const
    {
        return !((*this) == other);
    }

    size_t numMatchType;
    std::vector<MatchType> matchType;
};

std::ostream& operator<< (std::ostream& os, const MatchCollections& m);

std::ostream& operator<< (std::ostream& os, const MatchType& m);

std::ostream& operator<< (std::ostream& os, const MatchInfo& m);
}

#endif
