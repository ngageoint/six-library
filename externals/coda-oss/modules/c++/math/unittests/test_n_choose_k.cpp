/* =========================================================================
 * This file is part of math-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * math-c++ is free software; you can redistribute it and/or modify
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

#include <TestCase.h>
#include <math/Utilities.h>

namespace
{
TEST_CASE(testNChooseK)
{
    TEST_ASSERT_EQ(math::nChooseK(0, 0), 1);
    TEST_ASSERT_EQ(math::nChooseK(1, 1), 1);
    TEST_ASSERT_EQ(math::nChooseK(3, 2), 3);
    TEST_ASSERT_EQ(math::nChooseK(10, 3), 120);
}

TEST_CASE(testNLessThanK)
{
    bool exceptionCaught = false;
    try
    {
        math::nChooseK(3, 10);
    }
    catch(const except::Exception& /*exception*/)
    {
       exceptionCaught = true;
    }
    TEST_ASSERT(exceptionCaught);
}
}

int main()
{
    TEST_CHECK(testNChooseK);
    TEST_CHECK(testNLessThanK);
    return 0;
}

