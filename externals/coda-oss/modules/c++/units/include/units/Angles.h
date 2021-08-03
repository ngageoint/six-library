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
#ifndef CODA_OSS_units_Angles_h_INCLUDED_
#define CODA_OSS_units_Angles_h_INCLUDED_

#include <cmath>

#include "math/Constants.h"
#include "units/Unit.h"

namespace units
{
//
// Avoid confusion between degrees and radians
//
namespace tags
{
struct Radians final { };
struct Degrees final { };
//struct Gradians final { };
}

template <typename AngleTag, typename T>
using Angle = Unit<T, AngleTag>;

template <typename T>
using Radians = Angle<tags::Radians, T>;

template <typename T>
using Degrees = Angle<tags::Degrees, T>;
//template <typename T>
//using Gradians = Angle<tags::Gradians, T>;

template <typename T, typename TResult = T>
inline constexpr void convert(Radians<T> v, Degrees<TResult>& result) noexcept
{
    result.value() = v.value() * math::Constants::RADIANS_TO_DEGREES;
}
template <typename T, typename TResult = T>
inline constexpr void convert(Degrees<T> v, Radians<TResult>& result) noexcept
{
    result.value() = v.value() * math::Constants::DEGREES_TO_RADIANS;
}

template<typename Tag, typename T>
inline Radians<T> toRadians(Angle<Tag, T> v) noexcept
{
  //return v.to<tags::Radians>();
  Radians<T> retval{ 0 };
  convert(v, retval);
  return retval;
}

template <typename Tag, typename T>
inline T sin(Angle<Tag, T> v) noexcept
{
  //return std::sin(v.to<tags::Radians>().value());
  return std::sin(toRadians(v).value());
}
template <typename Tag, typename T>
inline T cos(Angle<Tag, T> v) noexcept
{
  //return std::cos(v.to<tags::Radians>().value());
  return std::cos(toRadians(v).value());
}
template <typename Tag, typename T>
inline T tan(Angle<Tag, T> v) noexcept
{
  //return std::tan(v.to<tags::Radians>().value());
  return std::tan(toRadians(v).value());
}
}

#endif  // CODA_OSS_units_Angles_h_INCLUDED_
