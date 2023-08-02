/* =========================================================================
 * This file is part of coda_oss-c++
 * =========================================================================
 *
 * (C) Copyright 2020, Maxar Technologies, Inc.
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
 * License along with this program; If not, http://www.gnu.org/licenses/.
 *
 */
#pragma once
#ifndef CODA_OSS_coda_oss_optional_h_INCLUDED_
#define CODA_OSS_coda_oss_optional_h_INCLUDED_

#include "coda_oss/CPlusPlus.h"

// This logic needs to be here rather than <std/optional> so that `coda_oss::optional` will
// be the same as `std::optional`.
#ifndef CODA_OSS_HAVE_std_optional_
    #define CODA_OSS_HAVE_std_optional_ 0  // assume no <optional>
#endif
#if CODA_OSS_cpp17 // C++17 for `__has_include()`
    #if __has_include(<optional>) && __cpp_lib_optional
        #include <optional>
        #undef CODA_OSS_HAVE_std_optional_
        #define CODA_OSS_HAVE_std_optional_ 1  // provided by the implementation, probably C++17
    #endif
#endif // CODA_OSS_cpp17

#if !CODA_OSS_HAVE_std_optional_
#include "coda_oss/optional_.h"
#endif

namespace coda_oss
{
    #if CODA_OSS_HAVE_std_optional_
        using std::optional;
        using std::make_optional;
    #else
        using details::optional;
        using details::make_optional;
    #endif 
}

#endif  // CODA_OSS_coda_oss_optional_h_INCLUDED_
