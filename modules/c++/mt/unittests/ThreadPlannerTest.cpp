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

#include "import/sys.h"
#include "import/mt.h"
#include "TestCase.h"

using namespace sys;
using namespace mt;
using namespace std;

TEST_CASE(ThreadPlannerTest)
{
    size_t numThreads(16);
    ThreadPlanner planner(500, numThreads);
    std::cout << "Num Elements Per Thread: " << planner.getNumElementsPerThread() << std::endl;
    size_t startElement(0);
    size_t numElements(0);
    for(size_t i=0; i < numThreads; ++i)
    {
        planner.getThreadInfo(i, startElement, numElements);
        std::cout << "Thread: " << i << " Start: " << startElement << " Num: " << numElements << std::endl;
        if(i == numThreads-1)
        {
            TEST_ASSERT_EQ(numElements, 20)
        }
        else
        {
            TEST_ASSERT_EQ(numElements, 32)
        }
    }
}

int main(int argc, char *argv[])
{
    TEST_CHECK(ThreadPlannerTest);

    return 0;
}
