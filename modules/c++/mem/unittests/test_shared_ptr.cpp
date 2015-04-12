/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#include <mem/SharedPtr.h>

#include "TestCase.h"

namespace
{
TEST_CASE(testNullCopying)
{
    mem::SharedPtr<int> ptr1;
    TEST_ASSERT_EQ(ptr1.get(), static_cast<int *>(NULL));

    // Copy construction
    const mem::SharedPtr<int> ptr2(ptr1);
    TEST_ASSERT_EQ(ptr2.get(), static_cast<int *>(NULL));

    // Assignment operator
    mem::SharedPtr<int> ptr3;
    ptr3 = ptr1;
    TEST_ASSERT_EQ(ptr3.get(), static_cast<int *>(NULL));

    ptr1.reset();
    TEST_ASSERT_EQ(ptr1.get(), static_cast<int *>(NULL));
}

TEST_CASE(testAutoPtrConstructor)
{
    int * const rawPtr(new int(89));
    std::auto_ptr<int> autoPtr(rawPtr);
    const mem::SharedPtr<int> ptr(autoPtr);
    TEST_ASSERT_EQ(ptr.get(), rawPtr);
    TEST_ASSERT_EQ(autoPtr.get(), NULL);
    TEST_ASSERT_EQ(ptr.getCount(), 1);
}

TEST_CASE(testCopying)
{
    int * const rawPtr(new int(89));
    std::auto_ptr<mem::SharedPtr<int> > ptr3;
    {
        mem::SharedPtr<int> ptr1(rawPtr);
        TEST_ASSERT_EQ(ptr1.get(), rawPtr);
        TEST_ASSERT_EQ(*ptr1.get(), 89);
        TEST_ASSERT_EQ(ptr1.getCount(), 1);

        const mem::SharedPtr<int> ptr2(ptr1);
        TEST_ASSERT_EQ(ptr1.get(), rawPtr);
        TEST_ASSERT_EQ(ptr2.get(), rawPtr);
        TEST_ASSERT_EQ(*ptr2.get(), 89);
        TEST_ASSERT_EQ(ptr1.getCount(), 2);
        TEST_ASSERT_EQ(ptr2.getCount(), 2);

        // We already know they're pointing at the same underlying data, but
        // show it this way too for kicks
        *ptr2.get() = 334;
        TEST_ASSERT_EQ(*ptr1.get(), 334);
        TEST_ASSERT_EQ(*ptr2.get(), 334);

        // Decrement the reference count
        ptr1.reset();
        TEST_ASSERT_EQ(*ptr2.get(), 334);
        TEST_ASSERT_EQ(ptr2.getCount(), 1);

        // Do the same thing but show that ptr2 going out of scope is
        // equivalent to it being reset
        ptr3.reset(new mem::SharedPtr<int>(ptr2));
        TEST_ASSERT_EQ(ptr3->get(), rawPtr);
        TEST_ASSERT_EQ(ptr2.getCount(), 2);
        TEST_ASSERT_EQ(ptr3->getCount(), 2);
    }

    TEST_ASSERT_EQ(ptr3->get(), rawPtr);
    TEST_ASSERT_EQ(ptr3->getCount(), 1);
}

TEST_CASE(testAssigning)
{
    int * const rawPtr(new int(89));
    mem::SharedPtr<int> ptr3;
    {
        mem::SharedPtr<int> ptr1(rawPtr);
        TEST_ASSERT_EQ(ptr1.get(), rawPtr);
        TEST_ASSERT_EQ(*ptr1.get(), 89);
        TEST_ASSERT_EQ(ptr1.getCount(), 1);

        mem::SharedPtr<int> ptr2;
        ptr2 = ptr1;
        TEST_ASSERT_EQ(ptr1.get(), rawPtr);
        TEST_ASSERT_EQ(ptr2.get(), rawPtr);
        TEST_ASSERT_EQ(*ptr2.get(), 89);
        TEST_ASSERT_EQ(ptr1.getCount(), 2);
        TEST_ASSERT_EQ(ptr2.getCount(), 2);

        // We already know they're pointing at the same underlying data, but
        // show it this way too for kicks
        *ptr2.get() = 334;
        TEST_ASSERT_EQ(*ptr1.get(), 334);
        TEST_ASSERT_EQ(*ptr2.get(), 334);

        // Decrement the reference count
        ptr1.reset();
        TEST_ASSERT_EQ(*ptr2.get(), 334);
        TEST_ASSERT_EQ(ptr2.getCount(), 1);

        // Do the same thing but show that ptr2 going out of scope is
        // equivalent to it being reset
        ptr3 = ptr2;
        TEST_ASSERT_EQ(ptr3.get(), rawPtr);
        TEST_ASSERT_EQ(ptr2.getCount(), 2);
        TEST_ASSERT_EQ(ptr3.getCount(), 2);
    }

    TEST_ASSERT_EQ(ptr3.get(), rawPtr);
    TEST_ASSERT_EQ(ptr3.getCount(), 1);
}

struct Foo
{
    Foo(size_t val) :
        mVal(val)
    {
    }

    size_t mVal;
};

TEST_CASE(testSyntax)
{
    Foo* const rawPtr(new Foo(123));
    const mem::SharedPtr<Foo> ptr(rawPtr);

    TEST_ASSERT_EQ(ptr.get(), rawPtr);
    TEST_ASSERT_EQ(&*ptr, rawPtr);
    TEST_ASSERT_EQ(&(ptr->mVal), &(rawPtr->mVal));
}
}

int main(int argc, char **argv)
{
   TEST_CHECK(testNullCopying);
   TEST_CHECK(testAutoPtrConstructor);
   TEST_CHECK(testCopying);
   TEST_CHECK(testAssigning);
   TEST_CHECK(testSyntax);

   return 0;
}
