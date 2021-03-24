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

#include "CPlusPlus.h"

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
    #define CODA_OSS_sys_Endian 201907L // __cpp_lib_endian
}

#endif  // CODA_OSS_sys_Bit_h_INCLUDED_
