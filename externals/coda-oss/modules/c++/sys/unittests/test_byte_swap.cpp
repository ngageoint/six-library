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

#include <array>
#include <vector>
#include <std/bit> // std::endian
#include <std/cstddef>
#include <std/span>

#include <sys/Conf.h>
#include <sys/Span.h>

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

static std::vector<uint64_t> make_origValues(size_t NUM_PIXELS)
{
    ::srand(334);

    std::vector<uint64_t> retval(NUM_PIXELS);
    for (size_t ii = 0; ii < NUM_PIXELS; ++ii)
    {
        const auto value = static_cast<float>(::rand()) / RAND_MAX *
                std::numeric_limits<uint64_t>::max();
        retval[ii] = static_cast<uint64_t>(value);
    }
    return retval;
}

TEST_CASE(testByteSwap)
{
    constexpr size_t NUM_PIXELS = 10000;
    const auto origValues = make_origValues(NUM_PIXELS);

    // Byte swap the old-fashioned way
    auto values1(origValues);
    sys::byteSwap(values1.data(), sizeof(uint64_t), NUM_PIXELS);

    // Byte swap into output buffer
    std::vector<uint64_t> swappedValues2(origValues.size());
    sys::byteSwap(origValues.data(), sizeof(uint64_t), NUM_PIXELS, swappedValues2.data());

    // Everything should match
    for (size_t ii = 0; ii < NUM_PIXELS; ++ii)
    {
        TEST_ASSERT_EQ(values1[ii], swappedValues2[ii]);
    }
}

// 0xnn is an `int` which can't be used to initialize std::byte w/o a cast
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

template<typename TUInt>
static void testByteSwapValues_(const std::string& testName, const void* pBytes)
{
    auto pUInt = static_cast<const TUInt*>(pBytes);
    auto swap = sys::byteSwap(*pUInt);
    TEST_ASSERT_NOT_EQ(*pUInt, swap);

    const void* pResult_ = &swap;
    auto const pResultBytes = static_cast<const std::byte*>(pResult_);
    auto const pValueBytes = static_cast<const std::byte*>(pBytes);
    for (size_t i = 0, j = sizeof(TUInt); i < sizeof(TUInt) && j > 0; i++, j--)
    {
        TEST_ASSERT(pResultBytes[i] == pValueBytes[j-1]);
    }

    swap = sys::byteSwap(swap);  // swap back
    TEST_ASSERT_EQ(*pUInt, swap);

    // swap as an "array" of one value
    sys::byteSwap(pUInt, sizeof(TUInt), 1, &swap);
    TEST_ASSERT_NOT_EQ(*pUInt, swap);
    sys::byteSwap(&swap, sizeof(TUInt), 1); // swap back
    TEST_ASSERT_EQ(*pUInt, swap);

    const auto resultBytes = sys::swapBytes(*pUInt);
    TEST_ASSERT_EQ(resultBytes.size(), sizeof(TUInt));
    for (size_t i = 0, j = sizeof(TUInt); i < sizeof(TUInt) && j > 0; i++, j--)
    {
        TEST_ASSERT(resultBytes[i] == pValueBytes[j - 1]);
    }
}
TEST_CASE(testByteSwapValues)
{
    testByteSwapValues_<uint16_t>(testName, two_bytes);
    testByteSwapValues_<uint32_t>(testName, four_bytes);
    testByteSwapValues_<uint64_t>(testName, eight_bytes);
}

TEST_CASE(testByteSwap12)
{
    // test a goofy element size
    static constexpr std::byte twelve_bytes[]{
        x00, x11, x22, x33, x44, x55,
        x99, xAA, xBB, xDD, xEE, xFF};
    const auto pValueBytes = &(twelve_bytes[0]);

    std::vector<std::byte> swappedValues(12);
    auto pResultBytes = sys::make_span(swappedValues);

    auto elemSize = 12;
    auto numElements = swappedValues.size() / elemSize;
    sys::byteSwap(twelve_bytes, elemSize, numElements, pResultBytes.data());
    TEST_ASSERT(pResultBytes[0] == pValueBytes[11]);
    TEST_ASSERT(pResultBytes[1] == pValueBytes[10]);
    TEST_ASSERT(pResultBytes[2] == pValueBytes[9]);
    TEST_ASSERT(pResultBytes[3] == pValueBytes[8]);
    TEST_ASSERT(pResultBytes[4] == pValueBytes[7]);
    TEST_ASSERT(pResultBytes[5] == pValueBytes[6]);
    TEST_ASSERT(pResultBytes[6] == pValueBytes[5]);
    TEST_ASSERT(pResultBytes[7] == pValueBytes[4]);
    TEST_ASSERT(pResultBytes[8] == pValueBytes[3]);
    TEST_ASSERT(pResultBytes[9] == pValueBytes[2]);
    TEST_ASSERT(pResultBytes[10] == pValueBytes[1]);
    TEST_ASSERT(pResultBytes[11] == pValueBytes[0]);

    // swap as a SINGLE 12-byte value
    const auto result = sys::details::swapBytes<12>(twelve_bytes, pResultBytes);
    TEST_ASSERT(result[0] == pValueBytes[11]);
    TEST_ASSERT(result[1] == pValueBytes[10]);
    TEST_ASSERT(result[2] == pValueBytes[9]);
    TEST_ASSERT(result[3] == pValueBytes[8]);
    TEST_ASSERT(result[4] == pValueBytes[7]);
    TEST_ASSERT(result[5] == pValueBytes[6]);
    TEST_ASSERT(result[6] == pValueBytes[5]);
    TEST_ASSERT(result[7] == pValueBytes[4]);
    TEST_ASSERT(result[8] == pValueBytes[3]);
    TEST_ASSERT(result[9] == pValueBytes[2]);
    TEST_ASSERT(result[10] == pValueBytes[1]);
    TEST_ASSERT(result[11] == pValueBytes[0]);


    elemSize = 6; // note that an ODD size doesn't work correctly
    numElements = swappedValues.size() / elemSize;
    sys::byteSwap(twelve_bytes, elemSize, numElements, swappedValues.data());
    TEST_ASSERT(pResultBytes[0] == pValueBytes[5]);
    TEST_ASSERT(pResultBytes[1] == pValueBytes[4]);
    TEST_ASSERT(pResultBytes[2] == pValueBytes[3]);
    TEST_ASSERT(pResultBytes[3] == pValueBytes[2]);
    TEST_ASSERT(pResultBytes[4] == pValueBytes[1]);
    TEST_ASSERT(pResultBytes[5] == pValueBytes[0]);

    TEST_ASSERT(pResultBytes[6] == pValueBytes[11]);
    TEST_ASSERT(pResultBytes[7] == pValueBytes[10]);
    TEST_ASSERT(pResultBytes[8] == pValueBytes[9]);
    TEST_ASSERT(pResultBytes[9] == pValueBytes[8]);
    TEST_ASSERT(pResultBytes[10] == pValueBytes[7]);
    TEST_ASSERT(pResultBytes[11] == pValueBytes[6]);

    sys::byteSwap(swappedValues.data(), elemSize, numElements); // swap back
    for (size_t i = 0; i < swappedValues.size(); i++)
    {
        TEST_ASSERT(pResultBytes[i] == pValueBytes[i]);
    }
}

TEST_MAIN(
    TEST_CHECK(testEndianness);
    TEST_CHECK(testByteSwap);
    TEST_CHECK(testByteSwapValues);
    TEST_CHECK(testByteSwap12);
    )
