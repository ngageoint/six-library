/* =========================================================================
 * This file is part of coda_oss-c++
 * =========================================================================
 *
 * (C) Copyright 2020-2022, Maxar Technologies, Inc.
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
#ifndef CODA_OSS_coda_oss_string_h_INCLUDED_
#define CODA_OSS_coda_oss_string_h_INCLUDED_

#include <string>

#include "coda_oss/CPlusPlus.h"

// This logic needs to be here rather than <std/string> so that `coda_oss::u8string` will
// be the same as `std::u8string`.
#ifndef CODA_OSS_HAVE_std_u8string_
    #define CODA_OSS_HAVE_std_u8string_ 0  // assume no std::u8string
#endif
#if CODA_OSS_cpp20
    #if defined(__cpp_lib_char8_t)  // https://en.cppreference.com/w/cpp/feature_test
        #undef CODA_OSS_HAVE_std_u8string_
        #define CODA_OSS_HAVE_std_u8string_ 1
    #endif
#endif // CODA_OSS_cpp20

#include "config/compiler_extensions.h"

namespace coda_oss
{
    // char8_t for UTF-8 characters
    CODA_OSS_disable_warning_push
    #if _MSC_VER
    #pragma warning(disable: 5052) // Keyword '...' was introduced in C++20 and requires use of the '...' command-line option
    #endif

    #if !defined(__cpp_char8_t) // https://en.cppreference.com/w/cpp/feature_test
        enum class char8_t : unsigned char { }; // https://en.cppreference.com/w/cpp/language/types
    #elif CODA_OSS_HAVE_std_u8string_
       // `char8_t` is a keyword in C++20
    #endif
    #if CODA_OSS_HAVE_std_u8string_
        using u8string = std::u8string;
    #else
        using u8string = std::basic_string<char8_t>; // https://en.cppreference.com/w/cpp/string
    #endif

    CODA_OSS_disable_warning_pop
}

#endif  // CODA_OSS_coda_oss_string_h_INCLUDED_
