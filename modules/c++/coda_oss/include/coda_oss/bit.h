/* =========================================================================
 * This file is part of coda_oss-c++
 * =========================================================================
 *
 * (C) Copyright 2020-2022, Maxar Technologies, Inc.
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
#ifndef CODA_OSS_coda_oss_bit_h_INCLUDED_
#define CODA_OSS_coda_oss_bit_h_INCLUDED_

#include <stdlib.h>
#include <stdint.h>
#ifdef __GNUC__
#include <byteswap.h>  // "These functions are GNU extensions."
#endif

#include<bit>
#include <type_traits>

#include "coda_oss/namespace_.h"
namespace coda_oss
{
    using std::endian;


    // https://en.cppreference.com/w/cpp/numeric/byteswap
    namespace details
    {
    // Overloads for common types
    inline constexpr uint8_t byteswap(uint8_t val) noexcept
    {
        return val;  // no-op
    }
    #if defined(_MSC_VER)
    // These routines should generate a single instruction; see
    // https://devblogs.microsoft.com/cppblog/a-tour-of-4-msvc-backend-improvements/
    inline uint16_t byteswap(uint16_t val) noexcept
    {
        return _byteswap_ushort(val);
    }
    inline uint32_t byteswap(uint32_t val) noexcept
    {
        return _byteswap_ulong(val);
    }
    inline uint64_t byteswap(uint64_t val) noexcept
    {
        return _byteswap_uint64(val);
    }
    #elif defined(__GNUC__)
    inline uint16_t byteswap(uint16_t val) noexcept
    {
        return bswap_16(val);
    }
    inline uint32_t byteswap(uint32_t val) noexcept
    {
        return bswap_32(val);
    }
    inline uint64_t byteswap(uint64_t val) noexcept
    {
        return bswap_64(val);
    }
    #else
    #error "No platform-specific byteswap()" // TODO: do something else?
    #endif
    }

    template <typename T>
    inline T byteswap(T n) noexcept
    {
        // "std::byteswap participates in overload resolution only if T satisfies integral, i.e., T is an integer type. The program is
        // ill-formed if T has padding bits."
        static_assert(std::is_integral<T>::value, "T must be integral");

        using unsigned_t = std::make_unsigned_t<T>; // "Since C++14" https://en.cppreference.com/w/cpp/types/make_unsigned
        return details::byteswap(static_cast<unsigned_t>(n));
    }
}

#endif  // CODA_OSS_coda_oss_bit_h_INCLUDED_
