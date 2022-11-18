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

#include <math/poly/Fixed2D.h>
#include "TestCase.h"

static const size_t ORDER_X = 4;
static const size_t ORDER_Y = 5;
typedef math::poly::Fixed2D<ORDER_X, ORDER_Y, double> TestFixed2D;

double getRand()
{
    static const auto call_srand = [](){ srand(176); return true; };
    static auto srand_called = call_srand();
    std::ignore = srand_called;
    return (50.0 * rand() / RAND_MAX - 25.0);
}

TestFixed2D getRandPoly()
{
    TestFixed2D poly;
    for (size_t ii = 0; ii <= ORDER_X; ++ii)
    {
        for (size_t jj = 0; jj <= ORDER_Y; ++jj)
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

    TestFixed2D poly(getRandPoly());

    // transformedPoly = poly(x * scale, y * scale)
    const double scale(13);
    TestFixed2D transformedPoly = poly.scaleVariable(scale);

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

TEST_MAIN(
    TEST_CHECK(testScaleVariable);
)
