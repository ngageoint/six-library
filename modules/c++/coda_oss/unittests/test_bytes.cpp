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

#include "coda_oss_TestCase.h"

#include <array>

#include "coda_oss/bit.h" // std::endian
#include "coda_oss/cstddef.h"

TEST_CASE(testEndianness)
{
    /*const*/ auto native = coda_oss::endian::native; // "const" causes "conditional expression is constant."

    if (native == coda_oss::endian::big) { }
    else if (native == coda_oss::endian::little) { }
    else
    {
        TEST_FAIL("Mixed-endian not supported!");
    }
}

template<typename TEndian>
static void testEndianness_std_(const std::string& testName)
{
    /*const*/ auto native = TEndian::native; // "const" causes "conditional expression is constant."
    auto endianness = coda_oss::endian::native;  // "conditional expression is constant"
    if (native == TEndian::big)
    {
        TEST_ASSERT(endianness == coda_oss::endian::big);
    }
    else if (native == TEndian::little)
    {
        TEST_ASSERT(endianness == coda_oss::endian::little);
    }
    else
    {
        TEST_FAIL("Mixed-endian not supported!");
    }
}
TEST_CASE(testEndianness_std)
{
    testEndianness_std_<coda_oss::endian>(testName);
    //testEndianness_std_<std::endian>(testName);
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
    test_byte_<coda_oss::byte>(testName);
    #if defined(__cpp_lib_byte) && (__cpp_lib_byte >= 201603L) // https://en.cppreference.com/w/cpp/utility/feature_test
    test_byte_<std::byte>(testName);
    #endif
}

int main(int /*argc*/, char** /*argv*/)
{
    TEST_CHECK(testEndianness);
    TEST_CHECK(testEndianness_std);
    TEST_CHECK(testByte);

    return EXIT_SUCCESS;
}
