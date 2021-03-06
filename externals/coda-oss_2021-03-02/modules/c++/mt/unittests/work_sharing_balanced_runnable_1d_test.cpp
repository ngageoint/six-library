/* =========================================================================
 * This file is part of mt-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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
#include <mt/WorkSharingBalancedRunnable1D.h>
#include "TestCase.h"

namespace
{
class IncOp
{
public:
    IncOp(std::vector<size_t>& globalWorkDone) :
        mGlobalWorkDone(globalWorkDone)
    {
    }

    void operator()(size_t index) const
    {
        mGlobalWorkDone[index]++;
    }

private:
    std::vector<size_t>& mGlobalWorkDone;
};

TEST_CASE(WorkSharingBalancedRunnable1DTestWorkDone)
{
    const size_t numElements = 100000;
    const size_t initValue = 0;
    const size_t numThreads = sys::OS().getNumCPUs();
    for (size_t ii = 0; ii < numThreads; ++ii)
    {
        std::vector<size_t> workVec(numElements, initValue);
        IncOp op(workVec);

        mt::runWorkSharingBalanced1D(numElements, numThreads, op);

        TEST_ASSERT_EQ(workVec.size(), numElements);

        const size_t targetValue = 1;
        for (const auto& value : workVec)
        {
            TEST_ASSERT_EQ(value, targetValue);
        }
    }
}

TEST_CASE(WorkSharingBalancedRunnable1DTestWorkDoneLessWorkThanThreads)
{
    const size_t numElements = 6;
    const size_t initValue = 0;
    const size_t numThreads = 13;
    for (size_t ii = 0; ii < numThreads; ++ii)
    {
        std::vector<size_t> workVec(numElements, initValue);
        IncOp op(workVec);

        mt::runWorkSharingBalanced1D(numElements, numThreads, op);

        TEST_ASSERT_EQ(workVec.size(), numElements);

        const size_t targetValue = 1;
        for (const auto& value : workVec)
        {
            TEST_ASSERT_EQ(value, targetValue);
        }
    }
}
}

int main(int /*argc*/, char** /*argv*/)
{
    TEST_CHECK(WorkSharingBalancedRunnable1DTestWorkDone);
    TEST_CHECK(WorkSharingBalancedRunnable1DTestWorkDoneLessWorkThanThreads);
    return 0;
}
