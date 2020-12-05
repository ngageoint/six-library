/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
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

#include <six/Init.h>
#include <six/sidd/DigitalElevationData.h>

namespace six
{
namespace sidd
{
GeographicCoordinates::GeographicCoordinates() :
    longitudeDensity(six::Init::undefined<double>()),
    latitudeDensity(six::Init::undefined<double>()),
    referenceOrigin(six::Init::undefined<double>(),
                    six::Init::undefined<double>())
{
}

Geopositioning::Geopositioning() :
    geodeticDatum("World Geodetic System 1984"),
    referenceEllipsoid("World Geodetic System 1984"),
    verticalDatum("Mean Sea Level"),
    soundingDatum("Mean Sea Level"),
    falseOrigin(six::Init::undefined<size_t>()),
    utmGridZoneNumber(six::Init::undefined<size_t>())
{
}

PositionalAccuracy::PositionalAccuracy() :
    numRegions(six::Init::undefined<size_t>()),
    absoluteAccuracyHorizontal(six::Init::undefined<double>()),
    absoluteAccuracyVertical(six::Init::undefined<double>()),
    pointToPointAccuracyHorizontal(six::Init::undefined<double>()),
    pointToPointAccuracyVertical(six::Init::undefined<double>())
{
}

DigitalElevationData::DigitalElevationData() :
    nullValue(six::Init::undefined<int64_t>())
{
}
}
}
