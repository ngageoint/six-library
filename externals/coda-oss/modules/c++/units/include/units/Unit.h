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

    template <typename ResultTag = Tag, typename TReturn = T>
    constexpr Unit<TReturn, ResultTag> to() const noexcept;
};

template<typename Tag, typename T>
inline constexpr Unit<T, Tag> as(T v) noexcept
{
    return Unit<T, Tag>(v);
}

template <typename T, typename Tag, typename ResultTag = Tag, typename TResult = T>
inline constexpr void convert(Unit<T, Tag> v, Unit<TResult, ResultTag>& result) noexcept
{
    result = as<Tag, TResult>(v.value());
}

template <typename T, typename Tag>
template <typename ResultTag, typename TReturn>
constexpr Unit<TReturn, ResultTag> Unit<T, Tag>::to() const noexcept
{
    Unit<TReturn, ResultTag> retval{ 0 };
    convert(*this, retval);
    return retval;
}

}

#endif  // CODA_OSS_units_Unit_h_INCLUDED_
