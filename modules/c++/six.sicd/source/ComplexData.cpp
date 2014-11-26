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
#include "six/sicd/ComplexData.h"

namespace six
{
namespace sicd
{
const char ComplexData::VENDOR_ID[] = "GDAIS";

Data* ComplexData::clone() const
{
    return new ComplexData(*this);
}

ComplexData::ComplexData() :
    collectionInformation(new CollectionInformation()),
    imageData(new ImageData()),
    geoData(new GeoData()),
    grid(new Grid()),
    timeline(new Timeline()),
    position(new Position()),
    radarCollection(new RadarCollection()),
    imageFormation(new ImageFormation()),
    scpcoa(new SCPCOA()),
    mVersion("1.1.0")
{
    //only initialize the mandatory elements above
}

DateTime ComplexData::getCollectionStartDateTime() const
{
    return timeline->collectStart;
}
}
}
