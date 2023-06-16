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
#pragma once
#ifndef SIX_cphd_Types_h_INCLUDED_
#define SIX_cphd_Types_h_INCLUDED_

#include <stdint.h>

#include <ostream>

#include <types/Complex.h>

#include <six/Types.h>
#include <six/Enums.h>
#include <six/CollectionInformation.h>
#include <six/MatchInformation.h>
#include <six/Utilities.h>
#include <six/sicd/GeoData.h>

namespace cphd
{
// Use the same types that SIX uses

using zfloat = six::zfloat;
using zdouble = six::zdouble;
using zint16_t = six::zint16_t;

#if CODA_OSS_types_unique_ComplexInteger
using zint8_t = std::complex<int8_t>; // TODO: types::zint8_t;
using zint32_t = std::complex<int32_t>; // TODO: types::zint32_t;
using zint64_t = std::complex<int64_t>; // TODO: types::zint64_t;
#else
using zint8_t = types::zint8_t;
using zint32_t = types::zint32_t;
using zint64_t = types::zint64_t;
#endif

typedef six::Vector2 Vector2;

typedef six::Vector3 Vector3;

typedef six::DateTime DateTime;
inline std::ostream& operator<<(std::ostream& os, const DateTime& value)
{
    os << six::toString(value);
    return os;
}

typedef six::UByte UByte;

typedef six::Poly1D Poly1D;

typedef six::Poly2D Poly2D;

typedef six::PolyXYZ PolyXYZ;
inline std::ostream& operator<<(std::ostream& os, const PolyXYZ& value)
{
    os << six::toString(value);
    return os;
}

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

typedef six::GeoInfo GeoInfo;

typedef six::MatchInformation MatchInformation;
}


#endif // SIX_cphd_Types_h_INCLUDED_