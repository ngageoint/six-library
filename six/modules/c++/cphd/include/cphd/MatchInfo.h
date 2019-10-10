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
    //! Default constructor
    MatchCollection();

    //! Equality operators
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

    //! Text string that uniquely identifies the matching collection
    std::string coreName;

    //! Attribute index mc
    size_t index;

    //! (Optional) Collection sequence index for the match
    //! collection
    size_t matchIndex;

    //! (Optional) Relevant match parameter. Attribute name identifies
    //! the parameter
    six::ParameterCollection parameter;
};

/*
 * Block containing information about match type mt
 */
struct MatchType
{
    //! Default constructor
    MatchType();

    //! Equality operator
    bool operator==(const MatchType& other) const
    {
        return typeID == other.typeID && index == other.index &&
                currentIndex == other.currentIndex &&
                matchCollection == other.matchCollection;
    }
    bool operator!=(const MatchType& other) const
    {
        return !((*this) == other);
    }

    //! Text string identifying the match type
    std::string typeID;

    //! Attribute index = mt
    size_t index;

    //! (Optional) Collection sequence index for the collection from
    //! which this CPHD product was generated.
    size_t currentIndex;

    //! (Optional) Number of matched collections for this match
    //! type. 
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
        return matchType == other.matchType;
    }

    bool operator!=(const MatchInfo& other) const
    {
        return !((*this) == other);
    }

    std::vector<MatchType> matchType;
};

std::ostream& operator<< (std::ostream& os, const MatchCollection& m);
std::ostream& operator<< (std::ostream& os, const MatchType& m);
std::ostream& operator<< (std::ostream& os, const MatchInfo& m);
}

#endif
