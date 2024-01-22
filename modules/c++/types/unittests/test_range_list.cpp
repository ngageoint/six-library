/* =========================================================================
 * This file is part of types-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, Radiant Geospatial Solutions
 *
 * types-c++ is free software; you can redistribute it and/or modify
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
#include <vector>

#include "TestCase.h"
#include <types/RangeList.h>

TEST_CASE(TestDisjointInsertion)
{
    types::RangeList RL;
    RL.insert({3, 4}); // [3, 7)
    RL.insert({9, 2}); // [9, 11)
    RL.insert({1, 1}); // [1, 2)

    const std::vector<types::Range> ranges = RL.getRanges();

    TEST_ASSERT_EQ(ranges.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(ranges[0].mStartElement, static_cast<size_t>(1));
    TEST_ASSERT_EQ(ranges[0].mNumElements, static_cast<size_t>(1));
    TEST_ASSERT_EQ(ranges[1].mStartElement, static_cast<size_t>(3));
    TEST_ASSERT_EQ(ranges[1].mNumElements, static_cast<size_t>(4));
    TEST_ASSERT_EQ(ranges[2].mStartElement, static_cast<size_t>(9));
    TEST_ASSERT_EQ(ranges[2].mNumElements, static_cast<size_t>(2));
}

TEST_CASE(TestMergedInsertion)
{
    types::RangeList RL;
    RL.insert({3, 4}); // [3, 7)
    RL.insert({9, 2}); // [9, 11)
    RL.insert({1, 1}); // [1, 2)
    RL.insert({2, 1}); // [2, 3) merging starts now
    RL.insert({6, 6}); // [6, 12)
    // Final range should be [1, 12)

    const std::vector<types::Range> ranges = RL.getRanges();

    TEST_ASSERT_EQ(ranges.size(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(ranges[0].mStartElement, static_cast<size_t>(1));
    TEST_ASSERT_EQ(ranges[0].mNumElements, static_cast<size_t>(11));
}

TEST_CASE(TestSinglePointTouching)
{
    types::RangeList RL;
    RL.insert(2);
    RL.insert(4);
    RL.insert(6);
    RL.insert(7);
    RL.insert(5);
    RL.insert(1);
    RL.insert(3);
    // Final range should be [1, 8)

    const std::vector<types::Range> ranges = RL.getRanges();

    TEST_ASSERT_EQ(ranges.size(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(ranges[0].mStartElement, static_cast<size_t>(1));
    TEST_ASSERT_EQ(ranges[0].mNumElements, static_cast<size_t>(7));
}

TEST_CASE(TestRemoveFromMiddle)
{
    types::RangeList RL(types::Range(10, 10)); // [10, 20)
    RL.remove({11, 5}); // [11, 16)
    // Final ranges should be [10, 11), [16, 20)

    const std::vector<types::Range> ranges = RL.getRanges();

    TEST_ASSERT_EQ(ranges.size(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(ranges[0].mStartElement, static_cast<size_t>(10));
    TEST_ASSERT_EQ(ranges[0].mNumElements, static_cast<size_t>(1));
    TEST_ASSERT_EQ(ranges[1].mStartElement, static_cast<size_t>(16));
    TEST_ASSERT_EQ(ranges[1].mNumElements, static_cast<size_t>(4));
}

TEST_CASE(TestRemoveFromLeft)
{
    types::RangeList RL(types::Range(10, 10)); // [10, 20)
    RL.remove({8, 5}); // [8, 13)
    // Final range should be [13, 20)

    const std::vector<types::Range> ranges = RL.getRanges();

    TEST_ASSERT_EQ(ranges.size(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(ranges[0].mStartElement, static_cast<size_t>(13));
    TEST_ASSERT_EQ(ranges[0].mNumElements, static_cast<size_t>(7));
}

TEST_CASE(TestRemoveFromRight)
{
    types::RangeList RL(types::Range(10, 10)); // [10, 20)
    RL.remove({15, 10}); // [15, 25)
    // Final range should be [10, 15)

    const std::vector<types::Range> ranges = RL.getRanges();

    TEST_ASSERT_EQ(ranges.size(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(ranges[0].mStartElement, static_cast<size_t>(10));
    TEST_ASSERT_EQ(ranges[0].mNumElements, static_cast<size_t>(5));
}

TEST_CASE(TestRemoveMultiRangeOverlap)
{
    const std::vector<types::Range> initRanges =
    {
        { 0,  5}, // [0, 5)
        {10, 10}, // [10, 20)
        {25,  5}  // [25, 30)
    };

    types::RangeList RL(initRanges);
    RL.remove({2, 25}); // [2, 27)
    // Final ranges should be [0, 2), [27, 30)

    const std::vector<types::Range> ranges = RL.getRanges();

    TEST_ASSERT_EQ(ranges.size(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(ranges[0].mStartElement, static_cast<size_t>(0));
    TEST_ASSERT_EQ(ranges[0].mNumElements, static_cast<size_t>(2));
    TEST_ASSERT_EQ(ranges[1].mStartElement, static_cast<size_t>(27));
    TEST_ASSERT_EQ(ranges[1].mNumElements, static_cast<size_t>(3));
}

TEST_CASE(TestExpansion)
{
    const std::vector<types::Range> initRanges =
    {
        { 1,  3}, // [1, 4)
        {10, 10}, // [10, 20)
        {24,  5}  // [24, 29)
    };

    types::RangeList RL(initRanges);
    RL.expand(2);
    // Final ranges should be [0, 6), [8, 31)

    const std::vector<types::Range> ranges = RL.getRanges();

    TEST_ASSERT_EQ(ranges.size(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(ranges[0].mStartElement, static_cast<size_t>(0));
    TEST_ASSERT_EQ(ranges[0].mNumElements, static_cast<size_t>(6));
    TEST_ASSERT_EQ(ranges[1].mStartElement, static_cast<size_t>(8));
    TEST_ASSERT_EQ(ranges[1].mNumElements, static_cast<size_t>(23));
}

TEST_CASE(TestIntersection)
{
    // Compute the intersection C = A.intersect(B)
    // A:     |--------|    |-------|   |--------|  |-| |-|  |-|
    // B:    |----------|     |---|      |-| |-|   |-------|     |-|
    // C:     |--------|      |---|      |-| |-|    |-| |-|

    const types::RangeList A(std::vector<types::Range>(
    {
        {2, 5},
        {10, 5},
        {20, 10},
        {31, 2},
        {37, 1},
        {45, 1}

    }));

    const types::RangeList B(std::vector<types::Range>(
    {
        {1, 6},
        {12, 2},
        {22, 1},
        {26, 3},
        {30, 10},
        {46, 1}
    }));

    // Final ranges: [2, 7), [12, 14), [22, 23), [26, 29), [31, 33), [37, 38)
    const types::RangeList C = A.intersect(B);

    const std::vector<types::Range> ranges = C.getRanges();
    TEST_ASSERT_EQ(ranges.size(), static_cast<size_t>(6));
    TEST_ASSERT_TRUE(types::Range(2,5) == ranges[0]);
    TEST_ASSERT_TRUE(types::Range(12,2) == ranges[1]);
    TEST_ASSERT_TRUE(types::Range(22,1) == ranges[2]);
    TEST_ASSERT_TRUE(types::Range(26,3) == ranges[3]);
    TEST_ASSERT_TRUE(types::Range(31,2) == ranges[4]);
    TEST_ASSERT_TRUE(types::Range(37,1) == ranges[5]);
}

TEST_MAIN(
    TEST_CHECK(TestDisjointInsertion);
    TEST_CHECK(TestMergedInsertion);
    TEST_CHECK(TestSinglePointTouching);
    TEST_CHECK(TestRemoveFromMiddle);
    TEST_CHECK(TestRemoveFromLeft);
    TEST_CHECK(TestRemoveFromRight);
    TEST_CHECK(TestRemoveMultiRangeOverlap);
    TEST_CHECK(TestExpansion);
    TEST_CHECK(TestIntersection);
)
