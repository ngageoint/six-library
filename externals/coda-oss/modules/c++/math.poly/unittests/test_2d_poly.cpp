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

#include <math/poly/TwoD.h>
#include "TestCase.h"

namespace
{
double getRand()
{
    return (50.0 * rand() / RAND_MAX - 25.0);
}

math::poly::TwoD<double> getRandPoly(size_t orderX, size_t orderY)
{
    math::poly::TwoD<double> poly(orderX, orderY);
    for (size_t ii = 0; ii <= orderX; ++ii)
    {
        for (size_t jj = 0; jj <= orderY; ++jj)
        {
            poly[ii][jj] = getRand();
        }
    }

    return poly;
}

void getRandValues(std::vector<double>& xValues,
                   std::vector<double>& yValues)
{
    xValues.resize(100);
    yValues.resize(xValues.size());
    for (size_t ii = 0; ii < xValues.size(); ++ii)
    {
        xValues[ii] = getRand();
        yValues[ii] = getRand();
    }
}

TEST_CASE(testScaleVariable)
{
    std::vector<double> xValues;
    std::vector<double> yValues;
    getRandValues(xValues, yValues);

    math::poly::TwoD<double> poly(getRandPoly(4, 5));

    // transformedPoly = poly(x * scale, y * scale)
    const double scale(13);
    math::poly::TwoD<double> transformedPoly = poly.scaleVariable(scale);

    for (size_t ii = 0; ii < xValues.size(); ++ii)
    {
        // These numbers can get big, so make an epsilon based on a percentage
        // of the expected value
        const double xx(xValues[ii]);
        const double yy(yValues[ii]);
        const double expectedValue(poly(xx * scale, yy * scale));
        TEST_ASSERT_ALMOST_EQ_EPS(transformedPoly(xx, yy),
                                  expectedValue,
                                  std::abs(.01 * expectedValue));
    }

    // transformedPoly = poly(x * scaleX, y * scaleY)
    const double scaleX(3.14);
    const double scaleY(1.26);
    transformedPoly = poly.scaleVariable(scaleX, scaleY);

    for (size_t ii = 0; ii < xValues.size(); ++ii)
    {
        const double xx(xValues[ii]);
        const double yy(yValues[ii]);
        const double expectedValue(poly(xx * scaleX, yy * scaleY));
        TEST_ASSERT_ALMOST_EQ_EPS(transformedPoly(xx, yy),
                                  expectedValue,
                                  std::abs(.01 * expectedValue));
    }
}

TEST_CASE(testTruncateTo)
{
    math::poly::TwoD<double> poly(4, 5);
    for (size_t ii = 0, coeff = 10; ii <= poly.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= poly.orderY(); ++jj, coeff += 10)
        {
            poly[ii][jj] = static_cast<double>(coeff);
        }
    }

    const math::poly::TwoD<double> truncatedPoly = poly.truncateTo(2, 3);
    TEST_ASSERT_EQ(truncatedPoly[0][0], 10.0);
    TEST_ASSERT_EQ(truncatedPoly[0][1], 20.0);
    TEST_ASSERT_EQ(truncatedPoly[0][2], 30.0);
    TEST_ASSERT_EQ(truncatedPoly[0][3], 40.0);
    TEST_ASSERT_EQ(truncatedPoly[1][0], 70.0);
    TEST_ASSERT_EQ(truncatedPoly[1][1], 80.0);
    TEST_ASSERT_EQ(truncatedPoly[1][2], 90.0);
    TEST_ASSERT_EQ(truncatedPoly[1][3], 100.0);
}

TEST_CASE(testTruncateToNonZeros)
{
    math::poly::TwoD<double> poly(4, 5);
    for (size_t ii = 0, coeff = 10; ii <= poly.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= poly.orderY(); ++jj, coeff += 10)
        {
            poly[ii][jj] = static_cast<double>(coeff);
        }
    }

    // Shouldn't truncate anything
    math::poly::TwoD<double> truncatedPoly = poly.truncateToNonZeros(0.0);
    TEST_ASSERT_EQ(truncatedPoly.orderX(), poly.orderX());
    TEST_ASSERT_EQ(truncatedPoly.orderY(), poly.orderY());
    for (size_t ii = 0; ii <= poly.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= poly.orderY(); ++jj)
        {
            TEST_ASSERT_EQ(truncatedPoly[ii][jj], poly[ii][jj]);
        }
    }

    // Still shouldn't truncate anything
    poly[4][5] = 0.0;
    truncatedPoly = poly.truncateToNonZeros(0.0);
    TEST_ASSERT_EQ(truncatedPoly.orderX(), poly.orderX());
    TEST_ASSERT_EQ(truncatedPoly.orderY(), poly.orderY());
    for (size_t ii = 0; ii <= poly.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= poly.orderY(); ++jj)
        {
            TEST_ASSERT_EQ(truncatedPoly[ii][jj], poly[ii][jj]);
        }
    }

    // Now we should lose the bottom row
    poly[3][5] = 0.0;
    for (size_t jj = 0; jj <= poly.orderY(); ++jj)
    {
        poly[4][jj] = 0.0;
    }

    truncatedPoly = poly.truncateToNonZeros(0.0);
    TEST_ASSERT_EQ(truncatedPoly.orderX(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(truncatedPoly.orderY(), poly.orderY());
    for (size_t ii = 0; ii <= truncatedPoly.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= truncatedPoly.orderY(); ++jj)
        {
            TEST_ASSERT_EQ(truncatedPoly[ii][jj], poly[ii][jj]);
        }
    }

    // Now we should lose the right three columns as well
    for (size_t ii = 0; ii <= poly.orderX(); ++ii)
    {
        for (size_t jj = 3; jj <= poly.orderY(); ++jj)
        {
            poly[ii][jj] = 0.0;
        }
    }

    truncatedPoly = poly.truncateToNonZeros(0.0);
    TEST_ASSERT_EQ(truncatedPoly.orderX(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(truncatedPoly.orderY(), static_cast<size_t>(2));
    for (size_t ii = 0; ii <= truncatedPoly.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= truncatedPoly.orderY(); ++jj)
        {
            TEST_ASSERT_EQ(truncatedPoly[ii][jj], poly[ii][jj]);
        }
    }

    // Now we should lose everything, but we'll still have a 0th order
    // polynomial for convenience (more useful than an empty polynomial)
    for (size_t ii = 0; ii <= poly.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= poly.orderY(); ++jj)
        {
            poly[ii][jj] = 0.0;
        }
    }

    truncatedPoly = poly.truncateToNonZeros(0.0);
    TEST_ASSERT_EQ(truncatedPoly.orderX(), static_cast<size_t>(0));
    TEST_ASSERT_EQ(truncatedPoly.orderY(), static_cast<size_t>(0));
    TEST_ASSERT_EQ(truncatedPoly[0][0], 0.0);
}

TEST_CASE(testTransformInput)
{
    std::vector<double> xValues;
    std::vector<double> yValues;
    getRandValues(xValues, yValues);

    math::poly::TwoD<double> poly(getRandPoly(4, 5));
    math::poly::TwoD<double> gx(getRandPoly(5, 3));
    math::poly::TwoD<double> gy(getRandPoly(3, 4));

    math::poly::TwoD<double> transformedPoly = poly.transformInput(gx, gy);

    for (size_t ii = 0; ii < xValues.size(); ++ii)
    {
        // These numbers can get big, so make an epsilon based on a percentage
        // of the expected value
        const double xx(xValues[ii]);
        const double yy(yValues[ii]);
        const double expectedValue(poly(gx(xx, yy), gy(xx, yy)));
        TEST_ASSERT_ALMOST_EQ_EPS(transformedPoly(xx, yy),
                                  expectedValue,
                                  std::abs(.01 * expectedValue));
    }

    transformedPoly = poly.transformInput(gx);
    for (size_t ii = 0; ii < xValues.size(); ++ii)
    {
        // These numbers can get big, so make an epsilon based on a percentage
        // of the expected value
        const double xx(xValues[ii]);
        const double yy(yValues[ii]);
        const double expectedValue(poly(gx(xx, yy), yy));
        TEST_ASSERT_ALMOST_EQ_EPS(transformedPoly(xx, yy),
                                  expectedValue,
                                  std::abs(.01 * expectedValue));
    }
}

TEST_CASE(testOperators)
{
    math::poly::TwoD<double> p1(2, 2);
    math::poly::TwoD<double> p2(1, 3);

    // x^0*(0*y^0 1*y^1 2*y^2 )
    // x^1*(2*y^0 3*y^1 4*y^2 )
    // x^2*(4*y^0 5*y^1 6*y^2 )
    for (size_t ii = 0; ii <= p1.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= p1.orderY(); ++jj)
        {
            p1[ii][jj] = static_cast<double>(ii * p1.orderY() + jj);
        }
    }

    // x^0*(0*y^0 1*y^1 2*y^2 3*y^3 )
    // x^1*(3*y^0 4*y^1 5*y^2 6*y^3 )
    for (size_t ii = 0; ii <= p2.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= p2.orderY(); ++jj)
        {
            p2[ii][jj] = static_cast<double>(ii * p2.orderY() + jj);
        }
    }

    // x^0*(0*y^0 2*y^1 4*y^2 3*y^3 )
    // x^1*(5*y^0 7*y^1 9*y^2 6*y^3 )
    // x^2*(4*y^0 5*y^1 6*y^2 0*y^3 )
    math::poly::TwoD<double> op = p1 + p2;
    math::poly::TwoD<double> opEquals(p1);
    opEquals += p2;

    TEST_ASSERT_EQ(op.orderX(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(op.orderY(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(opEquals.orderX(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(opEquals.orderY(), static_cast<size_t>(3));

    TEST_ASSERT_EQ(op[0][0], 0.0);
    TEST_ASSERT_EQ(op[0][1], 2.0);
    TEST_ASSERT_EQ(op[0][2], 4.0);
    TEST_ASSERT_EQ(op[0][3], 3.0);
    TEST_ASSERT_EQ(op[1][0], 5.0);
    TEST_ASSERT_EQ(op[1][1], 7.0);
    TEST_ASSERT_EQ(op[1][2], 9.0);
    TEST_ASSERT_EQ(op[1][3], 6.0);
    TEST_ASSERT_EQ(op[2][0], 4.0);
    TEST_ASSERT_EQ(op[2][1], 5.0);
    TEST_ASSERT_EQ(op[2][2], 6.0);
    TEST_ASSERT_EQ(op[2][3], 0.0);

    for (size_t ii = 0; ii <= op.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= op.orderY(); ++jj)
        {
            TEST_ASSERT_EQ(op[ii][jj], opEquals[ii][jj]);
        }
    }

    // x^0*( 0*y^0  0*y^1  0*y^2 -3*y^3 )
    // x^1*(-1*y^0 -1*y^1 -1*y^2 -6*y^3 )
    // x^2*( 4*y^0  5*y^1  6*y^2  0*y^3 )
    op = p1 - p2;
    opEquals = p1;
    opEquals -= p2;

    TEST_ASSERT_EQ(op.orderX(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(op.orderY(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(opEquals.orderX(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(opEquals.orderY(), static_cast<size_t>(3));

    TEST_ASSERT_EQ(op[0][0], 0.0);
    TEST_ASSERT_EQ(op[0][1], 0.0);
    TEST_ASSERT_EQ(op[0][2], 0.0);
    TEST_ASSERT_EQ(op[0][3], -3.0);
    TEST_ASSERT_EQ(op[1][0], -1.0);
    TEST_ASSERT_EQ(op[1][1], -1.0);
    TEST_ASSERT_EQ(op[1][2], -1.0);
    TEST_ASSERT_EQ(op[1][3], -6.0);
    TEST_ASSERT_EQ(op[2][0], 4.0);
    TEST_ASSERT_EQ(op[2][1], 5.0);
    TEST_ASSERT_EQ(op[2][2], 6.0);
    TEST_ASSERT_EQ(op[2][3], 0.0);

    for (size_t ii = 0; ii <= op.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= op.orderY(); ++jj)
        {
            TEST_ASSERT_EQ(op[ii][jj], opEquals[ii][jj]);
        }
    }

    // x^0*( 0*y^0  0*y^1  1*y^2  4*y^3  7*y^4  6*y^5 )
    // x^1*( 0*y^0  5*y^1 17*y^2 29*y^3 33*y^4 24*y^5 )
    // x^2*( 6*y^0 21*y^1 47*y^2 71*y^3 65*y^4 42*y^5 )
    // x^3*(12*y^0 31*y^1 58*y^2 73*y^3 60*y^4 36*y^5 )
    op = p1 * p2;
    opEquals = p1;
    opEquals *= p2;

    TEST_ASSERT_EQ(op.orderX(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(op.orderY(), static_cast<size_t>(5));
    TEST_ASSERT_EQ(opEquals.orderX(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(opEquals.orderY(), static_cast<size_t>(5));

    TEST_ASSERT_EQ(op[0][0], 0.0);
    TEST_ASSERT_EQ(op[0][1], 0.0);
    TEST_ASSERT_EQ(op[0][2], 1.0);
    TEST_ASSERT_EQ(op[0][3], 4.0);
    TEST_ASSERT_EQ(op[0][4], 7.0);
    TEST_ASSERT_EQ(op[0][5], 6.0);
    TEST_ASSERT_EQ(op[1][0], 0.0);
    TEST_ASSERT_EQ(op[1][1], 5.0);
    TEST_ASSERT_EQ(op[1][2], 17.0);
    TEST_ASSERT_EQ(op[1][3], 29.0);
    TEST_ASSERT_EQ(op[1][4], 33.0);
    TEST_ASSERT_EQ(op[1][5], 24.0);
    TEST_ASSERT_EQ(op[2][0], 6.0);
    TEST_ASSERT_EQ(op[2][1], 21.0);
    TEST_ASSERT_EQ(op[2][2], 47.0);
    TEST_ASSERT_EQ(op[2][3], 71.0);
    TEST_ASSERT_EQ(op[2][4], 65.0);
    TEST_ASSERT_EQ(op[2][5], 42.0);
    TEST_ASSERT_EQ(op[3][0], 12.0);
    TEST_ASSERT_EQ(op[3][1], 31.0);
    TEST_ASSERT_EQ(op[3][2], 58.0);
    TEST_ASSERT_EQ(op[3][3], 73.0);
    TEST_ASSERT_EQ(op[3][4], 60.0);
    TEST_ASSERT_EQ(op[3][5], 36.0);

    for (size_t ii = 0; ii <= op.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= op.orderY(); ++jj)
        {
            TEST_ASSERT_EQ(op[ii][jj], opEquals[ii][jj]);
        }
    }

    // x^0*(0*y^0 0.5*y^1 1*y^2 )
    // x^1*(1*y^0 1.5*y^1 2*y^2 )
    // x^2*(2*y^0 2.5*y^1 3*y^2 )
    op = p1 / 2.0;
    opEquals = p1;
    opEquals /= 2.0;

    TEST_ASSERT_EQ(op.orderX(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(op.orderY(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(opEquals.orderX(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(opEquals.orderY(), static_cast<size_t>(2));

    TEST_ASSERT_EQ(op[0][0], 0.0);
    TEST_ASSERT_EQ(op[0][1], 0.5);
    TEST_ASSERT_EQ(op[0][2], 1.0);
    TEST_ASSERT_EQ(op[1][0], 1.0);
    TEST_ASSERT_EQ(op[1][1], 1.5);
    TEST_ASSERT_EQ(op[1][2], 2.0);
    TEST_ASSERT_EQ(op[2][0], 2.0);
    TEST_ASSERT_EQ(op[2][1], 2.5);
    TEST_ASSERT_EQ(op[2][2], 3.0);

    for (size_t ii = 0; ii <= op.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= op.orderY(); ++jj)
        {
            TEST_ASSERT_EQ(op[ii][jj], opEquals[ii][jj]);
        }
    }

    TEST_ASSERT_TRUE(op == opEquals);
    TEST_ASSERT_FALSE(op != opEquals);
}

TEST_CASE(testIsScalar)
{
    math::poly::TwoD<double> p1(2, 2);

    // x^0*(0*y^0 1*y^1 2*y^2 )
    // x^1*(2*y^0 3*y^1 4*y^2 )
    // x^2*(4*y^0 5*y^1 6*y^2 )
    for (size_t ii = 0; ii <= p1.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= p1.orderY(); ++jj)
        {
            p1[ii][jj] = static_cast<double>(ii * p1.orderY() + jj);
        }
    }

    TEST_ASSERT_FALSE(p1.isScalar());


    math::poly::TwoD<double> p2(1, 3);
    p2[0][0] = 1;
    TEST_ASSERT(p2.isScalar());
}

TEST_CASE(testAtY)
{
    math::poly::TwoD<double> p1(2, 2);
    // x^0*(1*y^0 2*y^1 3*y^2 )
    // x^1*(3*y^0 4*y^1 5*y^2 )
    // x^2*(5*y^0 6*y^1 7*y^2 )
    for (size_t ii = 0; ii <= p1.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= p1.orderY(); ++jj)
        {
            p1[ii][jj] = static_cast<double>(ii * p1.orderY() + jj + 1);
        }
    }
    TEST_ASSERT_EQ(p1(2, 3), p1.atY(3)(2));

    // 2D polynomials with one dimension having 0 order
    math::poly::TwoD<double> p2(2, 0);
    for (size_t ii = 0; ii <= p2.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= p2.orderY(); ++jj)
        {
            p2[ii][jj] = static_cast<double>(ii * p2.orderY() + jj + 1);
        }
    }
    TEST_ASSERT_EQ(p2.atY(3)(2), p2(2, 3));
    TEST_ASSERT_EQ(p2.flipXY().atY(4)(5), p2(4, 5));

    math::poly::TwoD<double> p3(0, 2);
    for (size_t ii = 0; ii <= p3.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= p3.orderY(); ++jj)
        {
            p3[ii][jj] = static_cast<double>(ii * p3.orderY() + jj + 1);
        }
    }
    TEST_ASSERT_EQ(p3.atY(3)(2), p3(2, 3));
    TEST_ASSERT_EQ(p3.flipXY().atY(4)(5), p3(4, 5));

    // Empty polynomial object
    math::poly::TwoD<double> p4;
    TEST_ASSERT_EQ(p4.atY(3)(2), p4(2, 3));
    TEST_ASSERT_EQ(p4.flipXY().atY(4)(5), p4(4, 5));
}
}

int main(int, char**)
{
    srand(176);
    TEST_CHECK(testScaleVariable);
    TEST_CHECK(testTruncateTo);
    TEST_CHECK(testTruncateToNonZeros);
    TEST_CHECK(testTransformInput);
    TEST_CHECK(testOperators);
    TEST_CHECK(testIsScalar);
    TEST_CHECK(testAtY);
}

