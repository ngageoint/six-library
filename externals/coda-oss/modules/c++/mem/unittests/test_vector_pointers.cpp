/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * mem-c++ is free software; you can redistribute it and/or modify
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

#include <mem/VectorOfPointers.h>

#include "TestCase.h"

namespace
{

TEST_CASE(testVecOfRawPointers)
{
    mem::VectorOfPointers<int> myVec;

    myVec.push_back(new int(1));
    myVec.push_back(new int(2));
    myVec.push_back(new int(3));
    myVec.push_back(new int(4));
    myVec.push_back(new int(5));

    TEST_ASSERT_EQ(myVec.size(), 5);

    myVec.erase(myVec.begin() + 2);

    TEST_ASSERT_EQ(myVec.size(), 4);
    TEST_ASSERT_EQ(*myVec[2], 4);

    myVec.push_back(new int(6));

    TEST_ASSERT_EQ(myVec.size(), 5);

    myVec.erase(myVec.begin(), myVec.begin() + 4);

    TEST_ASSERT_EQ(myVec.size(), 1);
    TEST_ASSERT_EQ(*myVec[0], 6);

    myVec.erase(myVec.begin());
    TEST_ASSERT_TRUE(myVec.empty());
}

TEST_CASE(testVecOfSharedPointers)
{
    mem::VectorOfSharedPointers<int> myVec;

    myVec.push_back(new int(1));
    myVec.push_back(new int(2));
    myVec.push_back(new int(3));
    myVec.push_back(new int(4));
    myVec.push_back(new int(5));

    TEST_ASSERT_EQ(myVec.size(), 5);

    myVec.erase(myVec.begin() + 2);

    TEST_ASSERT_EQ(myVec.size(), 4);
    TEST_ASSERT_EQ(*myVec[2], 4);

    myVec.push_back(new int(6));

    TEST_ASSERT_EQ(myVec.size(), 5);

    myVec.erase(myVec.begin(), myVec.begin() + 4);

    TEST_ASSERT_EQ(myVec.size(), 1);
    TEST_ASSERT_EQ(*myVec[0], 6);

    myVec.erase(myVec.begin());
    TEST_ASSERT_TRUE(myVec.empty());
}
}

int main(int, char**)
{
    TEST_CHECK(testVecOfRawPointers);
    TEST_CHECK(testVecOfSharedPointers);
    return 0;
}
