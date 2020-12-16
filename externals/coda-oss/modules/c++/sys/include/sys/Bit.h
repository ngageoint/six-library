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
#if CODA_OSS_cplusplus < 202002L  // pre-C++20
#define CODA_OSS_DEFINE_std_endian_ 1
#else
#define CODA_OSS_DEFINE_std_endian_ 0  // std::filesystem part of C++17
#endif  // CODA_OSS_cplusplus
#endif  // CODA_OSS_DEFINE_std_endian_

#if CODA_OSS_DEFINE_std_endian_
// This is ever-so-slightly uncouth: we're not supposed to augment "std".
namespace std
{
    using endian = sys::Endian;
}
#else
#include <bit>
#endif