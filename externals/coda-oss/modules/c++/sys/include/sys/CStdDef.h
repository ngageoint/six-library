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
#ifndef CODA_OSS_sys_CStdDef_h_INCLUDED_
#define CODA_OSS_sys_CStdDef_h_INCLUDED_
#pragma once

#include <cstddef>

#include "Conf.h"

namespace sys
{
    // https://en.cppreference.com/w/cpp/types/byte
    enum class Byte : unsigned char {};
}

#ifndef CODA_OSS_DEFINE_std_byte_
    #if CODA_OSS_cpp17
        #if defined(__cpp_lib_byte) && (__cpp_lib_byte < 201603)
            #error "Wrong value for __cpp_lib_byte."
        #endif
        #define CODA_OSS_DEFINE_std_byte_ 0  // part of C++17
        #define CODA_OSS_lib_byte 1
    #else
        #define CODA_OSS_DEFINE_std_byte_ CODA_OSS_AUGMENT_std_namespace  // maybe use our own
    #endif  // CODA_OSS_cpp17
#endif  // CODA_OSS_DEFINE_std_byte_

#if CODA_OSS_DEFINE_std_byte_ == 1
    namespace std // This is slightly uncouth: we're not supposed to augment "std".
    {
        using byte = sys::Byte;
    }
    #define CODA_OSS_lib_byte 1
#endif  // CODA_OSS_DEFINE_std_byte_

namespace coda_oss
{
    #if CODA_OSS_lib_byte
    using byte = std::byte;
    #else
    using byte = ::sys::Byte;
    #endif
}

#endif  // CODA_OSS_sys_CStdDef_h_INCLUDED_
