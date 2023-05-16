/* =========================================================================
 * This file is part of mt-c++
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
#include <std/cstddef> // std::byte
#include <std/span>

#include <mt/ByteSwap.h>

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

TEST_CASE(testThreadedByteSwapV)
{
    constexpr size_t NUM_PIXELS = 10000;
    const auto origValues = make_origValues(NUM_PIXELS);

    constexpr size_t numThreads = 4;

    // Byte swap the old-fashioned way
    auto values1(origValues);
    mt::threadedByteSwap_(values1.data(), sizeof(uint64_t), NUM_PIXELS, numThreads);

    // Byte swap into output buffer
    std::vector<uint64_t> swappedValues2(origValues.size());
    mt::threadedByteSwap_(origValues.data(), sizeof(uint64_t), NUM_PIXELS, numThreads, swappedValues2.data());

    // Everything should match
    for (size_t ii = 0; ii < NUM_PIXELS; ++ii)
    {
        TEST_ASSERT_EQ(values1[ii], swappedValues2[ii]);
    }
}

TEST_CASE(testOldThreadedByteSwap)
{
    constexpr size_t NUM_PIXELS = 10000;
    const auto origValues = make_origValues(NUM_PIXELS);

    constexpr size_t numThreads = 4;

    // Byte swap the old-fashioned way
    auto values1(origValues);
    mt::threadedByteSwap(values1.data(), NUM_PIXELS, numThreads);

    // Byte swap into output buffer
    std::vector<uint64_t> swappedValues2(origValues.size());
    mt::threadedByteSwap(origValues.data(), NUM_PIXELS, numThreads, swappedValues2.data());

    // Everything should match
    for (size_t ii = 0; ii < NUM_PIXELS; ++ii)
    {
        TEST_ASSERT_EQ(values1[ii], swappedValues2[ii]);
    }
}

TEST_CASE(testThreadedByteSwap)
{
    constexpr size_t NUM_PIXELS = 10000;
    const auto origValues = make_origValues(NUM_PIXELS);
    const std::span<const uint64_t> origValues_(origValues.data(), origValues.size());

    constexpr size_t numThreads = 4;

    // Byte swap the old-fashioned way
    auto values1(origValues);
    const std::span<uint64_t> values1_(values1.data(), values1.size());
    mt::threadedByteSwap(values1_, numThreads);

    // Byte swap into output buffer
    std::vector<uint64_t> swappedValues2(origValues.size());
    const std::span<uint64_t> swappedValues2_(swappedValues2.data(), swappedValues2.size());
    mt::threadedByteSwap(origValues_, numThreads, swappedValues2_);

    // Everything should match
    for (size_t ii = 0; ii < NUM_PIXELS; ++ii)
    {
        TEST_ASSERT_EQ(values1[ii], swappedValues2[ii]);
    }
}

TEST_MAIN(
    TEST_CHECK(testThreadedByteSwapV);
    TEST_CHECK(testOldThreadedByteSwap);
    TEST_CHECK(testThreadedByteSwap);
    )
     