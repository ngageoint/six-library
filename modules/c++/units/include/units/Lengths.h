/* =========================================================================
 * This file is part of units-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
 *
 * units-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_units_Lengths_h_INCLUDED_
#define CODA_OSS_units_Lengths_h_INCLUDED_

#include "math/Constants.h"
#include "units/Unit.h"

namespace units
{
//
// Avoid confusion between feet and meters
//
namespace tags
{
struct Feet final { };
struct Meters final { };
}

template <typename LengthTag, typename T>
using Length = Unit<T, LengthTag>;

template <typename T>
using Feet = Length<tags::Feet, T>;

template <typename T>
using Meters = Length<tags::Meters, T>;

template <typename T, typename TResult = T>
inline /*constexpr*/ Feet<TResult>& convert(Meters<T> v, Feet<TResult>& result) noexcept
{
    result.value() = v.value() * math::Constants::METERS_TO_FEET;
    return result;  // ICC doesn't like "constexpr void"
}
template <typename T, typename TResult = T>
inline /*constexpr*/ Meters<TResult>& convert(Feet<T> v, Meters<TResult>& result) noexcept
{
    result.value() = v.value() * math::Constants::FEET_TO_METERS;
    return result;  // ICC doesn't like "constexpr void"
}

namespace tags
{
struct NauticalMiles final { };
}

template <typename T>
using NauticalMiles = Length<tags::NauticalMiles, T>;

template <typename T, typename TResult = T>
inline /*constexpr*/ Feet<TResult>& convert(NauticalMiles<T> v, Feet<TResult>& result) noexcept
{
    result.value() = v.value() * math::Constants::NAUTICAL_MILES_TO_FEET;
    return result;  // ICC doesn't like "constexpr void"
}
template <typename T, typename TResult = T>
inline /*constexpr*/ Meters<TResult>& convert(NauticalMiles<T> v, Meters<TResult>& result) noexcept
{
    result.value() = v.value() * math::Constants::NAUTICAL_MILES_TO_METERS;
    return result;  // ICC doesn't like "constexpr void"
}
template <typename T, typename TResult = T>
inline /*constexpr*/ NauticalMiles<TResult>& convert(Meters<T> v, NauticalMiles<TResult>& result) noexcept
{
    result.value() = v.value() * math::Constants::METERS_TO_NAUTICAL_MILES;
    return result;  // ICC doesn't like "constexpr void"
}
}

#endif  // CODA_OSS_units_Lengths_h_INCLUDED_
