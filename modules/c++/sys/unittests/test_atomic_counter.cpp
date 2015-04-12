/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

#include <limits>
#include <vector>
#include <algorithm>

#include <sys/AtomicCounter.h>
#include <sys/Runnable.h>
#include <sys/Thread.h>

#include "TestCase.h"

namespace
{
typedef sys::AtomicCounter::ValueType ValueType;

TEST_CASE(testConstructor)
{
    TEST_ASSERT_EQ(sys::AtomicCounter().get(), 0);
    TEST_ASSERT_EQ(sys::AtomicCounter(12345).get(), 12345);

    TEST_ASSERT_EQ(
        sys::AtomicCounter(std::numeric_limits<ValueType>::max()).get(),
        std::numeric_limits<ValueType>::max());
}

TEST_CASE(testIncrement)
{
    sys::AtomicCounter ctr(100);

    TEST_ASSERT_EQ(ctr.getThenIncrement(), 100);
    TEST_ASSERT_EQ(ctr.get(), 101);

    TEST_ASSERT_EQ(ctr.getThenIncrement(), 101);
    TEST_ASSERT_EQ(ctr.get(), 102);

    ValueType value(ctr++);
    TEST_ASSERT_EQ(value, 102);
    TEST_ASSERT_EQ(ctr.get(), 103);

    TEST_ASSERT_EQ(ctr.incrementThenGet(), 104);
    TEST_ASSERT_EQ(ctr.get(), 104);

    value = ++ctr;
    TEST_ASSERT_EQ(value, 105);
    TEST_ASSERT_EQ(ctr.get(), 105);
}

TEST_CASE(testDecrement)
{
    sys::AtomicCounter ctr(100);

    TEST_ASSERT_EQ(ctr.getThenDecrement(), 100);
    TEST_ASSERT_EQ(ctr.get(), 99);

    TEST_ASSERT_EQ(ctr.getThenDecrement(), 99);
    TEST_ASSERT_EQ(ctr.get(), 98);

    ValueType value(ctr--);
    TEST_ASSERT_EQ(value, 98);
    TEST_ASSERT_EQ(ctr.get(), 97);

    TEST_ASSERT_EQ(ctr.decrementThenGet(), 96);
    TEST_ASSERT_EQ(ctr.get(), 96);

    value = --ctr;
    TEST_ASSERT_EQ(value, 95);
    TEST_ASSERT_EQ(ctr.get(), 95);
}

class IncrementAtomicCounter : public sys::Runnable
{
public:
    IncrementAtomicCounter(size_t numIncrements,
                           sys::AtomicCounter& ctr,
                           ValueType* values) :
        mNumIncrements(numIncrements),
        mCtr(ctr),
        mValues(values)
    {
    }

    virtual void run()
    {
        for (size_t ii = 0; ii < mNumIncrements; ++ii)
        {
            mValues[ii] = mCtr.getThenIncrement();
        }
    }

private:
    const size_t        mNumIncrements;
    sys::AtomicCounter& mCtr;
    ValueType* const    mValues;
};

TEST_CASE(testThreadedIncrement)
{
    const size_t numThreads = 13;
    const size_t numIncrements = 1000;

    std::vector<ValueType> values(numThreads * numIncrements);
    std::vector<const ValueType*> valuesPtr(numThreads);
    std::vector<sys::Thread *> threads(numThreads);
    sys::AtomicCounter ctr(0);

    // Create all the threads
    ValueType* ptr(&values[0]);
    for (size_t ii = 0; ii < numThreads; ++ii, ptr += numIncrements)
    {
        threads[ii] =
            new sys::Thread(new IncrementAtomicCounter(numIncrements,
                                                       ctr,
                                                       ptr));
        valuesPtr[ii] = ptr;
    }

    for (size_t ii = 0; ii < numThreads; ++ii)
    {
        threads[ii]->start();
    }

    // Wait for them all
    for (size_t ii = 0; ii < numThreads; ++ii)
    {
        threads[ii]->join();
        delete threads[ii];
    }

    // Each thread should have its values monotonically increasing
    for (size_t ii = 0; ii < numThreads; ++ii)
    {
        const ValueType* const threadValues(valuesPtr[ii]);
        for (size_t jj = 0, end = numIncrements - 1; jj < end; ++jj)
        {
            TEST_ASSERT(threadValues[jj + 1] > threadValues[jj]);
        }
    }

    // We should have gotten every value along the way
    std::sort(values.begin(), values.end());
    for (size_t ii = 0; ii < values.size(); ++ii)
    {
        TEST_ASSERT_EQ(values[ii], ii);
    }
}

class DecrementAtomicCounter : public sys::Runnable
{
public:
    DecrementAtomicCounter(size_t numDecrements,
                           sys::AtomicCounter& ctr,
                           ValueType* values) :
        mNumDecrements(numDecrements),
        mCtr(ctr),
        mValues(values)
    {
    }

    virtual void run()
    {
        for (size_t ii = 0; ii < mNumDecrements; ++ii)
        {
            mValues[ii] = mCtr.getThenDecrement();
        }
    }

private:
    const size_t        mNumDecrements;
    sys::AtomicCounter& mCtr;
    ValueType* const    mValues;
};

TEST_CASE(testThreadedDecrement)
{
    const size_t numThreads = 13;
    const size_t numDecrements = 1000;

    std::vector<ValueType> values(numThreads * numDecrements);
    std::vector<const ValueType*> valuesPtr(numThreads);
    std::vector<sys::Thread *> threads(numThreads);
    sys::AtomicCounter ctr(numThreads * numDecrements - 1);

    // Create all the threads
    ValueType* ptr(&values[0]);
    for (size_t ii = 0; ii < numThreads; ++ii, ptr += numDecrements)
    {
        threads[ii] =
            new sys::Thread(new DecrementAtomicCounter(numDecrements,
                                                       ctr,
                                                       ptr));
        valuesPtr[ii] = ptr;
    }

    for (size_t ii = 0; ii < numThreads; ++ii)
    {
        threads[ii]->start();
    }

    // Wait for them all
    for (size_t ii = 0; ii < numThreads; ++ii)
    {
        threads[ii]->join();
        delete threads[ii];
    }

    // Each thread should have its values monotonically decreasing
    for (size_t ii = 0; ii < numThreads; ++ii)
    {
        const ValueType* const threadValues(valuesPtr[ii]);
        for (size_t jj = 0, end = numDecrements - 1; jj < end; ++jj)
        {
            TEST_ASSERT(threadValues[jj + 1] < threadValues[jj]);
        }
    }

    // We should have gotten every value along the way
    std::sort(values.begin(), values.end());
    for (size_t ii = 0; ii < values.size(); ++ii)
    {
        TEST_ASSERT_EQ(values[ii], ii);
    }
}
}

int main(int argc, char** argv)
{
    TEST_CHECK(testConstructor);
    TEST_CHECK(testIncrement);
    TEST_CHECK(testDecrement);
    TEST_CHECK(testThreadedIncrement);
    TEST_CHECK(testThreadedDecrement);

    return 0;
}
