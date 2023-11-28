/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#pragma once
#ifndef __SIX_MATCH_INFORMATION_H__
#define __SIX_MATCH_INFORMATION_H__

#include <six/Types.h>
#include <six/Init.h>
#include <six/Parameter.h>
#include <six/ParameterCollection.h>
#include <six/Exports.h>

namespace six
{
/*!
 *  \struct MatchCollect
 *  \brief  Block containing information about match collection. Block repeated
 *          for match collection = 1 to NumMatchCollections.
 *
 *  Added in SICD 1.0.0
 *  This object is used for fields previously in SICD 0.4.1
 *      - coreName
 *      - parameters
 */
struct SIX_SIX_API MatchCollect
{
    //! Constructor
    MatchCollect();

    //! Text string that uniquely identifies the matching collection.
    std::string coreName;

    //! Collection sequence index for the match collection.
    //  Added in SICD 1.0.0
    int matchIndex;

    //! Relevant match parameter. Attribute name identifies the parameter.
    ParameterCollection parameters;

    //! Equality operator
    bool operator==(const MatchCollect& rhs) const
    {
        return (coreName == rhs.coreName && matchIndex == rhs.matchIndex &&
            parameters == rhs.parameters);
    }

    bool operator!=(const MatchCollect& rhs) const
    {
        return !(*this == rhs);
    }
};

/*!
 *  \struct MatchCollection
 *  \brief SICD and SIDD MatchInfo/Collect
 *
 *  Block containing information about the n'th matched
 *  collection.  Match types are text strings describing the
 *  collection (e.g., COHERENT, STEREO)
 */
struct SIX_SIX_API MatchType
{
    //! Constructor
    MatchType();

    //! Platform id.  List rcv only platform for bistatic colls
    //  only exists in SICD 0.4.1
    std::string collectorName;

    //! Optional - tx platform identifier for bistatic match colls
    //  only exists in SICD 0.4.1
    std::string illuminatorName;

    //! Match type is a string describing the collect
    //  only exists in SICD 0.4.1
    std::vector<std::string> matchType;

    //! Text string identifying the match type. Examples: COHERENT, STEREO
    //  added in SICD 1.0.0, present in SIDD 2.0.0
    std::string typeID;

    //! Optional - Collection sequence index for the current collection.
    //  added in SICD 1.0.0, present in SIDD 2.0.0
    int currentIndex;

    //! Block containing information about match collection. Block repeated
    //  for match collection = 1 to NumMatchCollections.
    std::vector<MatchCollect> matchCollects;

    bool operator==(const MatchType& rhs) const;
    bool operator!=(const MatchType& rhs) const
    {
        return !(*this == rhs);
    }
};

/*!
 *  \struct MatchInformation
 *  \brief SICD and SIDD (starting in 2.0) MatchInfo
 *
 *  Parameters describing other imaging collections that are matched
 *  or related to this collection.  This parameter block is optional
 *  in the SICD and SIDD specs.
 */
struct SIX_SIX_API MatchInformation
{
public:
    //!  Constructor.  Creates a single default-constructed collection.
    MatchInformation();

    /*!
     *  At least one is mandatory.
     *  called "Collect" in SICD 0.4.x
     *  called "MatchType" in SICD/SIDD 1.x
     */
    std::vector<MatchType> types;

    //! Equality operator
    bool operator==(const MatchInformation& rhs) const
    {
        return types == rhs.types;
    }

    bool operator!=(const MatchInformation& rhs) const
    {
        return !(*this == rhs);
    }

    /*
     *  Ostream operators for six::MatchInformation type
     */
    friend std::ostream& operator<< (std::ostream& os, const MatchInformation& m);
};
}

#endif
