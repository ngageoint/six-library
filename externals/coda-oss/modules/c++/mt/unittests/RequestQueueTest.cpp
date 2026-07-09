/* =========================================================================
 * This file is part of mt-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2025-26 ARKA Group, L.P. All rights reserved
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

#include <mt/RequestQueue.h>
#include "TestCase.h"

namespace
{
TEST_CASE(PriorityEnqueue)
{
    mt::RequestQueue<int> testSubject;
    testSubject.enqueue(1);
    testSubject.enqueue(2);
    testSubject.enqueue(3);
    testSubject.enqueue(4);
    testSubject.enqueue(5);
    testSubject.enqueue(6);
    testSubject.priorityEnqueue(7);

    int request = -1;
    testSubject.dequeue(request);
    TEST_ASSERT_EQ(request, 7);
    testSubject.dequeue(request);
    TEST_ASSERT_EQ(request, 1);
    testSubject.dequeue(request);
    TEST_ASSERT_EQ(request, 2);

    testSubject.priorityEnqueue(8);

    testSubject.dequeue(request);
    TEST_ASSERT_EQ(request, 8);
    testSubject.dequeue(request);
    TEST_ASSERT_EQ(request, 3);
}

}

int main(int, char**)
{
    TEST_CHECK(PriorityEnqueue);

    return 0;
}
