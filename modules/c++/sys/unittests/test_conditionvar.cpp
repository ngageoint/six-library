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

#include <sys/ConditionVar.h>
#include <mt/CriticalSection.h>

#include "TestCase.h"

typedef mt::CriticalSection<sys::Mutex> CriticalSection;

TEST_CASE(testDefaultConstructor)
{
    sys::ConditionVar cond;
    TEST_SUCCESS;

    cond.acquireLock();
    TEST_SUCCESS;

    cond.dropLock();
    TEST_SUCCESS;
}

TEST_CASE(testParameterizedConstructor)
{
    sys::Mutex mutex;
    TEST_SUCCESS;

    sys::ConditionVar cond(mutex);
    TEST_SUCCESS;

    cond.acquireLock();
    TEST_SUCCESS;

    cond.dropLock();
    TEST_SUCCESS;
}

TEST_CASE(testMultipleTimeouts)
{
    sys::Mutex mutex;
    TEST_SUCCESS;

    sys::ConditionVar cond(mutex);
    TEST_SUCCESS;

    for (size_t ii = 0; ii < 5; ++ii)
    {
        CriticalSection scopedLock(mutex);
        TEST_SUCCESS;
        try
        {
            cond.wait(0.001);
        }
        catch (const except::Exception&)
        {
        }
    }
    {
        CriticalSection scopedLock(mutex);
        TEST_SUCCESS;

        cond.broadcast();
        TEST_SUCCESS;
    }
}

TEST_MAIN(
    TEST_CHECK(testDefaultConstructor);
    TEST_CHECK(testParameterizedConstructor);
    TEST_CHECK(testMultipleTimeouts);
    )

