/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
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

#include <std/memory>

#include <mem/ScopedCloneablePtr.h>

#include "TestCase.h"

namespace
{
struct Foo final
{
    int val1 = 0;
    int val2 = 0;
    Foo() = default;
    Foo(int v1, int v2) : val1(v1), val2(v2){};
    Foo(const Foo&) = delete;
    Foo& operator=(const Foo&) = delete;
    std::unique_ptr<Foo> clone() const
    {
        // compiler has a hard time resolving overload ... probably because =deletes
        //return std::make_unique<Foo>(val1, val2);
        return std::unique_ptr<Foo>(new Foo(val1, val2));
    }
};

struct Bar final
{
    mem::ScopedCloneablePtr<Foo> foo;
    int val3 = 0;
};

struct Baz final
{
    std::shared_ptr<Foo> pFoo;
    int val3 = 0;
};

struct AssignOnDestruct final
{
    AssignOnDestruct(int &ref, int finalVal) :
        mRef(ref),
        mFinalVal(finalVal)
    {
    }

    ~AssignOnDestruct()
    {
        mRef = mFinalVal;
    }

private:
    int&      mRef;
    const int mFinalVal;
};

TEST_CASE(testCopyConstructor)
{
    // Initialize the values
    Bar bar1;
    bar1.foo.reset(new Foo());
    bar1.foo->val1 = 10;
    bar1.foo->val2 = 20;
    bar1.val3 = 30;

    // Show that the compiler-generated copy constructor is correct
    Bar bar2(bar1);
    TEST_ASSERT_EQ(bar2.foo->val1, 10);
    TEST_ASSERT_EQ(bar2.foo->val2, 20);
    TEST_ASSERT_EQ(bar2.val3, 30);

    // Show it was a deep copy
    bar2.foo->val1 = 40;
    bar2.foo->val2 = 50;
    bar2.val3 = 60;
    TEST_ASSERT_EQ(bar1.foo->val1, 10);
    TEST_ASSERT_EQ(bar1.foo->val2, 20);
    TEST_ASSERT_EQ(bar1.val3, 30);
}

TEST_CASE(testSharedCopyConstructor)
{
    // Initialize the values
    Baz b1;
    b1.pFoo.reset(new Foo());
    b1.pFoo->val1 = 10;
    b1.pFoo->val2 = 20;
    b1.val3 = 30;

    // Show that memory is shared, not copied as with mem::ScopedCloneablePtr
    auto b2 = b1;
    b2.pFoo->val1 = 40;
    b2.pFoo->val2 = 50;
    b2.val3 = 60;
    TEST_ASSERT_EQ(b1.pFoo->val1, 40);
    TEST_ASSERT_EQ(b1.pFoo->val2, 50);
    TEST_ASSERT_EQ(b1.val3, 30);
}

TEST_CASE(testAssignmentOperator)
{
    // Initialize the values
    Bar bar1;
    bar1.foo.reset(new Foo());
    bar1.foo->val1 = 10;
    bar1.foo->val2 = 20;
    bar1.val3 = 30;

    // Show that the compiler-generated assignment operator is correct
    Bar bar2;
    bar2 = bar1;
    TEST_ASSERT_EQ(bar2.foo->val1, 10);
    TEST_ASSERT_EQ(bar2.foo->val2, 20);
    TEST_ASSERT_EQ(bar2.val3, 30);

    // Show it was a deep copy
    bar2.foo->val1 = 40;
    bar2.foo->val2 = 50;
    bar2.val3 = 60;
    TEST_ASSERT_EQ(bar1.foo->val1, 10);
    TEST_ASSERT_EQ(bar1.foo->val2, 20);
    TEST_ASSERT_EQ(bar1.val3, 30);
}

TEST_CASE(testSharedAssignmentOperator)
{
    // Initialize the values
    Baz b1;
    b1.pFoo.reset(new Foo());
    b1.pFoo->val1 = 10;
    b1.pFoo->val2 = 20;
    b1.val3 = 30;

    Baz b2;
    b2 = b1;

    // Show that memory is shared, not copied as with mem::ScopedCloneablePtr
    b2.pFoo->val1 = 40;
    b2.pFoo->val2 = 50;
    b2.val3 = 60;
    TEST_ASSERT_EQ(b1.pFoo->val1, 40);
    TEST_ASSERT_EQ(b1.pFoo->val2, 50);
    TEST_ASSERT_EQ(b1.val3, 30);
}

TEST_CASE(testDestructor)
{
    // When the ScopedCloneablePtr goes out of scope, it should delete the
    // pointer which will cause the AssignOnDestruct destructor to assign
    // 'val'
    int val(0);
    {
        const mem::ScopedCloneablePtr<AssignOnDestruct> ptr(
            new AssignOnDestruct(val, 334));
        TEST_ASSERT_EQ(val, 0);
    }

    TEST_ASSERT_EQ(val, 334);
}

TEST_CASE(testSyntax)
{
    Foo* const rawPtr(new Foo());
    const mem::ScopedCloneablePtr<Foo> ptr(rawPtr);

    TEST_ASSERT_EQ(ptr.get(), rawPtr);
    TEST_ASSERT_EQ(&*ptr, rawPtr);
    TEST_ASSERT_EQ(&(ptr->val1), &(rawPtr->val1));
}

TEST_CASE(testEqualityOperator)
{
    mem::ScopedCloneablePtr<int> ptr1;
    mem::ScopedCloneablePtr<int> ptr2;

    //Null smart pointers are equal
    TEST_ASSERT(ptr1 == ptr2);

    ptr1.reset(new int(4));

    TEST_ASSERT(ptr1 != ptr2);

    ptr2.reset(new int(5));

    TEST_ASSERT_FALSE(ptr1 == ptr2);

    ptr2.reset(new int(4));

    TEST_ASSERT_FALSE(ptr1 != ptr2);
}
}

int main(int, char**)
{
    TEST_CHECK(testCopyConstructor);
    TEST_CHECK(testSharedCopyConstructor);
    TEST_CHECK(testAssignmentOperator);
    TEST_CHECK(testSharedAssignmentOperator);    
    TEST_CHECK(testDestructor);
    TEST_CHECK(testSyntax);
    TEST_CHECK(testEqualityOperator);

    return 0;
}
