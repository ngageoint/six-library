/* =========================================================================
 * This file is part of mt-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * mt-c++ is free software; you can redistribute it and/or modify
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

#include <mt/ThreadPlanner.h>
#include "TestCase.h"

namespace
{
TEST_CASE(GetThreadInfoTest)
{
    const size_t numThreads(16);
    const mt::ThreadPlanner planner(500, numThreads);
    size_t startElement(0);
    size_t numElements(0);
    for (size_t ii = 0; ii < numThreads; ++ii)
    {
        planner.getThreadInfo(ii, startElement, numElements);
        if (ii == numThreads - 1)
        {
            TEST_ASSERT_EQ(numElements, 20)
        }
        else
        {
            TEST_ASSERT_EQ(numElements, 32)
        }
    }
}

TEST_CASE(GetNumThreadsThatWillBeUsedTest)
{
    static const size_t NUM_ELEMENTS = 100;

    // This divides evenly
    const mt::ThreadPlanner planner1(NUM_ELEMENTS, 10);
    TEST_ASSERT_EQ(planner1.getNumThreadsThatWillBeUsed(), 10);

    // This doesn't divide evenly but we still need all the threads
    const mt::ThreadPlanner planner2(NUM_ELEMENTS, 9);
    TEST_ASSERT_EQ(planner2.getNumThreadsThatWillBeUsed(), 9);

    // This doesn't divide evenly - we only need 10 threads
    const mt::ThreadPlanner planner3(NUM_ELEMENTS, 11);
    TEST_ASSERT_EQ(planner3.getNumThreadsThatWillBeUsed(), 10);

    // Have one less thread than the number of elements - at least one thread
    // is going to need two elements, so we might as well just use 50 threads
    // and give them all two elements
    const mt::ThreadPlanner planner4(NUM_ELEMENTS, 99);
    TEST_ASSERT_EQ(planner4.getNumThreadsThatWillBeUsed(), 50);

    // Have as many elements as threads - one element per thread
    const mt::ThreadPlanner planner5(NUM_ELEMENTS, 100);
    TEST_ASSERT_EQ(planner5.getNumThreadsThatWillBeUsed(), 100);

    // Have more elements than threads - only the first NUM_ELEMENTS threads
    // are needed
    const mt::ThreadPlanner planner6(NUM_ELEMENTS, 101);
    TEST_ASSERT_EQ(planner6.getNumThreadsThatWillBeUsed(), 100);

    const mt::ThreadPlanner planner7(NUM_ELEMENTS, 9999);
    TEST_ASSERT_EQ(planner7.getNumThreadsThatWillBeUsed(), 100);
}
}

int main(int, char**)
{
    TEST_CHECK(GetThreadInfoTest);
    TEST_CHECK(GetNumThreadsThatWillBeUsedTest);

    return 0;
}
