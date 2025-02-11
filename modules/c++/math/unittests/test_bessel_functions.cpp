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
#include <math/Bessel.h>

TEST_CASE(orderZero)
{
    TEST_ASSERT_ALMOST_EQ(math::besselI(0, 1), 1.266065878);
    TEST_ASSERT_ALMOST_EQ(math::besselI(0, 2), 2.279585302);
    TEST_ASSERT_ALMOST_EQ(math::besselI(0, 3), 4.88079259);
}

TEST_CASE(orderOne)
{
    TEST_ASSERT_ALMOST_EQ(math::besselI(1, 4), 9.759465154);
    TEST_ASSERT_ALMOST_EQ_EPS(math::besselI(1, 5), 24.33564214, 1e-5);
    TEST_ASSERT_ALMOST_EQ_EPS(math::besselI(1, 5.6), 42.32828803, 1e-5);
}

TEST_CASE(orderFive)
{
    TEST_ASSERT_ALMOST_EQ(math::besselI(5, -1), -2.71463156e-4);
    TEST_ASSERT_ALMOST_EQ(math::besselI(5, 0), 0);
    TEST_ASSERT_ALMOST_EQ(math::besselI(5, 1), 2.71463156e-4);
}

TEST_MAIN(
    TEST_CHECK(orderZero);
    TEST_CHECK(orderOne);
    TEST_CHECK(orderFive);
    )

