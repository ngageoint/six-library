/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <config/coda_oss_config.h>
#include <mem/SharedPtr.h>

#include "TestCase.h"

namespace
{
struct Foo
{
    Foo(size_t val) :
        mVal(val)
    {
    }

    size_t mVal;
};

struct Bar : public Foo
{
    Bar(size_t val) : Foo(val)
    {
    }
};

struct FooPtrTest
{
    FooPtrTest(mem::SharedPtr<Foo> ptr) :
        mFooPtr(ptr)
    {
    }

    mem::SharedPtr<Foo> mFooPtr;
};

struct BarPtrTest : public FooPtrTest
{
    BarPtrTest(mem::SharedPtr<Bar> ptr) :
        FooPtrTest(ptr), mBarPtr(ptr)
    {
    }

    mem::SharedPtr<Bar> mBarPtr;
};

size_t cpp11Function(std::shared_ptr<Foo> foo)
{
    return foo->mVal;
}

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
    TEST_ASSERT_EQ(autoPtr.get(), static_cast<int *>(NULL));
    TEST_ASSERT_EQ(ptr.getCount(), 1);
}

TEST_CASE(testAutoPtrReset)
{
    // Similar to the construction test,
    // except using the reset() that takes an auto_ptr
    int* const rawPtr1 = new int(90);
    std::auto_ptr<int> autoPtr(rawPtr1);

    int* const rawPtr2 = new int(100);
    mem::SharedPtr<int> sharedPtr(rawPtr2);

    TEST_ASSERT_EQ(autoPtr.get(), rawPtr1);
    TEST_ASSERT_EQ(sharedPtr.get(), rawPtr2);

    sharedPtr.reset(autoPtr);
    TEST_ASSERT_EQ(sharedPtr.get(), rawPtr1);
    TEST_ASSERT_NULL(autoPtr.get());
    TEST_ASSERT_EQ(sharedPtr.getCount(), 1);
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

TEST_CASE(testSyntax)
{
    Foo* const rawPtr(new Foo(123));
    const mem::SharedPtr<Foo> ptr(rawPtr);

    TEST_ASSERT_EQ(ptr.get(), rawPtr);
    TEST_ASSERT_EQ(&*ptr, rawPtr);
    TEST_ASSERT_EQ(&(ptr->mVal), &(rawPtr->mVal));
}

TEST_CASE(testCasting)
{
    {
        // Test creating SharedPtr of base class from raw pointer of derived
        Bar* const rawBar(new Bar(456));
        const mem::SharedPtr<Foo> fooPtr(rawBar);
        TEST_ASSERT_EQ(fooPtr.get(), rawBar);
        TEST_ASSERT_EQ(fooPtr.getCount(), 1);
    }

    {
        // Test creating SharedPtr of base class from auto pointer of derived
        Bar* const rawBar(new Bar(456));
        std::auto_ptr<Bar> autoBar(rawBar);
        const mem::SharedPtr<Foo> fooPtr(autoBar);
        TEST_ASSERT_EQ(fooPtr.get(), rawBar);
        TEST_ASSERT_EQ(autoBar.get(), static_cast<Bar *>(NULL));
        TEST_ASSERT_EQ(fooPtr.getCount(), 1);
    }

    {
        // Test creating SharedPtr of base class from SharedPtr of derived
        Bar* const rawBar(new Bar(456));
        const mem::SharedPtr<Bar> barPtr(rawBar);
        TEST_ASSERT_EQ(barPtr.getCount(), 1);
        const mem::SharedPtr<Foo> fooPtr(barPtr);
        TEST_ASSERT_EQ(fooPtr.get(), rawBar);
        TEST_ASSERT_EQ(fooPtr.getCount(), 2);
        TEST_ASSERT_EQ(barPtr.get(), rawBar);
        TEST_ASSERT_EQ(barPtr.getCount(), 2);

        const mem::SharedPtr<Foo> fooPtr2 = barPtr;
        TEST_ASSERT_EQ(fooPtr2.get(), rawBar);
        TEST_ASSERT_EQ(fooPtr2.getCount(), 3);
    }

    {
        // Test creating a class with a SharedPtr of base class as parameter
        // from a class with a SharedPtr of derived as parameter
        Bar* const rawBar(new Bar(456));
        const mem::SharedPtr<Bar> barPtr(rawBar);
        TEST_ASSERT_EQ(barPtr.getCount(), 1);
        const BarPtrTest barTest(barPtr);
        TEST_ASSERT_EQ(barTest.mFooPtr.get(), rawBar);
        TEST_ASSERT_EQ(barTest.mBarPtr.get(), rawBar);
        TEST_ASSERT_EQ(barPtr.getCount(), 3);
    }
}

TEST_CASE(testStdSharedPtr)
{
    const mem::SharedPtr<Foo> fooLegacy(new Foo(123));
    std::shared_ptr<Foo> fooCtor(fooLegacy);
    TEST_ASSERT_EQ(fooLegacy.get(), fooCtor.get());

    std::shared_ptr<Foo> fooAssign = fooLegacy;
    TEST_ASSERT_EQ(fooLegacy.get(), fooAssign.get());

    TEST_ASSERT_EQ(cpp11Function(fooLegacy), 123);
}
}

int main(int, char**)
{
   TEST_CHECK(testNullCopying);
   TEST_CHECK(testAutoPtrConstructor);
   TEST_CHECK(testAutoPtrReset);
   TEST_CHECK(testCopying);
   TEST_CHECK(testAssigning);
   TEST_CHECK(testSyntax);
   TEST_CHECK(testCasting);
   TEST_CHECK(testStdSharedPtr);

   return 0;
}
