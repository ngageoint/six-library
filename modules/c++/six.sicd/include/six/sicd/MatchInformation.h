/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#ifndef __SIX_MATCH_INFORMATION_H__
#define __SIX_MATCH_INFORMATION_H__

#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"

namespace six
{
namespace sicd
{
/*!
 *  \struct MatchCollection
 *  \brief SICD MatchInfoo/Collect
 *
 *  Block containing information about the n'th matched
 *  collection.  Match types are text strings describing the
 *  collection (e.g., COHERENT, STEREO)
 */
struct MatchCollection
{

    //! Constructor
    MatchCollection()
    {
        // Optional
        illuminatorName = Init::undefined<std::string>();
    }

    //! Platform id.  List rcv only platform for bistatic colls
    std::string collectorName;

    //! tx platform identifier for bistatic match colls
    std::string illuminatorName;

    //! unique ID for collect
    std::string coreName;

    //! Match type is a string describing the collect
    std::vector<std::string> matchType;

    //! Relevant match params
    std::vector<Parameter> parameters;

    //! Destructor
    ~MatchCollection()
    {
    }
    //! Clone this object
    MatchCollection* clone() const
    {
        return new MatchCollection(*this);
    }

};

/*!
 *  \struct MatchInformation
 *  \brief SICD MatchInfo
 *
 *  Parameters describing other imaging collections that are matched
 *  or related to this collection.  This parameter block is optional
 *  in the SICD spec.
 */
struct MatchInformation
{
    /*!
     *  One or more Collect objcts, containing information about
     *  the Nth matched collection.  Collections are indexed from 1
     */
    std::vector<MatchCollection*> collects;

    //!  Constructor
    MatchInformation()
    {
        collects.push_back(new MatchCollection());
    }

    //!  Destructor (including non-NULL children)
    ~MatchInformation();


    //!  Clone (including clones of children)
    MatchInformation* clone() const;
};

}
}
#endif
