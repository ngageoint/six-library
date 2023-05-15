/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * sys-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.

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

#include "TestCase.h"

#include <stdint.h>

#include <array>
#include <std/bit> // std::endian
#include <std/cstddef> // std::byte
#include <std/span>

#include <sys/Conf.h>

TEST_CASE(testEndianness)
{
    /*const*/ auto native = std::endian::native; // "const" causes "conditional expression is constant."

    if (native == std::endian::big) { }
    else if (native == std::endian::little) { }
    else
    {
        TEST_FAIL_MSG("Mixed-endian not supported!");
    }

    const bool isBigEndianSystem = sys::isBigEndianSystem();

    if (native == std::endian::big)
    {
        TEST_ASSERT(isBigEndianSystem);
    }
    else
    {
        TEST_ASSERT(!isBigEndianSystem);    
    }
    if (native == std::endian::little)
    {
        TEST_ASSERT(!isBigEndianSystem);
    }
    else
    {
        TEST_ASSERT(isBigEndianSystem);
    }


    if (isBigEndianSystem)
    {
        TEST_ASSERT(native == std::endian::big);
    }
    else
    {
        TEST_ASSERT(native == std::endian::little);    
    }
}

TEST_CASE(testByteSwap)
{
    ::srand(334);

    static const size_t NUM_PIXELS = 10000;
    std::vector<sys::Uint64_T> origValues(NUM_PIXELS);
    for (size_t ii = 0; ii < NUM_PIXELS; ++ii)
    {
        const auto value = static_cast<float>(::rand()) / RAND_MAX *
                std::numeric_limits<sys::Uint64_T>::max();
        origValues[ii] = static_cast<sys::Uint64_T>(value);
    }

    // Byte swap the old-fashioned way
    std::vector<sys::Uint64_T> values1(origValues);
    sys::byteSwap(&values1[0], sizeof(sys::Uint64_T), NUM_PIXELS);


    // Byte swap into output buffer
    const std::vector<sys::Uint64_T> values2(origValues);
    std::vector<sys::Uint64_T> swappedValues2(values2.size());
    sys::byteSwap(&values2[0], sizeof(sys::Uint64_T), NUM_PIXELS,
                  &swappedValues2[0]);

    // Everything should match
    for (size_t ii = 0; ii < NUM_PIXELS; ++ii)
    {
        TEST_ASSERT_EQ(values1[ii], swappedValues2[ii]);
    }
}

#define CODA_OSS_define_byte(v) constexpr static std::byte v = static_cast<std::byte>(0 ## v)
CODA_OSS_define_byte(x00);
CODA_OSS_define_byte(x11);
CODA_OSS_define_byte(x22);
CODA_OSS_define_byte(x33);
CODA_OSS_define_byte(x44);
CODA_OSS_define_byte(x55);
CODA_OSS_define_byte(x66);
CODA_OSS_define_byte(x77);
CODA_OSS_define_byte(x88);
CODA_OSS_define_byte(x99);
CODA_OSS_define_byte(xAA);
CODA_OSS_define_byte(xBB);
CODA_OSS_define_byte(xCC);
CODA_OSS_define_byte(xDD);
CODA_OSS_define_byte(xEE);
CODA_OSS_define_byte(xFF);
#undef CODA_OSS_define_byte

static constexpr std::byte two_bytes[]{x00, xFF};
static constexpr std::byte four_bytes[]{x00, x11, xEE, xFF};
static constexpr std::byte eight_bytes[]{x00, x11, x22, x33, xCC, xDD, xEE, xFF};
static constexpr std::byte sixteen_bytes[]{x00, x11, x22, x33, x44, x55, x66, x77, x88, x99, xAA, xBB, xCC, xDD, xEE, xFF};

TEST_CASE(testByteSwapUInt16)
{
    const void* pBytes = &(two_bytes[0]);
    auto pUInt16 = static_cast<const uint16_t*>(pBytes);
    auto swap = sys::byteSwap(*pUInt16);
    TEST_ASSERT_NOT_EQ(*pUInt16, swap);
    const void* pResult_ = &swap;
    auto pResultBytes = static_cast<const std::byte*>(pResult_);
    TEST_ASSERT(pResultBytes[0] == two_bytes[1]);
    TEST_ASSERT(pResultBytes[1] == two_bytes[0]);
    swap = sys::byteSwap(swap);  // swap back
    TEST_ASSERT_EQ(*pUInt16, swap);

    // array swap from input to output
    pBytes = &(sixteen_bytes[0]);
    const auto buffer = static_cast<const uint16_t*>(pBytes);
    std::array<uint16_t, 8> outputBuffer;
    sys::byteSwap(buffer, sizeof(uint16_t), outputBuffer.size(), outputBuffer.data());
    for (auto&& v : outputBuffer)
    {
        swap = sys::byteSwap(v);
        TEST_ASSERT_NOT_EQ(v, swap);
        swap = sys::byteSwap(swap);  // swap back
        TEST_ASSERT_EQ(v, swap);
    }

    // in-place swap
    sys::byteSwap(outputBuffer.data(), sizeof(uint16_t), outputBuffer.size());
    pBytes = outputBuffer.data();
    pResultBytes = static_cast<const std::byte*>(pBytes);
    for (size_t i=0; i<16; i++)
    {
        TEST_ASSERT(pResultBytes[i] == sixteen_bytes[i]);
    }
}

TEST_CASE(testByteSwapUInt32)
{
    const void* pBytes = &(four_bytes[0]);
    auto pUInt32 = static_cast<const uint32_t*>(pBytes);
    auto swap = sys::byteSwap(*pUInt32);
    TEST_ASSERT_NOT_EQ(*pUInt32, swap);
    const void* pResult_ = &swap;
    auto pResultBytes = static_cast<const std::byte*>(pResult_);
    TEST_ASSERT(pResultBytes[0] == four_bytes[3]);
    TEST_ASSERT(pResultBytes[1] == four_bytes[2]);
    TEST_ASSERT(pResultBytes[2] == four_bytes[1]);
    TEST_ASSERT(pResultBytes[3] == four_bytes[0]);
    swap = sys::byteSwap(swap);  // swap back
    TEST_ASSERT_EQ(*pUInt32, swap);

    // array swap from input to output
    pBytes = &(sixteen_bytes[0]);
    const auto buffer = static_cast<const uint32_t*>(pBytes);
    std::array<uint32_t, 4> outputBuffer;
    sys::byteSwap(buffer, sizeof(uint32_t), outputBuffer.size(), outputBuffer.data());
    for (auto&& v : outputBuffer)
    {
        swap = sys::byteSwap(v);
        TEST_ASSERT_NOT_EQ(v, swap);
        swap = sys::byteSwap(swap);  // swap back
        TEST_ASSERT_EQ(v, swap);
    }

    // in-place swap
    sys::byteSwap(outputBuffer.data(), sizeof(uint32_t), outputBuffer.size());
    pBytes = outputBuffer.data();
    pResultBytes = static_cast<const std::byte*>(pBytes);
    for (size_t i=0; i<16; i++)
    {
        TEST_ASSERT(pResultBytes[i] == sixteen_bytes[i]);
    }
}

TEST_CASE(testByteSwapUInt64)
{
    const void* pBytes = &(eight_bytes[0]);
    auto pUInt64 = static_cast<const uint64_t*>(pBytes);
    auto swap = sys::byteSwap(*pUInt64);
    TEST_ASSERT_NOT_EQ(*pUInt64, swap);
    const void* pResult_ = &swap;
    auto pResultBytes = static_cast<const std::byte*>(pResult_);
    TEST_ASSERT(pResultBytes[0] == eight_bytes[7]);
    TEST_ASSERT(pResultBytes[1] == eight_bytes[6]);
    TEST_ASSERT(pResultBytes[2] == eight_bytes[5]);
    TEST_ASSERT(pResultBytes[3] == eight_bytes[4]);
    TEST_ASSERT(pResultBytes[4] == eight_bytes[3]);
    TEST_ASSERT(pResultBytes[5] == eight_bytes[2]);
    TEST_ASSERT(pResultBytes[6] == eight_bytes[1]);
    TEST_ASSERT(pResultBytes[7] == eight_bytes[0]);
    swap = sys::byteSwap(swap);  // swap back
    TEST_ASSERT_EQ(*pUInt64, swap);

    // array swap from input to output
    pBytes = &(sixteen_bytes[0]);
    const auto buffer = static_cast<const uint64_t*>(pBytes);
    std::array<uint64_t, 2> outputBuffer;
    sys::byteSwap(buffer, sizeof(uint64_t), outputBuffer.size(), outputBuffer.data());
    for (auto&& v : outputBuffer)
    {
        swap = sys::byteSwap(v);
        TEST_ASSERT_NOT_EQ(v, swap);
        swap = sys::byteSwap(swap);  // swap back
        TEST_ASSERT_EQ(v, swap);
    }

    // in-place swap
    sys::byteSwap(outputBuffer.data(), sizeof(uint64_t), outputBuffer.size());
    pBytes = outputBuffer.data();
    pResultBytes = static_cast<const std::byte*>(pBytes);
    for (size_t i=0; i<16; i++)
    {
        TEST_ASSERT(pResultBytes[i] == sixteen_bytes[i]);
    }
}

TEST_CASE(testByteSwapFloat)
{
    static_assert(sizeof(float) == sizeof(uint32_t), "sizeof(float) != sizeof(uint32_t)");

    const float v = 3.141592654f;
    const void* pVoid = &v;
    auto pFloat = static_cast<const float*>(pVoid);
    auto swap = sys::byteSwap(*pFloat);
    // The swapped bits could be nonsense as a `float`, so comparing might not work
    //TEST_ASSERT_NOT_EQ(*pFloat, swap);
    const void* pResult_ = &swap;
    auto pResultBytes = static_cast<const std::byte*>(pResult_);
    auto pValueBytes = static_cast<const std::byte*>(pVoid);
    TEST_ASSERT(pResultBytes[0] == pValueBytes[3]);
    TEST_ASSERT(pResultBytes[1] == pValueBytes[2]);
    TEST_ASSERT(pResultBytes[2] == pValueBytes[1]);
    TEST_ASSERT(pResultBytes[3] == pValueBytes[0]);
    swap = sys::byteSwap(swap);  // swap back
    TEST_ASSERT_EQ(*pFloat, swap);

    // array swap from input to output
    const float values[] = {1.0f, 2.0f, 3.0f, 4.0f};
    pVoid = &(values[0]);
    const auto buffer = static_cast<const float*>(pVoid);
    std::array<float, 4> outputBuffer;
    sys::byteSwap(buffer, sizeof(float), outputBuffer.size(), outputBuffer.data());
    for (size_t i = 0; i<outputBuffer.size(); i++)
    {
        // can't test swapped bytes against anything; might not be a valid `float`
        swap = sys::byteSwap(outputBuffer[i]);
        TEST_ASSERT_EQ(values[i], swap);
    }

    // in-place swap
    sys::byteSwap(outputBuffer.data(), sizeof(float), outputBuffer.size());
    for (size_t i = 0; i < outputBuffer.size(); i++)
    {
        TEST_ASSERT_EQ(values[i], outputBuffer[i]);
    }
}

TEST_CASE(testByteSwapDouble)
{
    using value_type = double;
    static_assert(sizeof(value_type) == sizeof(uint64_t), "sizeof(value_type) != sizeof(uint64_t)");

    const value_type v = 3.141592654;
    const void* pVoid = &v;
    auto pDouble = static_cast<const value_type*>(pVoid);
    auto swap = sys::byteSwap(*pDouble);
    // The swapped bits could be nonsense as a `double`, so comparing might not work
    //TEST_ASSERT_NOT_EQ(*pDouble, swap);
    const void* pResult_ = &swap;
    auto pResultBytes = static_cast<const std::byte*>(pResult_);
    auto pValueBytes = static_cast<const std::byte*>(pVoid);
    TEST_ASSERT(pResultBytes[0] == pValueBytes[7]);
    TEST_ASSERT(pResultBytes[1] == pValueBytes[6]);
    TEST_ASSERT(pResultBytes[2] == pValueBytes[5]);
    TEST_ASSERT(pResultBytes[3] == pValueBytes[4]);
    TEST_ASSERT(pResultBytes[4] == pValueBytes[3]);
    TEST_ASSERT(pResultBytes[5] == pValueBytes[2]);
    TEST_ASSERT(pResultBytes[6] == pValueBytes[1]);
    TEST_ASSERT(pResultBytes[7] == pValueBytes[0]);
    swap = sys::byteSwap(swap);  // swap back
    TEST_ASSERT_EQ(*pDouble, swap);

    // array swap from input to output
    const std::vector<value_type> values = {1.0, 2.0};
    const std::span<const value_type> buffer(values.data(), values.size());
    std::array<value_type, 2> outputBuffer;
    const std::span<value_type> outputBuffer_(outputBuffer.data(), outputBuffer.size());
    sys::byteSwap(buffer, outputBuffer_);
    for (size_t i = 0; i < outputBuffer_.size(); i++)
    {
        // can't test swapped bytes against anything; might not be a valid `double`
        swap = sys::byteSwap(outputBuffer_[i]);
        TEST_ASSERT_EQ(values[i], swap);
    }

    // in-place swap
    sys::byteSwap(outputBuffer_);
    for (size_t i = 0; i < outputBuffer_.size(); i++)
    {
        TEST_ASSERT_EQ(values[i], outputBuffer_[i]);
    }
}

TEST_CASE(testByteSwapCxFloat)
{
    using value_type = std::complex<float>;
    static_assert(sizeof(value_type) == sizeof(uint64_t), "sizeof(value_type) != sizeof(uint64_t)");

    const value_type v = {1.1f, -9.9f};
    const void* pVoid = &v;
    auto pCxFloat = static_cast<const value_type*>(pVoid);
    auto swap = sys::byteSwap(*pCxFloat);
    // The swapped bits could be nonsense as a `double`, so comparing might not work
    //TEST_ASSERT_NOT_EQ(*pDouble, swap);
    const void* pResult_ = &swap;
    auto pResultBytes = static_cast<const std::byte*>(pResult_);
    auto pValueBytes = static_cast<const std::byte*>(pVoid);
    TEST_ASSERT(pResultBytes[0] == pValueBytes[3]);
    TEST_ASSERT(pResultBytes[1] == pValueBytes[2]);
    TEST_ASSERT(pResultBytes[2] == pValueBytes[1]);
    TEST_ASSERT(pResultBytes[3] == pValueBytes[0]);
    TEST_ASSERT(pResultBytes[4] == pValueBytes[7]);
    TEST_ASSERT(pResultBytes[5] == pValueBytes[6]);
    TEST_ASSERT(pResultBytes[6] == pValueBytes[5]);
    TEST_ASSERT(pResultBytes[7] == pValueBytes[4]);
    swap = sys::byteSwap(swap);  // swap back
    TEST_ASSERT_EQ(*pCxFloat, swap);

    // array swap from input to output
    const std::vector<value_type> values = {{1.1f, -9.9f}, {-22.22f, 88.88f}};
    const std::span<const value_type> buffer(values.data(), values.size());
    std::array<value_type, 2> outputBuffer;
    const std::span<value_type> outputBuffer_(outputBuffer.data(), outputBuffer.size());
    sys::byteSwap(buffer, outputBuffer_);
    for (size_t i = 0; i<outputBuffer_.size(); i++)
    {
        // can't test swapped bytes against anything; might not be a valid `double`
        swap = sys::byteSwap(outputBuffer_[i]);
        TEST_ASSERT_EQ(values[i], swap);
    }

    // in-place swap
    sys::byteSwap(outputBuffer_);
    for (size_t i = 0; i < outputBuffer_.size(); i++)
    {
        TEST_ASSERT_EQ(values[i], outputBuffer_[i]);
    }
}

TEST_MAIN(
    TEST_CHECK(testEndianness);
    TEST_CHECK(testByteSwap);
    TEST_CHECK(testByteSwapUInt16);
    TEST_CHECK(testByteSwapUInt32);
    TEST_CHECK(testByteSwapUInt64);
    TEST_CHECK(testByteSwapFloat);
    TEST_CHECK(testByteSwapDouble);
    TEST_CHECK(testByteSwapCxFloat);
    )
     