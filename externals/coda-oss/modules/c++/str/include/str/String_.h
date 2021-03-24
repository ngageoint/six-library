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
   #define CODA_OSS_sys_Char8_T 201907L // c.f., __cpp_lib_char8_t
}
#endif  // CODA_OSS_sys_U8string_DEFINED_

#endif  // CODA_OSS_str_String__h_INCLUDED_
