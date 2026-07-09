/* =========================================================================
 * This file is part of math.poly-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * math.poly-c++ is free software; you can redistribute it and/or modify
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

#include <stdlib.h>
#include <tuple>

#include <math/poly/Fixed1D.h>
#include "TestCase.h"

template<size_t ORDER>
using Fixed1D = math::poly::Fixed1D<ORDER, double>;

double getRand()
{
    static const auto call_srand = [](){ srand(176); return true; };
    static auto srand_called = call_srand();
    std::ignore = srand_called;
    return (50.0 * rand() / RAND_MAX - 25.0);
}

template<size_t ORDER>
Fixed1D<ORDER> getRandPoly()
{
    Fixed1D<ORDER> poly;
    for (size_t ii = 0; ii <= ORDER; ++ii)
    {
        poly[ii] = getRand();
    }

    return poly;
}

void getRandValues(std::vector<double>& value)
{
    value.resize(100);
    for (size_t ii = 0; ii < value.size(); ++ii)
    {
        value[ii] = getRand();
    }
}

TEST_CASE(testScaleVariable)
{
    std::vector<double> value;
    getRandValues(value);

    auto poly(getRandPoly<5>());

    // transformedPoly = poly(x * scale, y * scale)
    const double scale(13.34);
    auto transformedPoly = poly.scaleVariable(scale);

    for (size_t ii = 0; ii < value.size(); ++ii)
    {
        // These numbers can get big, so make an epsilon based on a percentage
        // of the expected value
        const double val(value[ii]);
        const double expectedValue(poly(val * scale));
        TEST_ASSERT_ALMOST_EQ_EPS(transformedPoly(val),
                                  expectedValue,
                                  std::abs(.01 * expectedValue));
    }
}

TEST_CASE(testVelocity)
{
    std::vector<double> values;
    getRandValues(values);

    // Constant poly should have 0 velocity
    auto constPoly(getRandPoly<0>());
    for (const auto& val: values)
    {
        TEST_ASSERT_EQ(constPoly.velocity(val), 0.0);
    }

    // Linear poly should have constant velocity
    auto linearPoly(getRandPoly<1>());
    for (const auto& val: values)
    {
        TEST_ASSERT_EQ(linearPoly.velocity(val), linearPoly[1]);
    }

    // Check quadratic and cubic against the derivative
    auto quadraticPoly(getRandPoly<2>());
    for (const auto& val: values)
    {
        TEST_ASSERT_EQ(quadraticPoly.velocity(val), quadraticPoly.derivative()(val));
    }

    auto cubicPoly(getRandPoly<3>());
    for (const auto& val: values)
    {
        TEST_ASSERT_EQ(cubicPoly.velocity(val), cubicPoly.derivative()(val));
    }
}

TEST_CASE(testAcceleration)
{
    std::vector<double> values;
    getRandValues(values);

    // Constant and linear polys should have 0 acceleration
    auto constPoly(getRandPoly<0>());
    for (const auto& val: values)
    {
        TEST_ASSERT_EQ(constPoly.acceleration(val), 0.0);
    }

    auto linearPoly(getRandPoly<1>());
    for (const auto& val: values)
    {
        TEST_ASSERT_EQ(linearPoly.acceleration(val), 0.0);
    }

    // Quadratic poly should have constant acceleration
    auto quadraticPoly(getRandPoly<2>());
    for (const auto& val: values)
    {
        TEST_ASSERT_EQ(quadraticPoly.acceleration(val), 2 * quadraticPoly[2]);
    }
    
    // Check cubic and quartic against the 2nd derivative
    auto cubicPoly(getRandPoly<3>());
    for (const auto& val: values)
    {
        TEST_ASSERT_EQ(cubicPoly.acceleration(val), cubicPoly.derivative().derivative()(val));
    }

    auto quarticPoly(getRandPoly<4>());
    for (const auto& val: values)
    {
        TEST_ASSERT_EQ(quarticPoly.acceleration(val), quarticPoly.derivative().derivative()(val));
    }
}

TEST_MAIN(
    TEST_CHECK(testScaleVariable);
    TEST_CHECK(testVelocity);
    TEST_CHECK(testAcceleration);
)
