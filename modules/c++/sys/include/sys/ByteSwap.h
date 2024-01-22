/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
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

#pragma once 
#ifndef CODA_OSS_sys_ByteSwap_h_INCLUDED_
#define CODA_OSS_sys_ByteSwap_h_INCLUDED_

#include <stdint.h>
#include <stdlib.h>

#include <coda_oss/span.h>
#include <coda_oss/cstddef.h>
#include <type_traits>
#include <stdexcept>
#include <complex>

#include "config/Exports.h"

#include "ByteSwapValue.h"
#include "Runnable.h"
#include "SysInt.h"

namespace sys
{
/*!
 *  Swap bytes in-place.  Note that a complex pixel
 *  is equivalent to two floats so elemSize and numElems
 *  must be adjusted accordingly.
 *
 *  \param [inout] buffer to transform
 *  \param elemSize
 *  \param numElems
 */
coda_oss::span<const coda_oss::byte> CODA_OSS_API byteSwap(coda_oss::span<coda_oss::byte>buffer, size_t elemSize);
void CODA_OSS_API byteSwap(void* buffer, size_t elemSize, size_t numElems);

// If the caller has given us bytes, assume she knows what she's doing; i.e., don't check sizeof(T)
template<typename TByte>
inline void byteSwap_(TByte* buffer, size_t elemSize, size_t numElems)
{
    void* const buffer_ = buffer;
    byteSwap(buffer_, elemSize, numElems);
}
inline void byteSwap(coda_oss::byte* buffer, size_t elemSize, size_t numElems)
{
    return byteSwap_(buffer, elemSize, numElems);
}
inline void byteSwap(byte* buffer, size_t elemSize, size_t numElems)
{
    return byteSwap_(buffer, elemSize, numElems);
}
inline void byteSwap(ubyte* buffer, size_t elemSize, size_t numElems)
{
    return byteSwap_(buffer, elemSize, numElems);
}

namespace details
{
template <typename T>
inline void check_elemSize(size_t elemSize)
{
    static_assert(is_byte_swappable<T>(), "T should not be a 'struct'");
    if (sizeof(T) != elemSize)
    {
        throw std::invalid_argument("'elemSize' != sizeof(T)");
    }
}

// Repackage into a span<T>; the size is 2* because for byte-swapping
// we want to look at this as an array of `T`, not `std::complex<T>`.
template <typename T>
inline auto make_span(coda_oss::span<const std::complex<T>> s)
{
    //static_assert(std::is_floating_point<T>::value, "std::complex<T> should use floating-point");

    const void* const p_ = s.data();
    auto const p = static_cast<const T*>(p_);
    const auto sz = s.size() * 2;  // real and imag
    return sys::make_span(p, sz);
}
template<typename T>
inline auto make_span(coda_oss::span<std::complex<T>> s)
{
    //static_assert(std::is_floating_point<T>::value, "std::complex<T> should use floating-point");

    void* const p_ = s.data();
    auto const p = static_cast<T*>(p_);
    const auto sz = s.size() * 2;  // real and imag
    return sys::make_span(p, sz);
}

}

// Otherwise, we can sanity-check the `elemSize` parameter
template <typename T>
inline void byteSwap(T* buffer, size_t elemSize, size_t numElems)
{
    details::check_elemSize<T>(elemSize);
    void* const buffer_ = buffer;
    byteSwap(buffer_, elemSize, numElems);
}
template <typename T>
inline void byteSwap(std::complex<T>* buffer, size_t elemSize, size_t numElems) // dont't want `T` as `std::complex<...>`
{
    //static_assert(std::is_floating_point<T>::value, "std::complex<T> should use floating-point");

    details::check_elemSize<T>(elemSize);
    void* const buffer_ = buffer;
    byteSwap(buffer_, elemSize, numElems);
}

template <typename T>
inline auto byteSwap(coda_oss::span<T> buffer)
{
    static_assert(!std::is_const<T>::value, "T cannot be 'const'");
    static_assert(details::is_byte_swappable<T>(), "T should not be a 'struct'");
    return byteSwap(as_writable_bytes(buffer), sizeof(T));
}

// Take care of treating std::complex<T> as T[]
template <typename T>
inline auto byteSwap(coda_oss::span<std::complex<T>> buffer)
{
    return byteSwap(details::make_span(buffer));
}

/*!
 *  Swap bytes into output buffer.  Note that a complex pixel
 *  is equivalent to two floats so elemSize and numElems
 *  must be adjusted accordingly.
 *
 *  \param buffer to transform
 *  \param elemSize
 *  \param numElems
 *  \param[out] outputBuffer buffer to write swapped elements to
 */
coda_oss::span<const coda_oss::byte> CODA_OSS_API byteSwap(coda_oss::span<const coda_oss::byte> buffer,
         size_t elemSize, coda_oss::span<coda_oss::byte> outputBuffer);
void CODA_OSS_API byteSwap(const void* buffer, size_t elemSize, size_t numElems, void* outputBuffer);

// If the caller has given us bytes, assume she knows what she's doing; i.e., don't check sizeof(T)
template<typename TByte, typename U>
inline void byteSwap_(const TByte* buffer, size_t elemSize, size_t numElems, U* outputBuffer)
{
    const void* const buffer_ = buffer;
    void* const outputBuffer_ = outputBuffer;
    byteSwap(buffer_, elemSize, numElems, outputBuffer_);
}
template<typename U>
inline void byteSwap(const coda_oss::byte* buffer, size_t elemSize, size_t numElems, U* outputBuffer)
{
    byteSwap_(buffer, elemSize, numElems, outputBuffer);
}
template <typename U>
inline void byteSwap(const byte* buffer, size_t elemSize, size_t numElems, U* outputBuffer)
{
    byteSwap_(buffer, elemSize, numElems, outputBuffer);
}
template <typename U>
inline void byteSwap(const ubyte* buffer, size_t elemSize, size_t numElems, U* outputBuffer)
{
    byteSwap_(buffer, elemSize, numElems, outputBuffer);
}

// Otherwise, we can sanity-check the `elemSize` parameter
template <typename T, typename U>
inline void byteSwap(const T* buffer, size_t elemSize, size_t numElems, U* outputBuffer)
{
    details::check_elemSize<T>(elemSize); 
    const void* const buffer_ = buffer;
    void* const outputBuffer_ = outputBuffer;
    byteSwap(buffer_, elemSize, numElems, outputBuffer_);
}
template <typename T, typename U>
inline void byteSwap(const std::complex<T>* buffer, size_t elemSize, size_t numElems, U* outputBuffer) // dont't want `T` as `std::complex<...>`
{
    //static_assert(std::is_floating_point<T>::value, "std::complex<T> should use floating-point");

    details::check_elemSize<T>(elemSize);
    const void* const buffer_ = buffer;
    void* const outputBuffer_ = outputBuffer;
    byteSwap(buffer_, elemSize, numElems, outputBuffer_);
}

template <typename T>
inline auto byteSwap(coda_oss::span<const T> buffer, coda_oss::span<coda_oss::byte> outputBuffer)
{
    static_assert(details::is_byte_swappable<T>(), "T should not be a 'struct'");
    return byteSwap(as_bytes(buffer), sizeof(T), outputBuffer);
}
// Take care of treating std::complex<T> as T[]
template <typename T>
inline auto byteSwap(coda_oss::span<const std::complex<T>> buffer, coda_oss::span<coda_oss::byte> outputBuffer)
{
    return byteSwap(details::make_span(buffer), outputBuffer);
}

template <typename T>
inline auto byteSwap(coda_oss::span<const T> buffer)
{
    std::vector<coda_oss::byte> retval(buffer.size_bytes());
    std::ignore = byteSwap(buffer, make_span(retval));
    return retval;
}
// Take care of treating std::complex<T> as T[]
template <typename T>
inline auto byteSwap(coda_oss::span<const std::complex<T>> buffer)
{
    return byteSwap(details::make_span(buffer));
}

// With buffer byte-swap now in place, we can safely byte-swap std::complex<T>.
// This signature is from ByteSwapValue.h
template <typename T>
inline auto byteSwapValue(std::complex<T> z)
{
    //static_assert(std::is_floating_point<T>::value, "std::complex<T> should use floating-point");

    // C++ mandates that `std::complex<T>` be the same as `T cx[2]`; that is
    // the structure is contiguous. https://en.cppreference.com/w/cpp/numeric/complex
    const auto& z_ = reinterpret_cast<T(&)[2]>(z);    
    return byteSwap(make_span(z_));
}
template<typename T>
inline auto byteSwap(std::complex<T> val)
{
    const auto bytes = byteSwapValue(val);
    assert(bytes.size() == sizeof(val));

    const void* const pBytes = bytes.data();
    auto const pRetVal = static_cast<const std::complex<T>*>(pBytes);
    return *pRetVal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ByteSwapRunnable final : public sys::Runnable
{
    ByteSwapRunnable(void* buffer, size_t elemSize, size_t startElement, size_t numElements) noexcept :
        mBuffer(static_cast<coda_oss::byte*>(buffer) + startElement * elemSize),
        mElemSize(elemSize), mNumElements(numElements)
    {
    }
    void run() override
    {
        byteSwap(mBuffer, mElemSize, mNumElements);
    }

    virtual ~ByteSwapRunnable() = default;
    ByteSwapRunnable(const ByteSwapRunnable&) = delete;
    ByteSwapRunnable& operator=(const ByteSwapRunnable&) = delete;
    ByteSwapRunnable(ByteSwapRunnable&&) = delete;
    ByteSwapRunnable& operator=(ByteSwapRunnable&&) = delete;

private:
    void* const mBuffer;
    const size_t mElemSize;
    const size_t mNumElements;
};

struct ByteSwapCopyRunnable final : public sys::Runnable
{
    ByteSwapCopyRunnable(const void* buffer, size_t elemSize, size_t startElement, size_t numElements, void* outputBuffer) noexcept :
        mBuffer(static_cast<const coda_oss::byte*>(buffer) + startElement * elemSize),
        mElemSize(elemSize), mNumElements(numElements),
         mOutputBuffer(static_cast<coda_oss::byte*>(outputBuffer) + startElement * elemSize)
    {
    }
    void run() override
    {
        byteSwap(mBuffer, mElemSize, mNumElements, mOutputBuffer);
    }

    virtual ~ByteSwapCopyRunnable() = default;
    ByteSwapCopyRunnable(const ByteSwapCopyRunnable&) = delete;
    ByteSwapCopyRunnable& operator=(const ByteSwapCopyRunnable&) = delete;
    ByteSwapCopyRunnable(ByteSwapCopyRunnable&&) = delete;
    ByteSwapCopyRunnable& operator=(ByteSwapCopyRunnable&&) = delete;

private:
    const void* const mBuffer;
    const size_t mElemSize;
    const size_t mNumElements;
    void* const mOutputBuffer;
};

}
#endif  // CODA_OSS_sys_ByteSwap_h_INCLUDED_
