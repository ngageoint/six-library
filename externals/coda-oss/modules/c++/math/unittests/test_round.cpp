/* =========================================================================
 * This file is part of math-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
 *
 * math.linear-c++ is free software; you can redistribute it and/or modify
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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <TestCase.h>
#include <math/Round.h>

namespace
{
TEST_CASE(testFix)
{
    float v1 = 12.3f;
    float expected1 = 12.0f;
    float actual1 = math::fix(v1);
    TEST_ASSERT_ALMOST_EQ(actual1, expected1);

    float v2 = 42.9999f;
    float expected2 = 42.0f;
    float actual2 = math::fix(v2);
    TEST_ASSERT_ALMOST_EQ(actual2, expected2);

    float v3 = -532.0101f;
    float expected3 = -532.0f;
    float actual3 = math::fix(v3);
    TEST_ASSERT_ALMOST_EQ(actual3, expected3);

    float v4 = -23.9999f;
    float expected4 = -23.0f;
    float actual4 = math::fix(v4);
    TEST_ASSERT_ALMOST_EQ(actual4, expected4);

    double v5 = 12.3;
    double expected5 = 12.0;
    double actual5 = math::fix(v5);
    TEST_ASSERT_ALMOST_EQ(actual5, expected5);

    double v6 = 42.9999;
    double expected6 = 42.0;
    double actual6 = math::fix(v6);
    TEST_ASSERT_ALMOST_EQ(actual6, expected6);

    double v7 = -532.0101;
    double expected7 = -532.0;
    double actual7 = math::fix(v7);
    TEST_ASSERT_ALMOST_EQ(actual7, expected7);

    double v8 = -23.9999;
    double expected8 = -23.0;
    double actual8 = math::fix(v8);
    TEST_ASSERT_ALMOST_EQ(actual8, expected8);
}

TEST_CASE(testRound)
{
    float v1 = 12.49999f;
    float expected1 = 12.0f;
    float actual1 = math::round(v1);
    TEST_ASSERT_ALMOST_EQ(actual1, expected1);

    float v2 = 42.5f;
    float expected2 = 43.0f;
    float actual2 = math::round(v2);
    TEST_ASSERT_ALMOST_EQ(actual2, expected2);

    float v3 = -532.4999f;
    float expected3 = -532.0f;
    float actual3 = math::round(v3);
    TEST_ASSERT_ALMOST_EQ(actual3, expected3);

    float v4 = -23.5f;
    float expected4 = -24.0f;
    float actual4 = math::round(v4);
    TEST_ASSERT_ALMOST_EQ(actual4, expected4);

    double v5 = 12.499999999;
    double expected5 = 12.0;
    double actual5 = math::round(v5);
    TEST_ASSERT_ALMOST_EQ(actual5, expected5);

    double v6 = 42.5;
    double expected6 = 43.0;
    double actual6 = math::round(v6);
    TEST_ASSERT_ALMOST_EQ(actual6, expected6);

    double v7 = -532.499999999;
    double expected7 = -532.0;
    double actual7 = math::round(v7);
    TEST_ASSERT_ALMOST_EQ(actual7, expected7);

    double v8 = -23.5;
    double expected8 = -24.0;
    double actual8 = math::round(v8);
    TEST_ASSERT_ALMOST_EQ(actual8, expected8);
}

TEST_CASE(testRoundDigits)
{
    float v1 = 12.499994f;
    float expected1 = 12.49999f;
    float actual1 = math::round(v1, 5);
    TEST_ASSERT_ALMOST_EQ(actual1, expected1);

    float v2 = 42.234567f;
    float expected2 = 42.235f;
    float actual2 = math::round(v2, 3);
    TEST_ASSERT_ALMOST_EQ(actual2, expected2);

    float v3 = -532.499994f;
    float expected3 = -532.49999f;
    float actual3 = math::round(v3, 5);
    TEST_ASSERT_ALMOST_EQ(actual3, expected3);

    float v4 = -23.123456f;
    float expected4 = -23.1235f;
    float actual4 = math::round(v4, 4);
    TEST_ASSERT_ALMOST_EQ(actual4, expected4);

    double v5 = 12.4999999994;
    double expected5 = 12.499999999;
    double actual5 = math::round(v5, 9);
    TEST_ASSERT_ALMOST_EQ(actual5, expected5);

    double v6 = 42.09182736455463;
    double expected6 = 42.0918273646;
    double actual6 = math::round(v6, 10);
    TEST_ASSERT_ALMOST_EQ(actual6, expected6);

    double v7 = -532.499999999994;
    double expected7 = -532.49999999999;
    double actual7 = math::round(v7, 11);
    TEST_ASSERT_ALMOST_EQ(actual7, expected7);

    double v8 = -23.755;
    double expected8 = -23.76;
    double actual8 = math::round(v8, 2);
    TEST_ASSERT_ALMOST_EQ(actual8, expected8);
}

TEST_CASE(testCeilingDivide)
{
    size_t n0 = 0;
    size_t d0 = 1;
    TEST_ASSERT_EQ(math::ceilingDivide(n0, d0), static_cast<size_t>(0));

    size_t n1 = 4;
    size_t d1 = 2;
    TEST_ASSERT_EQ(math::ceilingDivide(n1, d1), static_cast<size_t>(2));

    size_t n2 = 5;
    size_t d2 = 2;
    TEST_ASSERT_EQ(math::ceilingDivide(n2, d2), static_cast<size_t>(3));

    size_t n3 = 1;
    size_t d3 = 0;
    TEST_THROWS(math::ceilingDivide(n3, d3));
}
}

int main()
{
    TEST_CHECK(testFix);
    TEST_CHECK(testRound);
    TEST_CHECK(testRoundDigits);
    TEST_CHECK(testCeilingDivide);
    return 0;
}
