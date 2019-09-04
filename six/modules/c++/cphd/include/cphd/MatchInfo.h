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

/*
 * Block containing information about match collection mc
 */
struct MatchCollection
{
    MatchCollection();

    bool operator==(const MatchCollection& other) const
    {
        return coreName == other.coreName && index == other.index &&
                matchIndex == other.matchIndex &&
                parameter == other.parameter;
    }

    bool operator!=(const MatchCollection& other) const
    {
        return !((*this) == other);
    }

    std::string coreName;
    size_t index;
    size_t matchIndex;
    six::ParameterCollection parameter;
};

/*
 * Block containing information about match type mt
 */
struct MatchType
{
    MatchType();

    bool operator==(const MatchType& other) const
    {
        return typeID == other.typeID && index == other.index &&
                currentIndex == other.currentIndex &&
                numMatchCollections == other.numMatchCollections &&
                matchCollection == other.matchCollection;
    }

    bool operator!=(const MatchType& other) const
    {
        return !((*this) == other);
    }

    std::string typeID;
    size_t index;
    size_t currentIndex;
    size_t numMatchCollections;
    std::vector<MatchCollection> matchCollection;
};

/*
 * (Optional) Information about other collections that are matched 
 * to the collection from which this CPHD product was generated
 * See section 5.2.3
 */
struct MatchInfo
{
    MatchInfo();

    bool operator==(const MatchInfo& other) const
    {
        return numMatchTypes == other.numMatchTypes &&
                matchType == other.matchType;
    }

    bool operator!=(const MatchInfo& other) const
    {
        return !((*this) == other);
    }

    size_t numMatchTypes;
    std::vector<MatchType> matchType;
};

std::ostream& operator<< (std::ostream& os, const MatchCollection& m);
std::ostream& operator<< (std::ostream& os, const MatchType& m);
std::ostream& operator<< (std::ostream& os, const MatchInfo& m);
}

#endif
