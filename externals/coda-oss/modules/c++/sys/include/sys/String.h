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

#pragma once

#include <string>

#if !defined(CODA_OSS_sys_u8string_DEFINED_)
#define CODA_OSS_sys_u8string_DEFINED_ 1
namespace sys
{
    // Char8_T for UTF-8 characters
    #if __cplusplus >= 202002L  // C++20
    using Char8_T = char8_t;
    using u8string = std::u8string;
    #else
    enum Char8_T : unsigned char { }; // https://en.cppreference.com/w/cpp/language/types
    using u8string = std::basic_string<Char8_T>; // https://en.cppreference.com/w/cpp/string
    #endif  // __cplusplus
}
#endif  // CODA_OSS_sys_u8string_DEFINED_
