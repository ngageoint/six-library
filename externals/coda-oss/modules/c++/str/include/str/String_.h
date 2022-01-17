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

#include "config/compiler_extensions.h"
#include "coda_oss/string.h"
namespace str
{
    CODA_OSS_disable_warning_push
    #if _MSC_VER
    #pragma warning(disable: 5052) // Keyword '...' was introduced in C++20 and requires use of the '...' command-line option
    #endif
    using Char8_T = coda_oss::char8_t;
    CODA_OSS_disable_warning_pop
    using U8string = coda_oss::u8string;
}
namespace sys
{
    using Char8_T = str::Char8_T;
    using U8string = str::U8string;
}

#endif  // CODA_OSS_str_String__h_INCLUDED_
