/* =========================================================================
 * This file is file is part of six.sidd-c++
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
#include "six/sidd30/ProductCreation.h"

//using namespace six;
//using namespace sidd;

// Windows also has a ProcessorInformation in the API
bool six::sidd::ProcessorInformation::operator==(const six::sidd::ProcessorInformation& rhs) const
{
    return (application == rhs.application &&
        processingDateTime == rhs.processingDateTime &&
        site == rhs.site &&
        profile == rhs.profile);
}

bool six::sidd::ProductCreation::operator==(const six::sidd::ProductCreation& rhs) const
{
    return (processorInformation == rhs.processorInformation &&
        classification == rhs.classification &&
        productName == rhs.productName &&
        productClass == rhs.productClass &&
        productType == rhs.productType &&
        productCreationExtensions == rhs.productCreationExtensions);
}
