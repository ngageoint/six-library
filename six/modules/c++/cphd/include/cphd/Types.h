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
#ifndef __CPHD_TYPES_H__
#define __CPHD_TYPES_H__

#include <six/Types.h>
#include <six/Enums.h>
#include <six/CollectionInformation.h>
#include <six/MatchInformation.h>
#include <six/sicd/GeoData.h>

namespace cphd
{
// Use the same types that SIX uses

typedef six::Vector2 Vector2;

typedef six::Vector3 Vector3;

typedef six::DateTime DateTime;

typedef six::UByte UByte;

typedef six::Poly1D Poly1D;

typedef six::Poly2D Poly2D;

typedef six::PolyXYZ PolyXYZ;

typedef six::LatLon LatLon;

typedef six::LatLonAlt LatLonAlt;

typedef six::LatLonCorners LatLonCorners;

typedef six::LatLonAltCorners LatLonAltCorners;

typedef six::FFTSign FFTSign;

typedef six::RadarModeType RadarModeType;

typedef six::CollectType CollectType;

typedef six::DataType DataType;

typedef six::BooleanType BooleanType;

typedef six::CollectionInformation CollectionInformation;

typedef six::sicd::GeoInfo GeoInfo;

typedef six::MatchInformation MatchInformation;
}


#endif
