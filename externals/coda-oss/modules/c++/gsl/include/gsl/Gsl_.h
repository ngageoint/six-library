/* =========================================================================
 * This file is part of gsl-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
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
#ifndef CODA_OSS_gsl_Gsl__h_INCLUDED_
#define CODA_OSS_gsl_Gsl__h_INCLUDED_
#pragma once

#include <assert.h>

#include <exception>
#include <type_traits>
#include <utility>

#include <config/compiler_extensions.h>
#include "gsl/use_gsl.h" // Can't compile all of GSL with older versions of GCC/MSVC
#include "gsl/gsl_span_.h"

#if defined(__INTEL_COMPILER) // ICC, high-side
// Don't have access to Intel compiler on the low-side so just turn this off
// completely ... far too much hassle trying to get push/pop/etc. right
#pragma warning(disable: 3377) // "constexpr function return is non-constant"
#endif

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

        CODA_OSS_disable_warning_push
        CODA_OSS_UNREFERENCED_FORMAL_PARAMETER
        template <class T, class U>
        constexpr T narrow2_(T t, U u) noexcept(false)
        {
            return (!is_same_signedness<T, U>::value && ((t < T{}) != (u < U{}))) ?
                narrow_throw_exception(t) : t;
        }
        CODA_OSS_disable_warning_pop

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

#if !CODA_OSS_use_real_gsl
// Add to "gsl" if we're not using the real thing
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
 }
#endif // CODA_OSS_use_real_gsl

#endif  // CODA_OSS_gsl_Gsl__h_INCLUDED_
