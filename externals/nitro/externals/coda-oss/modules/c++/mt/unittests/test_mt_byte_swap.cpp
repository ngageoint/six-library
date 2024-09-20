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

#include <sys/ByteSwap.h>

#include <mt/ThreadedByteSwap.h>
#include <mt/Algorithm.h>

#undef min
#undef max

static std::vector<uint64_t> make_origValues_(size_t count)
{
    ::srand(334);

    std::vector<uint64_t> retval(count);
    for (size_t ii = 0; ii < count; ++ii)
    {
        const auto value = static_cast<float>(::rand()) / RAND_MAX *
                std::numeric_limits<uint64_t>::max();
        retval[ii] = static_cast<uint64_t>(value);
    }
    return retval;
}

static constexpr size_t NUM_PIXELS = 10000;
static const std::vector<uint64_t>& make_origValues()
{
    static const auto retval = make_origValues_(NUM_PIXELS);
    return retval;
}

TEST_CASE(testThreadedByteSwap)
{
    const auto& origValues = make_origValues();

    constexpr size_t numThreads = 4;

    // Byte swap the old-fashioned way
    auto values1(origValues);
    mt::threadedByteSwap(values1.data(), sizeof(values1[0]), NUM_PIXELS, numThreads);

    // Byte swap into output buffer
    std::vector<uint64_t> swappedValues2(origValues.size());
    mt::threadedByteSwap(origValues.data(), sizeof(origValues[0]), NUM_PIXELS, numThreads, swappedValues2.data());

    for (size_t ii = 0; ii < NUM_PIXELS; ++ii)  // Everything should match
    {
        TEST_ASSERT_EQ(values1[ii], swappedValues2[ii]);
    }
}

TEST_CASE(test_transform_ByteSwap)
{
    const auto& origValues = make_origValues();

    // Byte swap the old-fashioned way
    constexpr size_t numThreads = 4;
    auto expected_(origValues);
    constexpr auto elemSize = sizeof(expected_[0]);
    mt::threadedByteSwap(expected_.data(), elemSize, NUM_PIXELS, numThreads);
    const auto& expected = expected_;

    // Byte swap into output buffer
    const auto byteSwap = [&](const auto& buffer_) {
        auto buffer = buffer_;
        sys::byteSwap(&buffer, elemSize, 1 /*numElements*/);
        return buffer;
    };

    std::vector<uint64_t> actual(origValues.size());
    std::transform(origValues.begin(), origValues.end(), actual.begin(), byteSwap);
    for (size_t ii = 0; ii < NUM_PIXELS; ++ii)     // Everything should match
    {
        TEST_ASSERT_EQ(expected[ii], actual[ii]);
    }
}

TEST_CASE(test_Transform_par_ByteSwap)
{
    const auto& origValues = make_origValues();

    // Byte swap the old-fashioned way
    constexpr size_t numThreads = 4;
    auto expected_(origValues);
    constexpr auto elemSize = sizeof(expected_[0]);
    mt::threadedByteSwap(expected_.data(), elemSize, NUM_PIXELS, numThreads);
    const auto& expected = expected_;

    // Byte swap into output buffer
    const auto byteSwap = [&](const auto& buffer_) {
        auto buffer = buffer_;
        sys::byteSwap(&buffer, elemSize, 1 /*numElements*/);
        return buffer;
    };

    // be sure we do something more than just call std::transform()
    const mt::Transform_par_settings settings{ NUM_PIXELS / 4 /*cutoff*/ };

    std::vector<uint64_t> actual(origValues.size());
    mt::Transform_par(origValues.begin(), origValues.end(), actual.begin(), byteSwap, settings);
    for (size_t ii = 0; ii < NUM_PIXELS; ++ii)  // Everything should match
    {
        TEST_ASSERT_EQ(expected[ii], actual[ii]);
    }
}

TEST_MAIN(
    TEST_CHECK(testThreadedByteSwap);
    TEST_CHECK(test_transform_ByteSwap);
    TEST_CHECK(test_Transform_par_ByteSwap);
    )
     