/* =========================================================================
 * This file is part of math.linear-c++
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

//#include "/data2/u/muller/coda-oss/modules/c++/math.poly/include/math/poly/OneD.h"
//#include "/data2/u/muller/coda-oss/modules/c++/math.linear/include/math/linear/VectorN.h"
#include <math/poly/OneD.h>
#include <TestCase.h>

namespace
{
TEST_CASE(testInt)
{
    math::poly::OneD<int> poly(2);
    poly[0] = 3;
    poly[1] = 4;
    poly[2] = 5;

    int integral = poly.integrate(0,1);
    TEST_ASSERT_EQ(integral, 6);

    math::poly::OneD<int> derivative = poly.derivative();
    TEST_ASSERT_EQ(derivative[0], 4);
    TEST_ASSERT_EQ(derivative[1], 10);

    poly[2] = 0;
    poly = poly.truncateToNonZeros();
    const auto order = poly.order();
    TEST_ASSERT_EQ(order, static_cast<size_t>(1));
}

TEST_CASE(testDouble)
{
    math::poly::OneD<double> poly(2);
    poly[0] = 3.1;
    poly[1] = 4.2;
    poly[2] = 5.3;

    double integral = poly.integrate(0,1);
    TEST_ASSERT_EQ(integral, 209.0/30.0);

    math::poly::OneD<double> derivative = poly.derivative();
    TEST_ASSERT_EQ(derivative[0], 4.2);
    TEST_ASSERT_EQ(derivative[1], 10.6);

    poly[2] = 0;
    poly = poly.truncateToNonZeros();
    const auto order = poly.order();
    TEST_ASSERT_EQ(order, static_cast<size_t>(1));
}

TEST_CASE(testVector3)
{
    math::poly::OneD< math::linear::VectorN< 3, double > > poly(2);
    math::linear::VectorN< 3, double >& polyCoeffs0 = poly[0];
    polyCoeffs0[0] = 0.0;
    polyCoeffs0[1] = 1.0;
    polyCoeffs0[2] = 2.0;
    math::linear::VectorN< 3, double >& polyCoeffs1 = poly[1];
    polyCoeffs1[0] = 3.0;
    polyCoeffs1[1] = 4.0;
    polyCoeffs1[2] = 5.0;
    math::linear::VectorN< 3, double >& polyCoeffs2 = poly[2];
    polyCoeffs2[0] = 6.0;
    polyCoeffs2[1] = 7.0;
    polyCoeffs2[2] = 8.0;

    math::linear::VectorN< 3, double > integral = poly.integrate(0,1);
    TEST_ASSERT_EQ(integral[0], 3.5);
    TEST_ASSERT_EQ(integral[1], 16.0/3.0);
    TEST_ASSERT_EQ(integral[2], 7.166666666666666);

    math::poly::OneD< math::linear::VectorN< 3, double > > derivative = poly.derivative();
    math::linear::VectorN< 3, double >& derivativeCoeffs0 = derivative[0];
    math::linear::VectorN< 3, double >& derivativeCoeffs1 = derivative[1];
    TEST_ASSERT_EQ(derivativeCoeffs0[0], 3);
    TEST_ASSERT_EQ(derivativeCoeffs0[1], 4);
    TEST_ASSERT_EQ(derivativeCoeffs0[2], 5);
    TEST_ASSERT_EQ(derivativeCoeffs1[0], 12);
    TEST_ASSERT_EQ(derivativeCoeffs1[1], 14);
    TEST_ASSERT_EQ(derivativeCoeffs1[2], 16);

    polyCoeffs2[0] = 0.0;
    polyCoeffs2[1] = 0.0;
    polyCoeffs2[2] = 0.0;
    poly = poly.truncateToNonZeros();
    const auto order = poly.order();
    TEST_ASSERT_EQ(order, static_cast<size_t>(1));
}
}

int main()
{
    TEST_CHECK(testInt);
    TEST_CHECK(testDouble);
    TEST_CHECK(testVector3);
    return 0;
}
