/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * sys-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.

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

#include <limits>
#include "TestCase.h"

TEST_CASE(testNaNsAreNotEqual)
{
    // This test exists mainly to document behavior
    // It's not awesome that things work this way, but presumably
    // the caller is testing against a known value, so if this comes up
    // NaN oddness is already expected.
    TEST_ASSERT_NOT_EQ(std::numeric_limits<float>::quiet_NaN(),
        std::numeric_limits<float>::quiet_NaN());

    TEST_ASSERT_NOT_EQ(std::numeric_limits<float>::quiet_NaN(), 3.4);

}

TEST_CASE(testNaNIsNotAlmostEqualToNumber)
{
    // Uncomment the test to see it work.
    // These test macros are supposed to fail here.
    // But I don't have a way to intercept the failure.
    //
    /*
    TEST_ASSERT_ALMOST_EQ(std::numeric_limits<float>::quiet_NaN(), 5);
    TEST_ASSERT_ALMOST_EQ_EPS(std::numeric_limits<float>::quiet_NaN(),
                5, 3);
    */
}

TEST_CASE(testIsNaN)
{
    TEST_ASSERT_TRUE(IS_NAN(std::numeric_limits<float>::quiet_NaN()));
    int five = 5; // "conditional expression is constant"
    TEST_ASSERT_FALSE(IS_NAN(five));
    TEST_ASSERT_FALSE(IS_NAN(std::string("test string")));
}

TEST_MAIN(
    TEST_CHECK(testNaNsAreNotEqual);
    TEST_CHECK(testNaNIsNotAlmostEqualToNumber);
    TEST_CHECK(testIsNaN);
)

