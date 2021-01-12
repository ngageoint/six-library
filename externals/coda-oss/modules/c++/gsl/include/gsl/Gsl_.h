/* =========================================================================
 * This file is part of gsl-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * gsl-c++ is free software; you can redistribute it and/or modify
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

#ifndef CODA_OSS_Gsl__h_INCLUDED_
#define CODA_OSS_Gsl__h_INCLUDED_

#pragma once

// Can't compile all of GSL with older versions of GCC

#include <assert.h>

#include <exception>
#include <type_traits>
#include <utility>  

#include <mem/Span.h>

namespace Gsl
{
    // narrow_cast(): a searchable way to do narrowing casts of values
    template <class T, class U> constexpr T narrow_cast(U&& u) noexcept
    {
        return static_cast<T>(std::forward<U>(u));
    }

    struct narrowing_error final : public std::exception { };

    namespace details
    {
        template <typename Exception>
        [[noreturn]] void throw_exception(Exception&& e)
        {
            throw e;
        }

        template <class T, class U>
        struct is_same_signedness final
            : public std::integral_constant<bool, std::is_signed<T>::value == std::is_signed<U>::value> { };

        // this is messy to preserve "constexpr" with C++11
        template <class T>
        constexpr T narrow_throw_exception(T t) noexcept(false)
        {
            return throw_exception(narrowing_error()), t;
        }
        template <class T, class U>
        constexpr T narrow1_(T t, U u) noexcept(false)
        {
            return (static_cast<U>(t) != u) ? narrow_throw_exception(t) : t;
        }
        template <class T, class U>
        constexpr T narrow2_(T t, U u) noexcept(false)
        {
            return (!is_same_signedness<T, U>::value && ((t < T{}) != (u < U{}))) ?
                narrow_throw_exception(t) : t;
        }

        template <class T, class U>
        constexpr T narrow(T t, U u) noexcept(false)
        {
            return narrow1_(t, u) ? narrow2_(t, u) : t;
        }
    } // namespace details
    // narrow() : a checked version of narrow_cast() that throws if the cast changed the value
    template <class T, class U>
    constexpr T narrow(U u) noexcept(false)
    {
        return details::narrow(narrow_cast<T>(u), u);
    }
}

namespace gsl
{
    template <class T, class U>
    constexpr T narrow_cast(U&& u) noexcept
    {
        return Gsl::narrow_cast<T>(std::forward<U>(u));
    }

   template <class T, class U>
   constexpr T narrow(U u) noexcept(false)
    {
        return Gsl::narrow<T>(u);
    }


    template <typename T>
    using span = mem::Span<T>;

    template <typename T>
    inline span<T> make_span(T* d, size_t sz)
    {
        return mem::make_Span<T>(d, sz);
    }

    template <typename TContainer>
    inline span<typename TContainer::value_type> make_span(TContainer& c)
    {
        return mem::make_Span(c);
    }
    template <typename TContainer>
    inline span<typename TContainer::value_type> make_span(const TContainer& c)
    {
        return make_span(const_cast<TContainer&>(c));
    }
 }

#endif  // CODA_OSS_Gsl__h_INCLUDED_
