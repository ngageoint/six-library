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

TEST_CASE(testEndianness)
{
    /*const*/ auto native = std::endian::native; // "const" causes "conditional expression is constant."

    if (native == std::endian::big) { }
    else if (native == std::endian::little) { }
    else
    {
        TEST_FAIL("Mixed-endian not supported!");
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


TEST_MAIN(
    TEST_CHECK(testEndianness);
    TEST_CHECK(testByteSwap);
    )
