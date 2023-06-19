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
#include "sys/Conf.h"

#include <assert.h>

#include <stdexcept>
#include <type_traits>
#include <algorithm>
#include <tuple>
#include "coda_oss/bit.h"
#include "coda_oss/cstddef.h"
#include "coda_oss/span.h"

#include "sys/Span.h"

// https://en.cppreference.com/w/cpp/types/endian
using endian = coda_oss::endian;
template <endian endianness>
inline bool is_big_endian_()
{
    throw std::logic_error("Mixed-endian not supported.");
}
template<>
inline constexpr bool is_big_endian_<endian::big>()
{
    return true;
}
template <>
inline constexpr bool is_big_endian_<endian::little>()
{
    return false;
}
constexpr inline bool is_big_endian()
{
    return is_big_endian_<endian::native>();
}

// Want to explicitly test against both endian::bit and endian::little; i.e.,
// because of "mixed" endianness, little may not the same as !big
template <endian endianness>
inline bool is_little_endian_()
{
    throw std::logic_error("Mixed-endian not supported.");
}
template <>
inline constexpr bool is_little_endian_<endian::big>()
{
    return false;
}
template <>
inline constexpr bool is_little_endian_<endian::little>()
{
    return true;
}
constexpr inline bool is_little_endian()
{
    return is_little_endian_<endian::native>();
}

constexpr inline bool is_big_or_little_endian()
{
    return is_big_endian() || is_little_endian();
}

inline bool testIsBigEndianSystem()
{
    // This is an endian test
    int intVal = 1;
    unsigned char* endianTest = (unsigned char*)&intVal;
    return endianTest[0] != 1;
}
inline auto isBigEndianSystem_()
{
    static_assert(is_big_or_little_endian(), "Mixed-endian not supported.");
    const auto retval = testIsBigEndianSystem();
    if (retval != is_big_endian())
    {
        throw std::logic_error("endian values don't agree!");
    }
    return retval;
}
inline auto isLittleEndianSystem_()
{
    static_assert(is_big_or_little_endian(), "Mixed-endian not supported.");
    const auto retval = !testIsBigEndianSystem();
    if (retval != is_little_endian())
    {
        throw std::logic_error("endian values don't agree!");
    }
    return retval;
}
bool sys::isBigEndianSystem()
{
    auto const retval = isBigEndianSystem_();
    assert(!retval == isLittleEndianSystem_());
    return retval;
}
bool sys::isLittleEndianSystem()
{
    auto const retval = isLittleEndianSystem_();
    assert(!retval == isBigEndianSystem_());
    return retval;
}

   /*!
 *  Swap bytes in-place.  Note that a complex pixel
 *  is equivalent to two floats so elemSize and numElems
 *  must be adjusted accordingly.
 *
 *  \param [inout] buffer to transform
 *  \param elemSize
 *  \param numElems
 */
template <typename TUInt>
inline auto byteSwap(coda_oss::span<TUInt> buffer)
{
    static_assert(std::is_unsigned<TUInt>::value, "TUInt must be 'unsigned'");
    for (auto& v : buffer)
    {
        v = sys::byteSwap(v);
    }
    return sys::as_bytes(buffer);
}
template <typename TUInt>
inline auto byteSwap_n_(coda_oss::span<coda_oss::byte> buffer_)
{
    const auto buffer = sys::make_span<TUInt>(buffer_.data(), buffer_.size_bytes() / sizeof(TUInt));
    assert(buffer.size_bytes() == buffer_.size_bytes());
    return byteSwap(buffer);
}
template <typename TUInt>
inline auto byteSwap_n(coda_oss::span<coda_oss::byte> buffer, size_t elemSize)
{
    if (sizeof(TUInt) != elemSize)
    {
        throw std::invalid_argument("'elemSize' != sizeof(TUInt)");
    }
    return byteSwap_n_<TUInt>(buffer);
}
static coda_oss::span<const coda_oss::byte> byteSwap(coda_oss::span<coda_oss::byte> buffer, size_t elemSize, size_t numElems)
{
    switch (elemSize)
    {
        case sizeof(uint16_t): return byteSwap_n<uint16_t>(buffer, elemSize);
        case sizeof(uint32_t): return byteSwap_n<uint32_t>(buffer, elemSize);
        case sizeof(uint64_t): return byteSwap_n<uint64_t>(buffer, elemSize);
        default: break;
    }

    auto const bufferPtr = buffer.data();
    const auto half = elemSize >> 1;
    size_t offset = 0, innerOff = 0, innerSwap = 0;
    for (size_t i = 0; i < numElems; ++i, offset += elemSize)
    {
        for (size_t j = 0; j < half; ++j)
        {
            innerOff = offset + j;
            innerSwap = offset + elemSize - 1 - j;

            std::swap(bufferPtr[innerOff], bufferPtr[innerSwap]);
        }
    }

    return sys::make_const_span(buffer);
}
void sys::byteSwap(void* buffer_, size_t elemSize, size_t numElems)
{
    if ((buffer_ == nullptr) || (elemSize < 2) || (numElems == 0))
        return;

    auto const pBytes = static_cast<coda_oss::byte*>(buffer_);
    const coda_oss::span<coda_oss::byte> buffer(pBytes, elemSize * numElems);
    std::ignore = ::byteSwap(buffer, elemSize, numElems);
}
coda_oss::span<const coda_oss::byte> sys::byteSwap(coda_oss::span<coda_oss::byte> buffer, size_t elemSize)
{
    if ((buffer.empty()) || (elemSize < 2))
        return sys::make_const_span(buffer);

    size_t const numElems = buffer.size() / elemSize;
    if ((numElems * elemSize) != buffer.size())
    {
        throw std::invalid_argument("'buffer' is not a multiple of 'elemSize'");
    }

    return ::byteSwap(buffer, elemSize, numElems);
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
template <typename TUInt>
inline auto byteSwap_n_(coda_oss::span<const TUInt> buffer, coda_oss::span<coda_oss::byte> outputBuffer_)
{
    static_assert(std::is_unsigned<TUInt>::value, "TUInt must be 'unsigned'");

    assert(buffer.size_bytes() == outputBuffer_.size());
    void* pOutputBuffer = outputBuffer_.data();
    const auto outputBuffer = sys::make_span<TUInt>(pOutputBuffer, buffer.size());
    assert(buffer.size_bytes() == outputBuffer.size_bytes());

    const auto byteSwap = [](const auto& v) { return sys::byteSwap(v); };
    std::transform(buffer.begin(), buffer.end(), outputBuffer.begin(), byteSwap);

    return sys::as_bytes(outputBuffer);
}
template <typename TUInt>
inline auto byteSwap_n(coda_oss::span<const coda_oss::byte> buffer_, size_t elemSize, coda_oss::span<coda_oss::byte> outputBuffer)
{
    if (sizeof(TUInt) != elemSize)
    {
        throw std::invalid_argument("'elemSize' != sizeof(TUInt)");
    }

    const auto buffer = sys::make_span<TUInt>(buffer_.data(), buffer_.size_bytes() / sizeof(TUInt));
    assert(buffer.size_bytes() == buffer_.size_bytes());
    return byteSwap_n_<TUInt>(buffer,  outputBuffer);
}

static auto byteSwap(coda_oss::span<const coda_oss::byte> buffer,
                      size_t elemSize, size_t numElems,
                      coda_oss::span<coda_oss::byte> outputBuffer)
{
    auto const bufferPtr = buffer.data();
    auto const outputBufferPtr = outputBuffer.data();
    switch (elemSize)
    {
        case 1:
        {
            std::ignore = memcpy(outputBufferPtr, bufferPtr, elemSize * numElems);
            return sys::make_const_span(outputBuffer);
        }
        case 2: return byteSwap_n<uint16_t>(buffer, elemSize, outputBuffer);
        case 4: return byteSwap_n<uint32_t>(buffer, elemSize, outputBuffer);
        case 8: return byteSwap_n<uint64_t>(buffer, elemSize, outputBuffer);
        default: break;
    }

    const auto half = elemSize >> 1;
    size_t offset = 0;
    for (size_t ii = 0; ii < numElems; ++ii, offset += elemSize)
    {
        for (size_t jj = 0; jj < half; ++jj)
        {
            const size_t innerOff = offset + jj;
            const size_t innerSwap = offset + elemSize - 1 - jj;

            outputBufferPtr[innerOff] = bufferPtr[innerSwap];
            outputBufferPtr[innerSwap] = bufferPtr[innerOff];
        }
    }

    return sys::make_const_span(outputBuffer);
}

void sys::byteSwap(const void* buffer_, size_t elemSize, size_t numElems, void* outputBuffer_)
{
    if ((numElems == 0) || (buffer_ == nullptr) || (outputBuffer_ == nullptr))
    {
        return;
    }

    auto const pBytes = static_cast<const coda_oss::byte*>(buffer_);
    const coda_oss::span<const coda_oss::byte> buffer(pBytes, elemSize * numElems);

    auto const pOutputBytes = static_cast<coda_oss::byte*>(outputBuffer_);
    const coda_oss::span<coda_oss::byte> outputBuffer(pOutputBytes, elemSize * numElems);

    std::ignore = ::byteSwap(buffer, elemSize, numElems, outputBuffer);
}
coda_oss::span<const coda_oss::byte> sys::byteSwap(coda_oss::span<const coda_oss::byte> buffer,
         size_t elemSize, coda_oss::span<coda_oss::byte> outputBuffer)
{
    if ((buffer.empty()) || (outputBuffer.empty()))
    {
        return sys::make_const_span(outputBuffer);
    }

    size_t const numElems = buffer.size() / elemSize;
    if ((numElems * elemSize) != buffer.size())
    {
        throw std::invalid_argument("'buffer' is not a multiple of 'elemSize'");
    }
    if (buffer.size() != outputBuffer.size())
    {
        const auto s = "'buffer' and 'outputBuffer' are different sizes: " +
                std::to_string(buffer.size()) + " != " + std::to_string(outputBuffer.size());
        throw std::invalid_argument(s);
    }

    return ::byteSwap(buffer, elemSize, numElems, outputBuffer);
 }

// byte-swap a single value
coda_oss::span<const coda_oss::byte> sys::byteSwap(
        coda_oss::span<const coda_oss::byte> inPtr,
        coda_oss::span<coda_oss::byte> outPtr)
{
    if (inPtr.size() != outPtr.size())
    {
        throw std::invalid_argument("'size of byte buffers must match");
    }

    const auto elemSize = inPtr.size();
    switch (elemSize)
    {
    case sizeof(uint8_t): return details::swapUIntBytes<uint8_t>(inPtr, outPtr, std::nothrow);
    case sizeof(uint16_t): return details::swapUIntBytes<uint16_t>(inPtr, outPtr, std::nothrow);
    case sizeof(uint32_t): return details::swapUIntBytes<uint32_t>(inPtr, outPtr, std::nothrow);
    case sizeof(uint64_t): return details::swapUIntBytes<uint64_t>(inPtr, outPtr, std::nothrow);
    default: break;
    }

    for (size_t ii = 0, jj = elemSize - 1; ii < jj; ++ii, --jj)
    {
        outPtr[ii] = inPtr[jj];
        outPtr[jj] = inPtr[ii];
    }

    // Give the raw byte-swapped bytes back to the caller for easy serialization
    return make_const_span(outPtr);
}
