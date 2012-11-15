/* =========================================================================
 * This file is part of six.sidd-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

using namespace six;
using namespace six::sidd;

const char DerivedData::VENDOR_ID[] = "GDAIS";

DerivedData::DerivedData() :
    productCreation(new ProductCreation), display(NULL),
            geographicAndTarget(NULL), measurement(NULL),
            exploitationFeatures(NULL), productProcessing(NULL),
            downstreamReprocessing(NULL), errorStatistics(NULL),
            radiometric(NULL),
            mVersion("1.0.0")
{
}

Data* DerivedData::clone() const
{
    return new DerivedData(*this);
}
