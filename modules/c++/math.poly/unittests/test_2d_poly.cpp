/* =========================================================================
 * This file is part of math.poly-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
            poly[ii][jj] = coeff;
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
            poly[ii][jj] = coeff;
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
    TEST_ASSERT_EQ(truncatedPoly.orderX(), 3);
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
    TEST_ASSERT_EQ(truncatedPoly.orderX(), 3);
    TEST_ASSERT_EQ(truncatedPoly.orderY(), 2);
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
    TEST_ASSERT_EQ(truncatedPoly.orderX(), 0);
    TEST_ASSERT_EQ(truncatedPoly.orderY(), 0);
    TEST_ASSERT_EQ(truncatedPoly[0][0], 0.0);
}
}

int main(int argc, char** argv)
{
    srand(176);
    TEST_CHECK(testScaleVariable);
    TEST_CHECK(testTruncateTo);
    TEST_CHECK(testTruncateToNonZeros);
}
