/* =========================================================================
 * This file is part of coda_oss-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * coda_oss-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_coda_oss_span_h_INCLUDED_
#define CODA_OSS_coda_oss_span_h_INCLUDED_

#include <stdint.h>
#include <stdlib.h>
#include<stddef.h>

#include <cstddef>
#include <type_traits>

#include "coda_oss/CPlusPlus.h"
#include "coda_oss/namespace_.h"
#include "coda_oss/span_.h"
#include "coda_oss/cstddef.h" // byte

// Need a fairly decent C++ compiler to use the real GSL.  This brings in more than 
// we really need for span (e.g., gsl::narrow()), but it keeps things simple.
#include "gsl/gsl.h"  // not gsl/span; need #pragma here to turn off warnings

// This logic needs to be here rather than <std/span> so that `coda_oss::span` will
// be the same as `std::span`.
#ifndef CODA_OSS_HAVE_std_span_
    #define CODA_OSS_HAVE_std_span_ 0  // assume no <span>
#endif
#if CODA_OSS_cpp17 // C++17 for `__has_include()`
    #if __has_include(<span>) && __cpp_lib_span // Some versions of G++ say they're C++20 but don't have <span>
        #include <span>
        #undef CODA_OSS_HAVE_std_span_
        #define CODA_OSS_HAVE_std_span_ 1  // provided by the implementation, probably C++20
    #endif
#endif // CODA_OSS_cpp17

namespace coda_oss
{
    #if CODA_OSS_HAVE_std_span_
        using std::span; // coda_oss::span == std::span
    #elif defined(GSL_SPAN_H) // the above #include'd gsl/span
	    using gsl::span;
    #else
	    using details::span; // no std::span or gsl::span, use our own
    #endif 


// Even if `span` is `std::span`, these are still in the `coda_oss` namespace.

// https://en.cppreference.com/w/cpp/container/span/as_bytes
template <typename T>
inline auto as_bytes(span<const T> s) noexcept
{
    // https://en.cppreference.com/w/cpp/types/is_trivially_copyable "... serialized to/from binary files ..."
    static_assert(std::is_trivially_copyable<T>::value, "must be 'trivially' copyable.");

    const void* const p_ = s.data();
    auto const p = static_cast<const byte*>(p_);
    return span<const byte>(p, s.size_bytes());
}
template <typename T>
inline auto as_bytes(span<T> s) noexcept
{
    const span<const T> s_(s.data(), s.size());
    return as_bytes(s_);
}
template <typename T>
inline span<byte> as_writable_bytes(span<T> s) noexcept
{
    // https://en.cppreference.com/w/cpp/types/is_trivially_copyable "... serialized to/from binary files ..."
    static_assert(std::is_trivially_copyable<T>::value, "must be 'trivially' copyable.");

    static_assert(!std::is_const<T>::value, "T cannot be 'const'");

    void* const p_ = s.data();
    auto const p = static_cast<byte*>(p_);
    return span<byte>(p, s.size_bytes());
}

// https://en.cppreference.com/w/cpp/iterator/size
template <typename C>
constexpr size_t size(const C& c)
{
    return c.size();
}
template <typename C>
constexpr ptrdiff_t ssize(const C& c)
{
    return gsl::narrow<ptrdiff_t>(c.size());
}

template <typename T, size_t N>
constexpr size_t size(const T (&)[N]) noexcept
{
    return N;
}
template <typename T, ptrdiff_t N>
constexpr ptrdiff_t ssize(const T (&)[N]) noexcept
{
    return N;
}

}

#endif  // CODA_OSS_coda_oss_span_h_INCLUDED_
