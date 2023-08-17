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

#include <numpyutils/AutoPtr.h>

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

static void f(const std::string& testName, numpyutils::AutoPtr<Foo> p)
{
    TEST_ASSERT_NOT_NULL(p.get());
    TEST_ASSERT_EQ(123, p->mVal);
}
TEST_CASE(memAutoPtr)
{
    {
        numpyutils::AutoPtr<Foo> fooCtor;
        TEST_ASSERT_NULL(fooCtor.get());

        fooCtor.reset(new Foo(123));
        TEST_ASSERT_NOT_NULL(fooCtor.get());
        TEST_ASSERT_EQ(123, fooCtor->mVal);
    }
    {
        numpyutils::AutoPtr<Foo> fooCtor(new Foo(123));
        TEST_ASSERT_NOT_NULL(fooCtor.get());
        TEST_ASSERT_EQ(123, fooCtor->mVal);
    }
    {
        numpyutils::AutoPtr<Foo> fooCtor(new Foo(123));
        numpyutils::AutoPtr<Foo> other(fooCtor);
        TEST_ASSERT_NOT_NULL(other.get());
        TEST_ASSERT_EQ(123, other->mVal);
    }
    {
        numpyutils::AutoPtr<Foo> fooCtor(new Foo(123));
        f(testName, fooCtor);
        TEST_ASSERT_NULL(fooCtor.get());
    }
    {
        std::unique_ptr<Foo> fooCtor(new Foo(123));
        f(testName, std::move(fooCtor));
        TEST_ASSERT_NULL(fooCtor.get());
    }
}


TEST_MAIN(
   TEST_CHECK(memAutoPtr);
   )
