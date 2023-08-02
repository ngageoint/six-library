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
using zint8_t = six::ComplexInteger<int8_t>;
using zint32_t = six::ComplexInteger<int32_t>;
using zint64_t = six::ComplexInteger<int64_t>;

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

enum class Version
{
    v100, //  {"1.0.0", xml::lite::Uri("urn:CPHD:1.0.0")},
    v101, // {"1.0.1", xml::lite::Uri("http://api.nsgreg.nga.mil/schema/cphd/1.0.1")},
    v110, // {"1.1.0", xml::lite::Uri("http://api.nsgreg.nga.mil/schema/cphd/1.1.0")}
};
std::string to_string(Version); // "1.0.0", "1.0.1", "1.1.0"

// Existing code handles this via XML validation, not C++.
using ZeroToOne = double;
using NegHalfToHalf = double;  // [-0.5–0.5]
using PositiveDouble = double;
using XsNonNegativeInteger = size_t; // type="xs:nonNegativeInteger"/>
using XsPositiveInteger_fixed2 = size_t; // type="xs:positiveInteger" fixed="2"/>

}


#endif // SIX_cphd_Types_h_INCLUDED_