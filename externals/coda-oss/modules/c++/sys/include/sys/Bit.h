/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2020, Maxar Technologies, Inc.
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
 * License along with this program; If not, http://www.gnu.org/licenses/.
 *
 */
#ifndef CODA_OSS_sys_Bit_h_INCLUDED_
#define CODA_OSS_sys_Bit_h_INCLUDED_
#pragma once

#include "Conf.h"

namespace sys
{
    // https://en.cppreference.com/w/cpp/types/endian
    enum class Endian
    {
    #ifdef _WIN32
        little = 0,
        big = 1,
        native = little
    #else
        little = __ORDER_LITTLE_ENDIAN__,
        big = __ORDER_BIG_ENDIAN__,
        native = __BYTE_ORDER__
    #endif
    };
}

#ifndef CODA_OSS_DEFINE_std_endian_
    #if CODA_OSS_cpp20 && __has_include(<bit>)  // __has_include is C++17
        #define CODA_OSS_DEFINE_std_endian_ -1  // OK to #include <>, below
    #else
        #define CODA_OSS_DEFINE_std_endian_ CODA_OSS_AUGMENT_std_namespace // maybe use our own
    #endif  // CODA_OSS_cpp20
#endif  // CODA_OSS_DEFINE_std_endian_

#if CODA_OSS_DEFINE_std_endian_ == 1
    namespace std // This is slightly uncouth: we're not supposed to augment "std".
    {
        using endian = sys::Endian;
    }
#elif CODA_OSS_DEFINE_std_endian_ == -1  // set above
    #include <bit>
#endif // CODA_OSS_DEFINE_std_endian_

#endif  // CODA_OSS_sys_Bit_h_INCLUDED_
