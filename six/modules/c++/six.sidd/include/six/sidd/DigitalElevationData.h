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
#ifndef __SIX_SIDD_DIGITAL_ELEVATION_DATA_H__
#define __SIX_SIDD_DIGITAL_ELEVATION_DATA_H__

#include <string>

#include <six/Types.h>
#include <six/sidd/Enums.h>

namespace six
{
namespace sidd
{
struct GeographicCoordinates
{
    GeographicCoordinates();

    double longitudeDensity;
    double latitudeDensity;
    LatLon referenceOrigin;
};

struct Geopositioning
{
    Geopositioning();

    CoordinateSystemType coordinateSystemType;

    // TODO: These might be enums
    std::string geodeticDatum;
    std::string referenceEllipsoid;
    std::string verticalDatum;
    std::string soundingDatum; // TODO: Might be optional

    size_t falseOrigin;

    size_t utmGridZoneNumber; // Only present for UTM
};

struct PositionalAccuracy
{
    PositionalAccuracy();

    size_t positionalAccuracyRegions;

    double absoluteAccuracyHorizontal;
    double absoluteAccuracyVertical;

    double pointToPointAccuracyHorizontal;
    double pointToPointAccuracyVertical;
};

struct DigitalElevationData
{
    GeographicCoordinates geographicCoordinates;
    Geopositioning geopositioning;
    PositionalAccuracy positionalAccuracy;
};
}
}

#endif
