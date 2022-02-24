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
template <typename T>
static void test_degrees_(const std::string& testName)
{
    T sin, cos;
    constexpr units::Degrees<T> degrees_180 = 180;
    SinCos(degrees_180, sin, cos);
    TEST_ASSERT_ALMOST_EQ(0.0, sin);
    TEST_ASSERT_ALMOST_EQ(-1.0, cos);
}
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

    test_degrees_<float>(testName);
    test_degrees_<double>(testName);
    test_degrees_<long double>(testName);
}

TEST_CASE(test_lengths)
{
    {
        constexpr units::Feet<double> feet_3 = 3;
        const auto same = feet_3.to();
        TEST_ASSERT_EQ(same.value(), feet_3.value());

        units::Meters<double> meters{0};
        convert(feet_3, meters);  // convert ...
        TEST_ASSERT_ALMOST_EQ(meters.value(), 0.9144);
        const auto feet = meters.to<units::tags::Feet>();  // ...and back
        TEST_ASSERT_ALMOST_EQ(feet.value(), feet_3.value());
    }
    {
        constexpr auto meters_1 = units::make_Unit<units::tags::Meters>(1.0);
        const auto same = meters_1.to();
        TEST_ASSERT_EQ(same.value(), meters_1.value());

        units::Feet<double> feet{0};
        convert(meters_1, feet);  // convert ...
        TEST_ASSERT_ALMOST_EQ(feet.value(), 3.2808398);
        const auto meters = feet.to<units::tags::Meters>();  // ...and back
        TEST_ASSERT_ALMOST_EQ(meters.value(), meters_1.value());
    }
}

}

int main(int /*argc*/, char** /*argv*/)
{
    TEST_CHECK(test_degrees);
    TEST_CHECK(test_lengths);
    return 0;
}

