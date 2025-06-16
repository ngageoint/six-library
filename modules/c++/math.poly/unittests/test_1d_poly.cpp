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

#include <math/poly/OneD.h>
#include "TestCase.h"

double getRand()
{
    static const auto call_srand = [](){ srand(176); return true; };
    static auto srand_called = call_srand();
    std::ignore = srand_called;
    return (50.0 * rand() / RAND_MAX - 25.0);
}

math::poly::OneD<double> getRandPoly(size_t order)
{
    math::poly::OneD<double> poly(order);
    for (size_t ii = 0; ii <= order; ++ii)
    {
        poly[ii] = getRand();
    }

    return poly;
}

void getRandValues(std::vector<double>& values)
{
    values.resize(100);
    for (size_t ii = 0; ii < values.size(); ++ii)
    {
        values[ii] = getRand();
        values[ii] = getRand();
    }
}

TEST_CASE(testScaleVariable)
{
    std::vector<double> values;
    getRandValues(values);

    math::poly::OneD<double> poly(getRandPoly(4));

    // transformedPoly = poly(x * scale, y * scale)
    const double scale(13.34);
    math::poly::OneD<double> transformedPoly = poly.scaleVariable(scale);

    for (size_t ii = 0; ii < values.size(); ++ii)
    {
        // These numbers can get big, so make an epsilon based on a percentage
        // of the expected value
        const double val(values[ii]);
        const double expectedValue(poly(val * scale));
        TEST_ASSERT_ALMOST_EQ_EPS(transformedPoly(val),
                                  expectedValue,
                                  std::abs(.01 * expectedValue));
    }
}

TEST_CASE(testTruncateTo)
{
    math::poly::OneD<double> poly(5);
    for (size_t ii = 0, coeff = 10; ii <= poly.order(); ++ii, coeff += 10)
    {
        poly[ii] = static_cast<double>(coeff);
    }

    const math::poly::OneD<double> truncatedPoly = poly.truncateTo(2);
    TEST_ASSERT_EQ(truncatedPoly.order(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(truncatedPoly[0], 10.0);
    TEST_ASSERT_EQ(truncatedPoly[1], 20.0);
    TEST_ASSERT_EQ(truncatedPoly[2], 30.0);
}

TEST_CASE(testTruncateToNonZeros)
{
    math::poly::OneD<double> poly(5);
    for (size_t ii = 0, coeff = 10; ii <= poly.order(); ++ii, coeff += 10)
    {
        poly[ii] = static_cast<double>(coeff);
    }

    // Shouldn't truncate anything
    {
        math::poly::OneD<double> truncatedPoly = poly.truncateToNonZeros(0.0);
        TEST_ASSERT_EQ(truncatedPoly.order(), poly.order());
        for (size_t ii = 0; ii <= poly.order(); ++ii)
        {
            TEST_ASSERT_EQ(truncatedPoly[ii], poly[ii]);
        }
    }

    // Truncate one piece at a time
    for (size_t ii = 0, ord = poly.order(); ii < poly.order(); ++ii, --ord)
    {
        poly[ord] = 0.0;
        math::poly::OneD<double> truncatedPoly = poly.truncateToNonZeros(0.0);
        TEST_ASSERT_EQ(truncatedPoly.order(), static_cast<size_t>(ord - 1));
        for (size_t jj = 0; jj < ord; ++jj)
        {
            TEST_ASSERT_EQ(truncatedPoly[jj], poly[jj]);
        }
    }
}

TEST_CASE(testTransformInput)
{
    std::vector<double> values;
    getRandValues(values);

    math::poly::OneD<double> poly(getRandPoly(4));
    math::poly::OneD<double> gx(getRandPoly(5));

    math::poly::OneD<double> transformedPoly = poly.transformInput(gx);

    for (size_t ii = 0; ii < values.size(); ++ii)
    {
        // These numbers can get big, so make an epsilon based on a percentage
        // of the expected value
        const double val(values[ii]);
        const double expectedValue(poly(gx(val)));
        TEST_ASSERT_ALMOST_EQ_EPS(transformedPoly(val),
                                  expectedValue,
                                  std::abs(.01 * expectedValue));
    }
}

TEST_MAIN(
    TEST_CHECK(testScaleVariable);
    TEST_CHECK(testTruncateTo);
    TEST_CHECK(testTruncateToNonZeros);
    TEST_CHECK(testTransformInput);
    )
