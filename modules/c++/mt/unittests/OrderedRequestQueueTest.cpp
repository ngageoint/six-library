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

#include <mt/OrderedRequestQueue.h>
#include "TestCase.h"

class EvenFirst
{
public:
    bool operator()(int lhs, int rhs) const
    {
        bool result;
        if ((lhs % 2 && rhs % 2) || (!(lhs%2) && !(rhs%2)) )
        {
            result = lhs < rhs;
        }
        else
        {
            result = !(lhs % 2);
        }
        return result;
    }
};

namespace
{
/**
 * Tests priority enqueue and dequeue via the OrderedRequestQueues
 * standard numerical ordering and a custom even first comparator
 */
TEST_CASE(PriorityEnqueue)
{
    mt::OrderedRequestQueue<int> testSubject;
    testSubject.enqueue(1);
    testSubject.enqueue(2);
    testSubject.enqueue(3);
    testSubject.enqueue(4);
    testSubject.enqueue(5);
    testSubject.enqueue(6);

    int request = -1;
    testSubject.dequeue(request);
    TEST_ASSERT_EQ(request, 1);
    testSubject.dequeue(request);
    TEST_ASSERT_EQ(request, 2);
    testSubject.dequeue(request);
    TEST_ASSERT_EQ(request, 3);

    testSubject.enqueue(10);

    testSubject.dequeue(request);
    TEST_ASSERT_EQ(request, 4);
    testSubject.dequeue(request);
    TEST_ASSERT_EQ(request, 5);
    
    testSubject.enqueue(1);

    testSubject.dequeue(request);
    TEST_ASSERT_EQ(request, 1);

    mt::OrderedRequestQueue<int, EvenFirst> test2;
    test2.enqueue(1);
    test2.enqueue(2);
    test2.enqueue(3);
    test2.enqueue(4);
    test2.enqueue(5);
    test2.enqueue(6);

    test2.dequeue(request);
    TEST_ASSERT_EQ(request, 2);
    test2.dequeue(request);
    TEST_ASSERT_EQ(request, 4);
    test2.dequeue(request);
    TEST_ASSERT_EQ(request, 6);
    test2.dequeue(request);
    TEST_ASSERT_EQ(request, 1);
    test2.dequeue(request);
    TEST_ASSERT_EQ(request, 3);
    test2.dequeue(request);
    TEST_ASSERT_EQ(request, 5);


}

}

int main(int, char**)
{
    TEST_CHECK(PriorityEnqueue);

    return 0;
}
