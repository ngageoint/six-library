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
#ifndef CODA_OSS_units_Unit_h_INCLUDED_
#define CODA_OSS_units_Unit_h_INCLUDED_

namespace units
{
//
// Simple template to help avoid confusion between different units.
//
// This is intentionally simple for several reasons:
// * simple and easy to understand and maintain
// * getting things like "+" or "*" right can be (very) difficult
// * it's not even clear that doubling a temperature or a length of -4 has real meaning
// * there's really not that much code that needs to manipulate units
//
template <typename T, typename Tag>
struct Unit final
{
    using value_t = T;
    using tag_t = Tag;

    value_t value_;

    Unit() = delete;
    constexpr Unit(value_t v) noexcept : value_(v) { }

    /*constexpr*/ value_t& value() noexcept
    {
        return value_;
    }
    constexpr const value_t& value() const noexcept
    {
        return value_;
    }

    // Calls convert() to convert from Unit<int, feet> to Unit<double, meters>.
    // The defaults ensure converting to oneself works.
    template <typename ResultTag = Tag, typename TReturn = T>
    /*constexpr*/ Unit<TReturn, ResultTag> to() const noexcept;
};

// Make a unit specifying only the tag, inferring the type T.  Rather than
//    units::Unit<int, my_tag_t> u(123);
// (T specified as "int"), you can do
//    auto u = units::make_Unit<my_tag_t>(123);
// the type is inferred from "123".
template<typename Tag, typename T>
inline constexpr Unit<T, Tag> make_Unit(T v) noexcept
{
    return Unit<T, Tag>(v);
}

// Allow Unit::to() (below) to compile; the routine below normally won't be used.
template <typename T, typename Tag, typename ResultTag = Tag, typename TResult = T>
inline /*constexpr*/ Unit<TResult, ResultTag>& convert(Unit<T, Tag> v, Unit<TResult, ResultTag>& result) noexcept
{
    result = make_Unit<Tag, TResult>(v.value()); // or Unit<...>, this ensures make_Unit() works
    return result; // ICC doesn't like "constexpr void"; want to use parameters for type deduction
}

// Now that "convert" is visble (above), implement the Unit::to() member function.
// convert() might be easier to use as the tag/type for each can be inferred;
// the template argument to to() is the raw tag.  For example,
//   constexpr units::Meters<double> meters_1 = 1.0;
//   units::Feet<double> feet{0};
//   convert(meters_1, feet);
// Using to() is a bit more awkward
//    const auto meters = feet.to<units::tags::Meters>();
template <typename T, typename Tag>
template <typename ResultTag, typename TReturn>
inline /*constexpr*/ Unit<TReturn, ResultTag> Unit<T, Tag>::to() const noexcept
{
    Unit<TReturn, ResultTag> retval{ 0 };
    return convert(*this, retval);
}
}

#endif  // CODA_OSS_units_Unit_h_INCLUDED_
