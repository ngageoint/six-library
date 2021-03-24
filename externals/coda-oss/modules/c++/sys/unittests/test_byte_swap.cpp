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

#include <std/bit> // std::endian
#include <std/cstddef>

#include <sys/Conf.h>

namespace
{
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

TEST_CASE(testEndianness)
{
    /*const*/ auto native = sys::Endian::native; // "const" causes "conditional expression is constant."

    if (native == sys::Endian::big) { }
    else if (native == sys::Endian::little) { }
    else
    {
        TEST_FAIL("Mixed-endian not supported!");
    }

    const bool isBigEndianSystem = sys::isBigEndianSystem();

    if (native == sys::Endian::big)
    {
        TEST_ASSERT(isBigEndianSystem);
    }
    else
    {
        TEST_ASSERT(!isBigEndianSystem);    
    }
    if (native == sys::Endian::little)
    {
        TEST_ASSERT(!isBigEndianSystem);
    }
    else
    {
        TEST_ASSERT(isBigEndianSystem);
    }


    if (isBigEndianSystem)
    {
        TEST_ASSERT(native == sys::Endian::big);
    }
    else
    {
        TEST_ASSERT(native == sys::Endian::little);    
    }
}

template<typename TEndian>
static void testEndianness_std_(const std::string& testName)
{
    /*const*/ auto native = TEndian::native; // "const" causes "conditional expression is constant."
    auto endianness = sys::Endian::native;  // "conditional expression is constant"
    if (native == TEndian::big)
    {
        TEST_ASSERT(endianness == sys::Endian::big);
    }
    else if (native == TEndian::little)
    {
        TEST_ASSERT(endianness == sys::Endian::little);
    }
    else
    {
        TEST_FAIL("Mixed-endian not supported!");
    }
}
TEST_CASE(testEndianness_std)
{
    testEndianness_std_<sys::Endian>(testName);
    testEndianness_std_<std::endian>(testName);
}

template <typename TByte>
static void test_byte_(const std::string& testName)
{
    std::array<TByte, 256> bytes;
    for (size_t i = 0; i < bytes.size(); i++)
    {
        auto value = static_cast<TByte>(i);
        bytes[i] = value;
    }

    const auto actuals = bytes;  // copy
    TEST_ASSERT_EQ(actuals.size(), bytes.size());
    for (size_t i = 0; i < actuals.size(); i++)
    {
        const auto actual = static_cast<size_t>(actuals[i]);
        TEST_ASSERT_EQ(i, actual);
    }
}
TEST_CASE(testByte)
{
    test_byte_<sys::Byte>(testName);
    test_byte_<std::byte>(testName);
    #if CODA_OSS_lib_byte
    test_byte_<std::byte>(testName);
    #endif
}
    
}

int main(int /*argc*/, char** /*argv*/)
{
    TEST_CHECK(testByteSwap);
    TEST_CHECK(testEndianness);
    TEST_CHECK(testEndianness_std);
    TEST_CHECK(testByte);
    return 0;
}
