/* =========================================================================
* This file is part of six-c++
* =========================================================================
*
* (C) Copyright 2004 - 2018, MDA Information Systems LLC
*
* six-c++ is free software; you can redistribute it and/or modify
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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "TestCase.h"
#include <six/Serialize.h>

namespace
{
template<typename T> T getRandomScalar()
{
    return static_cast<T>(rand() / static_cast<T>(RAND_MAX));
}

template<> int getRandomScalar<int>()
{
    return static_cast<int>(rand());
}

template<> size_t getRandomScalar<size_t>()
{
    return static_cast<size_t>(rand());
}

template<> sys::byte getRandomScalar<sys::byte>()
{
    return static_cast<sys::byte>(rand());
}

template<typename T> std::vector<T> getRandomVector(size_t length)
{
    std::vector<T> values(length);
    for (size_t ii = 0; ii < length; ++ii)
    {
        values[ii] = getRandomScalar<T>();
    }
    return values;
}

template<typename T>
bool testScalar(bool byteSwap)
{
    const T val = getRandomScalar<T>();
    std::vector<sys::byte> serializedData;
    six::serialize<T>(val, byteSwap, serializedData);
    const sys::byte* buffer = &serializedData[0];
    T valCopy;
    six::deserialize<T>(buffer, byteSwap, valCopy);
    return val == valCopy;
}

template<typename T>
bool testVector(size_t length, bool byteSwap)
{
    const std::vector<T> val = getRandomVector<T>(length);
    std::vector<sys::byte> serializedData;
    six::serialize<std::vector<T> >(val, byteSwap, serializedData);
    const sys::byte* buffer = &serializedData[0];
    std::vector<T> valCopy;
    six::deserialize<std::vector<T> >(buffer, byteSwap, valCopy);
    return val == valCopy;
}
}

TEST_CASE(ScalarSerialize)
{
    // Test with no byte swapping
    TEST_ASSERT_TRUE(testScalar<int>(false));
    TEST_ASSERT_TRUE(testScalar<size_t>(false));
    TEST_ASSERT_TRUE(testScalar<sys::byte>(false));
    TEST_ASSERT_TRUE(testScalar<float>(false));
    TEST_ASSERT_TRUE(testScalar<double>(false));

    // Test with byte swapping (do not test sys::byte)
    TEST_ASSERT_TRUE(testScalar<int>(true));
    TEST_ASSERT_TRUE(testScalar<size_t>(true));
    TEST_ASSERT_TRUE(testScalar<float>(true));
    TEST_ASSERT_TRUE(testScalar<double>(true));
}

TEST_CASE(VectorSerialize)
{
    const size_t length = 213;

    // Test with no byte swapping
    TEST_ASSERT_TRUE(testVector<int>(length, false));
    TEST_ASSERT_TRUE(testVector<size_t>(length, false));
    TEST_ASSERT_TRUE(testVector<float>(length, false));
    TEST_ASSERT_TRUE(testVector<double>(length, false));
    TEST_ASSERT_TRUE(testVector<sys::byte>(length, false));

    // Test with byte swapping (do not test sys::byte)
    TEST_ASSERT_TRUE(testVector<int>(length, true));
    TEST_ASSERT_TRUE(testVector<size_t>(length, true));
    TEST_ASSERT_TRUE(testVector<float>(length, true));
    TEST_ASSERT_TRUE(testVector<double>(length, true));
}

int main(int, char**)
{
    srand(time(NULL));
    TEST_CHECK(ScalarSerialize);
    TEST_CHECK(VectorSerialize);
}
