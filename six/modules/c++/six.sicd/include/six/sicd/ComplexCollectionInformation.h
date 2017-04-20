/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_COMPLEX_COLLECTION_INFORMATION_H__
#define __SIX_COMPLEX_COLLECTION_INFORMATION_H__

#include "six/Types.h"
#include "six/CollectionInformation.h"
#include "six/Init.h"
#include "six/Parameter.h"
#include "six/ParameterCollection.h"
#include "six/sicd/ComplexClassification.h"

namespace six
{
namespace sicd
{
/*!
 *  \struct CollectionInformation
 *  \brief SICD CollectionInfo parameter
 *
 *  Collection information used by SICD, representing the tag
 *  <CollectionInfo>.  This block contains general information about
 *  the collection.  There is nothing wrong with the SICD name, but
 *  for API consistency, naming is lengthened.
 */
struct ComplexCollectionInformation : public CollectionInformation
{
    ComplexCollectionInformation();
    ComplexCollectionInformation* clone() const;
    virtual ~ComplexCollectionInformation()
    {
    }

    inline virtual std::string getClassificationLevel() const
    {
        return mClassification.level;
    }

    inline virtual void setClassificationLevel(const std::string& classification)
    {
        mClassification.level = classification;
    }

    inline const ComplexClassification& getClassification() const
    {
        return mClassification;
    }

    inline ComplexClassification& getClassification()
    {
        return mClassification;
    }

private:
    ComplexClassification mClassification;
};

}
}
#endif

