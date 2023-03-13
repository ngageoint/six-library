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

#include <std/memory>

#include <mem/SharedPtr.h>

#include "TestCase.h"

struct Foo final
{
    Foo() = default;
    Foo(int val) :
        mVal(val)
    {
    }

    int mVal = 0;
};

TEST_CASE(testStdUniquePtr)
{
    {
        std::unique_ptr<Foo> fooCtor;
        TEST_ASSERT_NULL(fooCtor.get());

        fooCtor.reset(new Foo(123));
        TEST_ASSERT_NOT_EQ(nullptr, fooCtor.get());
        TEST_ASSERT_EQ(123, fooCtor->mVal);
    }
    {
        auto fooCtor = coda_oss::make_unique<Foo>(123);
        TEST_ASSERT_NOT_EQ(nullptr, fooCtor.get());
        TEST_ASSERT_EQ(123, fooCtor->mVal);
    }
    {
        auto pFoos = coda_oss::make_unique<Foo[]>(123);  // 123 instances of Foo
        TEST_ASSERT_NOT_EQ(nullptr, pFoos.get());
        TEST_ASSERT_EQ(0, pFoos[0].mVal);
        TEST_ASSERT_EQ(0, pFoos[122].mVal);
    }
}

TEST_CASE(test_make_unique)
{
    {
        auto fooCtor = coda_oss::make_unique<Foo>(123);
        TEST_ASSERT_NOT_EQ(nullptr, fooCtor.get());
        TEST_ASSERT_EQ(123, fooCtor->mVal);
    }
    {
        auto pFoos = coda_oss::make_unique<Foo[]>(123);  // 123 instances of Foo
        TEST_ASSERT_NOT_EQ(nullptr, pFoos.get());
        TEST_ASSERT_EQ(0, pFoos[0].mVal);
        TEST_ASSERT_EQ(0, pFoos[122].mVal);
    }

    {
        auto fooCtor = std::make_unique<Foo>(123);
        TEST_ASSERT_NOT_EQ(nullptr, fooCtor.get());
        TEST_ASSERT_EQ(123, fooCtor->mVal);
    }
    {
        auto pFoos = std::make_unique<Foo[]>(123);  // 123 instances of Foo
        TEST_ASSERT_NOT_EQ(nullptr, pFoos.get());
        TEST_ASSERT_EQ(0, pFoos[0].mVal);
        TEST_ASSERT_EQ(0, pFoos[122].mVal);
    }
}

TEST_MAIN(
   TEST_CHECK(testStdUniquePtr);
   TEST_CHECK(test_make_unique);
   )
