/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
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
#include "six/sicd/ComplexData.h"

using namespace six;
using namespace six::sicd;

const char ComplexData::VENDOR_ID[] = "GDAIS";

Data* ComplexData::clone() const
{
    return new ComplexData(*this);
}

ComplexData::ComplexData() : 
    mVersion("1.0.1")
{
    //only initialize the mandatory elements
    collectionInformation.reset(new CollectionInformation());
    imageData.reset(new ImageData());
    geoData.reset(new GeoData());
    grid.reset(new Grid());
    timeline.reset(new Timeline());
    position.reset(new Position());
    radarCollection.reset(new RadarCollection());
    imageFormation.reset(new ImageFormation());
    scpcoa.reset(new SCPCOA());
}

