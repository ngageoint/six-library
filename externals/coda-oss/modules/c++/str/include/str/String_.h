/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_str_String__h_INCLUDED_
#define CODA_OSS_str_String__h_INCLUDED_
#pragma once

// prefer "sys/String.h" instead of this file

#include <string>

namespace str
{
    // Char8_T for UTF-8 characters
    enum Char8_T : unsigned char { }; // https://en.cppreference.com/w/cpp/language/types
    using U8string = std::basic_string<Char8_T>; // https://en.cppreference.com/w/cpp/string
}
#if !defined(CODA_OSS_sys_U8string_DEFINED_)
#define CODA_OSS_sys_U8string_DEFINED_ 1
namespace sys
{
    using Char8_T = str::Char8_T;
    using U8string = str::U8string;
}
#endif  // CODA_OSS_sys_U8string_DEFINED_


#ifndef CODA_OSS_AUGMENT_std_u8string_
    #define CODA_OSS_AUGMENT_std_u8string_ CODA_OSS_AUGMENT_std_namespace // maybe use our own
#endif
#ifndef CODA_OSS_DEFINE_std_u8string_
    // MSVC only sets __cplusplus >199711L with the /Zc:__cplusplus command-line option.
    // https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
    #if (__cplusplus >= 202002L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L)) || CODA_OSS_cpp20
        #define CODA_OSS_DEFINE_std_u8string_ 0  // part of C++20
        #define CODA_OSS_lib_char8_t 201907
    #else
        #define CODA_OSS_DEFINE_std_u8string_ CODA_OSS_AUGMENT_std_u8string_ 
    #endif  // __cplusplus
#endif  // CODA_OSS_DEFINE_std_u8string_

#if CODA_OSS_DEFINE_std_u8string_ == 1
    namespace std // This is slightly uncouth: we're not supposed to augment "std".
    {
        #if defined(_MSC_VER) && (_MSC_VER >= 1927)
        #pragma warning(push)
        #pragma warning(disable: 5052) // Keyword '...' was introduced in C++ 20 and requires use of the '...' command-line option
        #endif
        using char8_t = ::str::Char8_T;
        #if defined(_MSC_VER) && (_MSC_VER >= 1927)
        #pragma warning(pop)
        #endif
        using u8string = ::str::U8string;
    }
    #define CODA_OSS_lib_char8_t 201907
#endif  // CODA_OSS_DEFINE_std_u8string_

namespace coda_oss
{
    #if defined(_MSC_VER) && (_MSC_VER >= 1927)
    #pragma warning(push)
    #pragma warning(disable: 5052) // Keyword '...' was introduced in C++ 20 and requires use of the '...' command-line option
    #endif

    #if CODA_OSS_lib_char8_t
    using char8_t = std::char8_t;
    using u8string = std::u8string;
    #else
    using char8_t = ::str::Char8_T;
    using u8string = ::str::U8string;
    #endif

    #if defined(_MSC_VER) && (_MSC_VER >= 1927)
    #pragma warning(pop)
    #endif
}

#endif  // CODA_OSS_str_String__h_INCLUDED_
