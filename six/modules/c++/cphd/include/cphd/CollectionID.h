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
 */#ifndef __CPHD_COLLECTION_ID_H__
#define __CPHD_COLLECTION_ID_H__

#include <six/Types.h>
#include <six/Init.h>
#include <six/Parameter.h>
#include <six/ParameterCollection.h>
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
struct CollectionID : public six::CollectionInformation
{

    //!  Constructor
    CollectionID();

    //!  Destructor
    virtual ~CollectionID()
    {
    }

    /*!
     * Contains product release information.
     * Default value is UNRESTRICTED
     */
    std::string releaseInfo;

    //! Deep copy
    CollectionID* clone() const;

    bool operator==(const CollectionID& other) const
    {
        return collectorName == other.collectorName &&
               collectType == other.collectType &&
               illuminatorName == other.illuminatorName &&
               coreName == other.coreName &&
               radarMode == other.radarMode &&
               radarModeID == other.radarModeID &&
               releaseInfo == other.releaseInfo &&
               getClassificationLevel() == other.getClassificationLevel();
    }

    bool operator!=(const CollectionID& other) const
    {
        return !((*this) == other);
    }

    inline virtual std::string getClassificationLevel() const
    {
        return mClassification;
    }

    inline virtual void setClassificationLevel(const std::string& classification)
    {
        mClassification = classification;
    }

private:
    /*!
     *  Classification level
     */
    std::string mClassification;
};

std::ostream& operator<< (std::ostream& os, const CollectionID& c);
}
#endif

