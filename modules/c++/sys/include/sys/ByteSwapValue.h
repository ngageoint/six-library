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
#include <assert.h>

#include <type_traits>
#include <coda_oss/span.h>
#include <coda_oss/cstddef.h>
#include <coda_oss/bit.h>
#include <tuple>
#include <vector>
#include <array>
#include <stdexcept>

#include "config/Exports.h"

#include "Span.h"

#include <type_traits>
#include <coda_oss/span.h>
#include <coda_oss/cstddef.h>
#include <tuple>
#include <vector>
#include <array>
#include <stdexcept>

namespace sys
{
    /*!
     * Swap bytes for a single value into output buffer.  API is `span<byte>` rather than `void*` since
     * for a single value we know the size.  These "low level" routines may be less efficient than
     * the templates since it's not possible to specialize on a specific size.
     *
     *  \param buffer to transform
     *  \param[out] outputBuffer buffer to write swapped elements to
     */
    coda_oss::span<const coda_oss::byte> CODA_OSS_API byteSwap(
        coda_oss::span<const coda_oss::byte> pIn, coda_oss::span<coda_oss::byte> outPtr);

    namespace details
    {
    template <typename TUInt>
    inline auto swapUIntBytes(coda_oss::span<const coda_oss::byte> inBytes, coda_oss::span<coda_oss::byte> outBytes,
        std::nothrow_t) noexcept
    {
        static_assert(std::is_unsigned<TUInt>::value, "TUInt must be 'unsigned'");
        assert(sizeof(TUInt) == inBytes.size());
        assert(inBytes.size() == outBytes.size());

        const void* const pIn_ = inBytes.data();
        auto const pIn = static_cast<const TUInt*>(pIn_);
        void* const pOut_ = outBytes.data();
        auto const pOut = static_cast<TUInt*>(pOut_);
        
        *pOut = coda_oss::byteswap(*pIn); // at long last, swap the bytes

        // Give the raw byte-swapped bytes back to the caller for easy serialization
        return as_bytes(pOut);
    }
    template <typename TUInt>
    inline auto swapUIntBytes(coda_oss::span<const coda_oss::byte> inBytes, coda_oss::span<coda_oss::byte> outBytes)
    {
        if (sizeof(TUInt) != inBytes.size())
        {
            throw std::invalid_argument("'inBytes.size() != sizeof(TUInt)");
        }
        if (inBytes.size() != outBytes.size())
        {
            throw std::invalid_argument("'inBytes.size() != outBytes.size()");
        }
        return swapUIntBytes<TUInt>(inBytes, outBytes, std::nothrow);
    }

    // This is a template so that we can have specializations for different sizes.
    // By specializing on `size_t`, a `float` can be "cast" to `uint32_t` (via
    // `std::byte`) for byte-swapping.
    template <size_t elemSize>
    inline auto swapBytes(coda_oss::span<const coda_oss::byte> inBytes, coda_oss::span<coda_oss::byte> outBytes)
    {
        if (elemSize != inBytes.size())
        {
            throw std::invalid_argument("'inBytes.size() != elemSize");
        }
        return sys::byteSwap(inBytes, outBytes);  // size that wasn't specialized
    }

    // avoid copy-paste errors
    #define CODA_OSS_define_swapBytes_specialization_(T) template <> inline auto swapBytes<sizeof(T)> \
        (coda_oss::span<const coda_oss::byte> inBytes, coda_oss::span<coda_oss::byte> outBytes) { return swapUIntBytes<T>(inBytes, outBytes); }
    CODA_OSS_define_swapBytes_specialization_(uint8_t);
    CODA_OSS_define_swapBytes_specialization_(uint16_t);
    CODA_OSS_define_swapBytes_specialization_(uint32_t);
    CODA_OSS_define_swapBytes_specialization_(uint64_t);
    #undef CODA_OSS_define_swapBytes_specialization_

    template<typename T>
    inline constexpr bool is_byte_swappable() noexcept
    {
        // Trying to byte-swap anything other than integers is likely to cause
        // problems (or at least confusion):
        // * `struct`s have padding that should be ignored.
        // * each individual member of a `struct` should be byte-swaped
        // * byte-swaped `float` or `double` bits are nonsense
        return !std::is_compound<T>::value;
    }
    }

    /*!
    * Function to swap one element irrespective of size.
    * Returns the raw byte-swapped bytes for easy serialization.
    */
    template <typename T>
    inline auto swapBytes(coda_oss::span<const coda_oss::byte> inBytes, coda_oss::span<coda_oss::byte> outBytes)
    {
        static_assert(details::is_byte_swappable<T>(), "T should not be a 'struct'");
        return details::swapBytes<sizeof(T)>(inBytes, outBytes);
    }
    template <typename T>
    inline auto swapBytes(T in, coda_oss::span<coda_oss::byte> outBytes)
    {
        return swapBytes<T>(as_bytes(in), outBytes);
    }
    template <typename T>
    inline auto swapBytes(T in)
    {
        std::vector<coda_oss::byte> retval;
        retval.resize(sizeof(T));
        std::ignore = swapBytes(in, make_span(retval));
        return retval;
    }

    // Reverse the above: turn `span<byte>` back to T after byte-swapping
    template <typename T>
    inline auto swapBytes(coda_oss::span<const coda_oss::byte> in)
    {
        // Don't want to cast the swapped bytes in `in` to T* as they might not be valid;
        // e.g., a byte-swapped `float` could be garbage.
        T retval;
        swapBytes<T>(in, as_writable_bytes(retval));
        return retval;
    }

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
        T out;
        std::ignore = swapBytes(val, as_writable_bytes(out));
        return out;
    }
}
#endif // CODA_OSS_sys_ByteSwapValue_h_INCLUDED_
