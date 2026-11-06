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
#ifndef CODA_OSS_coda_oss_span_h_INCLUDED_
#define CODA_OSS_coda_oss_span_h_INCLUDED_

#include <cstdint>
#include <cstdlib>
#include <cstddef>
#include <type_traits>
#include <gsl/narrow>

#include "coda_oss/CPlusPlus.h"
#include "coda_oss/cstddef.h" // byte

// This logic needs to be here rather than <std/span> so that `coda_oss::span` will
// be the same as `coda_oss::span`.
#ifndef CODA_OSS_HAVE_std_span_
    #define CODA_OSS_HAVE_std_span_ 0  // assume no <span>
#endif
#if CODA_OSS_cpp17 // C++17 for `__has_include()`
    #if __has_include(<span>) && __cpp_lib_span && CODA_OSS_cpp20 // Some versions of G++ say they're C++20 but don't have <span>
        #include <span>
        #undef CODA_OSS_HAVE_std_span_
        #define CODA_OSS_HAVE_std_span_ 1  // provided by the implementation, probably C++20
    #endif
#endif // CODA_OSS_cpp17


#if ! CODA_OSS_HAVE_std_span_
#include <gsl/span>
#endif
namespace coda_oss
{
    #if CODA_OSS_HAVE_std_span_
        using std::span;
        using std::as_bytes;
        using std::as_writable_bytes;
        using std::dynamic_extent;
    #else
        using gsl::span;
        using gsl::as_bytes;
        using gsl::as_writable_bytes;
        using gsl::dynamic_extent;
    #endif

    // https://en.cppreference.com/cpp/feature_test#cpp_lib_nonmember_container_access
    #if !__cpp_lib_nonmember_container_access
    // https://en.cppreference.com/w/cpp/iterator/size
    template <typename C>
    constexpr size_t size(const C& c)
    {
        return c.size();
    }
    template <typename T, size_t N>
    constexpr size_t size(const T (&)[N]) noexcept
    {
        return N;
    }
    #else
    using std::size;
    #endif

    //https://en.cppreference.com/cpp/feature_test#cpp_lib_ssize
    #if !__cpp_lib_ssize
    template <typename C>
    constexpr ptrdiff_t ssize(const C& c)
    {
        return gsl::narrow<ptrdiff_t>(c.size());
    }
    template <typename T, ptrdiff_t N>
    constexpr ptrdiff_t ssize(const T (&)[N]) noexcept
    {
        return N;
    }
    #else
    using std::ssize;
    #endif
}

// Downstream projects use this alias even when std does not support various features.
// Preserve this until relevant projects are upgraded to high enough C++ version.
namespace std
{
using coda_oss::span;
using coda_oss::as_bytes;
using coda_oss::as_writable_bytes;
using coda_oss::dynamic_extent;

#if !__cpp_lib_nonmember_container_access
using coda_oss::size;
#endif

#if !__cpp_lib_ssize
using coda_oss::ssize;
#endif

}

#endif  // CODA_OSS_coda_oss_span_h_INCLUDED_
