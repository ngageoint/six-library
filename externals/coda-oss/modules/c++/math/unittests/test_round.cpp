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
    float v1 = 12.3;
    float expected1 = 12.0;
    float actual1 = math::fix(v1);
    TEST_ASSERT_ALMOST_EQ(actual1, expected1);

    float v2 = 42.9999;
    float expected2 = 42.0;
    float actual2 = math::fix(v2);
    TEST_ASSERT_ALMOST_EQ(actual2, expected2);

    float v3 = -532.0101;
    float expected3 = -532.0;
    float actual3 = math::fix(v3);
    TEST_ASSERT_ALMOST_EQ(actual3, expected3);

    float v4 = -23.9999;
    float expected4 = -23.0;
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
    float v1 = 12.49999;
    float expected1 = 12.0;
    float actual1 = math::round(v1);
    TEST_ASSERT_ALMOST_EQ(actual1, expected1);

    float v2 = 42.5;
    float expected2 = 43.0;
    float actual2 = math::round(v2);
    TEST_ASSERT_ALMOST_EQ(actual2, expected2);

    float v3 = -532.4999;
    float expected3 = -532.0;
    float actual3 = math::round(v3);
    TEST_ASSERT_ALMOST_EQ(actual3, expected3);

    float v4 = -23.5;
    float expected4 = -24.0;
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
    float v1 = 12.499994;
    float expected1 = 12.49999;
    float actual1 = math::round(v1, 5);
    TEST_ASSERT_ALMOST_EQ(actual1, expected1);

    float v2 = 42.234567;
    float expected2 = 42.235;
    float actual2 = math::round(v2, 3);
    TEST_ASSERT_ALMOST_EQ(actual2, expected2);

    float v3 = -532.499994;
    float expected3 = -532.49999;
    float actual3 = math::round(v3, 5);
    TEST_ASSERT_ALMOST_EQ(actual3, expected3);

    float v4 = -23.123456;
    float expected4 = -23.1235;
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
}

int main()
{
    TEST_CHECK(testFix);
    TEST_CHECK(testRound);
    TEST_CHECK(testRoundDigits);
    return 0;
}
