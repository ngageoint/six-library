/* =========================================================================
 * This file is part of math-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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
#include <limits>

TEST_CASE(testZero)
{
   TEST_ASSERT_EQ(math::sign(0), 0);
   TEST_ASSERT_EQ(math::sign(0.0), 0);
}

TEST_CASE(testPositive)
{
    TEST_ASSERT_EQ(math::sign(1), 1);
    TEST_ASSERT_EQ(math::sign(0.3), 1);
    TEST_ASSERT_EQ(math::sign(std::numeric_limits<double>::epsilon()), 1);
}

TEST_CASE(testNegative)
{
    TEST_ASSERT_EQ(math::sign(-std::numeric_limits<double>::epsilon()), -1);
    TEST_ASSERT_EQ(math::sign(-0.1), -1);
}

TEST_MAIN(
    TEST_CHECK(testZero);
    TEST_CHECK(testPositive);
    TEST_CHECK(testNegative);
    )

