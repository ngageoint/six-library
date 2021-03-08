/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
template <typename TAtomicCounter>
static void testConstructor_(const std::string& testName)
{
    using ValueType = typename TAtomicCounter::ValueType;

    TEST_ASSERT_EQ(TAtomicCounter().get(), 0);
    TEST_ASSERT_EQ(TAtomicCounter(12345).get(), 12345);

    TEST_ASSERT_EQ(
        TAtomicCounter(std::numeric_limits<ValueType>::max()).get(),
        std::numeric_limits<ValueType>::max());
}
TEST_CASE(testConstructor)
{
    testConstructor_<sys::AtomicCounterOS>(testName);
    testConstructor_<sys::AtomicCounterCpp11>(testName);
    testConstructor_<sys::AtomicCounter>(testName);
}

template <typename TAtomicCounter>
static void testIncrement_(const std::string& testName)
{
    using ValueType = typename TAtomicCounter::ValueType;

    TAtomicCounter ctr(100);

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
TEST_CASE(testIncrement)
{
    testIncrement_<sys::AtomicCounterOS>(testName);
    testIncrement_<sys::AtomicCounterCpp11>(testName);
    testIncrement_<sys::AtomicCounter>(testName);
}

template <typename TAtomicCounter>
static void testDecrement_(const std::string& testName)
{
    using ValueType = typename TAtomicCounter::ValueType;

    TAtomicCounter ctr(100);

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
TEST_CASE(testDecrement)
{
    testDecrement_<sys::AtomicCounterOS>(testName);
    testDecrement_<sys::AtomicCounterCpp11>(testName);
    testDecrement_<sys::AtomicCounter>(testName);
}

template <typename TAtomicCounter, typename ValueType = typename TAtomicCounter::ValueType>
struct IncrementAtomicCounterT final : public sys::Runnable
{
    IncrementAtomicCounterT(size_t numIncrements,
                           TAtomicCounter& ctr,
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
    TAtomicCounter& mCtr;
    ValueType* const    mValues;
};

template <typename TAtomicCounter>
static void testThreadedIncrement_(const std::string& testName)
{
    using ValueType = typename TAtomicCounter::ValueType;
    using IncrementAtomicCounter = IncrementAtomicCounterT<TAtomicCounter>;

    const size_t numThreads = 13;
    const size_t numIncrements = 1000;

    std::vector<ValueType> values(numThreads * numIncrements);
    std::vector<const ValueType*> valuesPtr(numThreads);
    std::vector<sys::Thread *> threads(numThreads);
    TAtomicCounter ctr(0);

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
        TEST_ASSERT_EQ(static_cast<int64_t>(values[ii]), static_cast<int64_t>(ii));
    }
}
TEST_CASE(testThreadedIncrement)
{
    testThreadedIncrement_<sys::AtomicCounterOS>(testName);
    testThreadedIncrement_<sys::AtomicCounterCpp11>(testName);
    testThreadedIncrement_<sys::AtomicCounter>(testName);
}

template <typename TAtomicCounter, typename ValueType = typename TAtomicCounter::ValueType>
struct DecrementAtomicCounterT final : public sys::Runnable
{
    DecrementAtomicCounterT(size_t numDecrements,
                           TAtomicCounter& ctr,
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
    TAtomicCounter& mCtr;
    ValueType* const    mValues;
};

template <typename TAtomicCounter>
static void testThreadedDecrement_(const std::string& testName)
{
    using ValueType = typename TAtomicCounter::ValueType;
    using DecrementAtomicCounter = DecrementAtomicCounterT<TAtomicCounter>;

    const size_t numThreads = 13;
    const size_t numDecrements = 1000;

    std::vector<ValueType> values(numThreads * numDecrements);
    std::vector<const ValueType*> valuesPtr(numThreads);
    std::vector<sys::Thread *> threads(numThreads);
    TAtomicCounter ctr(numThreads * numDecrements - 1);

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
        TEST_ASSERT_EQ(static_cast<int64_t>(values[ii]), static_cast<int64_t>(ii));
    }
}
TEST_CASE(testThreadedDecrement)
{
    testThreadedDecrement_<sys::AtomicCounterOS>(testName);
    testThreadedDecrement_<sys::AtomicCounterCpp11>(testName);
    testThreadedDecrement_<sys::AtomicCounter>(testName);
}
}

int main(int, char**)
{
    TEST_CHECK(testConstructor);
    TEST_CHECK(testIncrement);
    TEST_CHECK(testDecrement);
    TEST_CHECK(testThreadedIncrement);
    TEST_CHECK(testThreadedDecrement);

    return 0;
}
