/* =========================================================================
 * This file is part of coda_oss-c++
 * =========================================================================
 *
 * (C) Copyright 2020-2022, Maxar Technologies, Inc.
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

#ifndef CODA_OSS_coda_oss_string_h_INCLUDED_
#define CODA_OSS_coda_oss_string_h_INCLUDED_
#pragma once

#include <string>

#include "config/compiler_extensions.h"

#include "coda_oss/namespace_.h"
namespace coda_oss
{
    // char8_t for UTF-8 characters
    CODA_OSS_disable_warning_push
    #if _MSC_VER
    #pragma warning(disable: 5052) // Keyword '...' was introduced in C++20 and requires use of the '...' command-line option
    #endif
    enum class char8_t : unsigned char { }; // https://en.cppreference.com/w/cpp/language/types
    CODA_OSS_disable_warning_pop

    using u8string = std::basic_string<char8_t>; // https://en.cppreference.com/w/cpp/string
}

#endif  // CODA_OSS_coda_oss_string_h_INCLUDED_
