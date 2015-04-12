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

#include <math/poly/Fixed1D.h>
#include "TestCase.h"

namespace
{
static const size_t ORDER = 5;
typedef math::poly::Fixed1D<ORDER, double> TestFixed1D;

double getRand()
{
    return (50.0 * rand() / RAND_MAX - 25.0);
}

TestFixed1D getRandPoly()
{
    TestFixed1D poly;
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

    TestFixed1D poly(getRandPoly());

    // transformedPoly = poly(x * scale, y * scale)
    const double scale(13.34);
    TestFixed1D transformedPoly = poly.scaleVariable(scale);

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
}

int main(int argc, char** argv)
{
    srand(176);
    TEST_CHECK(testScaleVariable);
}
