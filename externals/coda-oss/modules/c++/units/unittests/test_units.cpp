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
#include <units/Angles.h>
#include <units/Lengths.h>

namespace
{
TEST_CASE(test_degrees)
{
    constexpr units::Degrees<double> degrees_0 = 0;
    constexpr units::Degrees<double> degrees_90 = 90;
    constexpr units::Degrees<double> degrees_180 = 180;
    constexpr units::Degrees<double> degrees_270 = 270;
    constexpr units::Degrees<double> degrees_360 = 360;

    TEST_ASSERT_ALMOST_EQ(0.0, sin(degrees_0));
    TEST_ASSERT_ALMOST_EQ(1.0, sin(degrees_90));
    TEST_ASSERT_ALMOST_EQ(0.0, sin(degrees_180));
    TEST_ASSERT_ALMOST_EQ(-1.0, sin(degrees_270));
    TEST_ASSERT_ALMOST_EQ(0.0, sin(degrees_360));

    TEST_ASSERT_ALMOST_EQ(1.0, cos(degrees_0));
    TEST_ASSERT_ALMOST_EQ(0.0, cos(degrees_90));
    TEST_ASSERT_ALMOST_EQ(-1.0, cos(degrees_180));
    TEST_ASSERT_ALMOST_EQ(0.0, cos(degrees_270));
    TEST_ASSERT_ALMOST_EQ(1.0, cos(degrees_360));
}

TEST_CASE(test_lengths)
{
    constexpr units::Feet<double> feet_3 = 3;
    units::Meters<double> meters { 0 };
    convert(feet_3, meters);
    TEST_ASSERT_ALMOST_EQ(meters.value(), 0.9144);

    constexpr units::Meters<double> meters_1 = 1;
    units::Feet<double> feet { 0 };
    convert(meters_1, feet);
    TEST_ASSERT_ALMOST_EQ(feet.value(), 3.2808398);
}

}

int main(int /*argc*/, char** /*argv*/)
{
    TEST_CHECK(test_degrees);
    TEST_CHECK(test_lengths);
    return 0;
}

