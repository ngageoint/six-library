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

#ifndef __CPHD_COLLECTION_ID_H__
#define __CPHD_COLLECTION_ID_H__

#include <six/Types.h>
#include <six/Init.h>
#include <six/CollectionInformation.h>

namespace cphd
{
/*!
 *  \struct CollectionID
 *  \brief CollectionID parameter
 *
 *  Collection ID used by CPHD, representing the tag
 *  <CollectionID>. This block contains general information about
 *  the collection.
 */
struct CollectionID
{

    //!  Constructor
    CollectionID();

    //!  Destructor
    virtual ~CollectionID()
    {
    }

    //! Equality operators
    bool operator==(const CollectionID& other) const
    {
        return collectInfo == other.collectInfo &&
               releaseInfo == other.releaseInfo;
    }
    bool operator!=(const CollectionID& other) const
    {
        return !((*this) == other);
    }

    /*!
     *  Contains product release information.
     *  Default value is UNRESTRICTED
     */
    std::string releaseInfo;

    /*
     *  Contains all other collectionID metadata
     *  From six/CollectionInformation
     */
    six::CollectionInformation collectInfo;

};

//! Ostream operators
std::ostream& operator<< (std::ostream& os, const CollectionID& c);
}
#endif

