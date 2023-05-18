/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
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

#ifndef CODA_OSS_sys_ByteSwapValue_h_INCLUDED_
#define CODA_OSS_sys_ByteSwapValue_h_INCLUDED_
#pragma once

#include <stdint.h>
#include <stdlib.h>
#ifdef __GNUC__
#include <byteswap.h>  // "These functions are GNU extensions."
#endif

namespace sys
{
    // Overloads for common types
    inline constexpr uint8_t byteSwap(uint8_t val)
    {
        return val;  // no-op
    }
    #if defined(_MSC_VER)
    // These routines should generate a single instruction; see https://devblogs.microsoft.com/cppblog/a-tour-of-4-msvc-backend-improvements/
    inline uint16_t byteSwap(uint16_t val)
    {
        return _byteswap_ushort(val);
    }
    inline uint32_t byteSwap(uint32_t val)
    {
        return _byteswap_ulong(val);
    }
    inline uint64_t byteSwap(uint64_t val)
    {
        return _byteswap_uint64(val);
    }
    #elif defined(__GNUC__)
    inline uint16_t byteSwap(uint16_t val)
    {
        return bswap_16(val);
    }
    inline uint32_t byteSwap(uint32_t val)
    {
        return bswap_32(val);
    }
    inline uint64_t byteSwap(uint64_t val)
    {
        return bswap_64(val);
    }
    #endif


    /*!
     *  Function to swap one element irrespective of size.  The inplace
     *  buffer function should be preferred.
     *
     *  To specialize complex float, first include the complex library
     *  \code
        #include <complex>
     *  \endcode
     *
     *  Then put an overload in as specified below:
     *  \code
        template <typename T> std::complex<T> byteSwap(std::complex<T> val)
        {
            std::complex<T> out(byteSwap<T>(val.real()),
                                byteSwap<T>(val.imag()));
            return out;
        }
     *  \endcode
     *
     */
    template <typename T> inline T byteSwap(T val)
    {
        size_t size = sizeof(T);
        T out;

        unsigned char* cOut = reinterpret_cast<unsigned char*>(&out);
        unsigned char* cIn = reinterpret_cast<unsigned char*>(&val);
        for (size_t i = 0, j = size - 1; i < j; ++i, --j)
        {
            cOut[i] = cIn[j];
            cOut[j] = cIn[i];
        }
        return out;
    }
}
#endif // CODA_OSS_sys_ByteSwapValue_h_INCLUDED_
