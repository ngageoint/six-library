/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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
#include "six/sidd/DerivedData.h"

namespace six
{
namespace sidd
{
const char DerivedData::VENDOR_ID[] = "GDAIS";

DerivedData::DerivedData() :
    productCreation(new ProductCreation),
    mVersion("1.0.0")
{
}

Data* DerivedData::clone() const
{
    return new DerivedData(*this);
}

DateTime DerivedData::getCollectionStartDateTime() const
{
    if (!exploitationFeatures.get() ||
        exploitationFeatures->collections.empty())
    {
        throw except::Exception(Ctxt("Must add a collection first"));
    }

    return exploitationFeatures->collections[0]->information->collectionDateTime;
}
}
}
