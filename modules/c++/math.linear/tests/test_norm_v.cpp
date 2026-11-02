/* =========================================================================
 * This file is part of math.linear-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <TestCase.h>
#include <math/linear/Vector.h>
#include <math.h>

namespace
{
TEST_CASE(VaryingDimension)
{
    // Test case1: Control variable--components values
    //               Varying the dimensions
    math::linear::Vector<double> v1(1, 2.2); // normSq should be (2.2)^2. norm should be 2.2
    math::linear::Vector<double> v2(2, 2.2); // normSq should be 2 * (2.2)^2, norm should be sqrt of that
    math::linear::Vector<double> v3(3, 2.2); // normSq should be 3 * (2.2)^2, norm should be sqrt of that
    math::linear::Vector<double> v4(50, 2.2); // normSq should be 50 * (2.2)^2, norm should be sqrt of that

    // normSq:
    double expectedNormSq1 = std::pow(2.2, 2.0);
    double actualNormSq1 = v1.normSq();
    TEST_ASSERT_ALMOST_EQ(expectedNormSq1, actualNormSq1);

    double expectedNormSq2 = 2 * expectedNormSq1;
    double actualNormSq2 = v2.normSq();
    TEST_ASSERT_ALMOST_EQ(expectedNormSq2, actualNormSq2);

    double expectedNormSq3 = 3 * expectedNormSq1;
    double actualNormSq3 = v3.normSq();
    TEST_ASSERT_ALMOST_EQ(expectedNormSq3, actualNormSq3);

    double expectedNormSq4 = 50 * expectedNormSq1;
    double actualNormSq4 = v4.normSq();
    TEST_ASSERT_ALMOST_EQ(expectedNormSq4, actualNormSq4);

    // norm:
    double expectedNorm1 = std::sqrt(expectedNormSq1);
    double actualNorm1 = v1.norm();
    TEST_ASSERT_ALMOST_EQ(expectedNorm1, actualNorm1);

    double expectedNorm2 = std::sqrt(expectedNormSq2);
    double actualNorm2 = v2.norm();
    TEST_ASSERT_ALMOST_EQ(expectedNorm2, actualNorm2);

    double expectedNorm3 = std::sqrt(expectedNormSq3);
    double actualNorm3 = v3.norm();
    TEST_ASSERT_ALMOST_EQ(expectedNorm3, actualNorm3);

    double expectedNorm4 = std::sqrt(expectedNormSq4);
    double actualNorm4 = v4.norm();
    TEST_ASSERT_ALMOST_EQ(expectedNorm4, actualNorm4);
}
double manualNormSq(const math::linear::Vector<double>& v)
{
    return (v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]);
}
TEST_CASE(VaryingComponentVal)
{
    // Test case: Control variable--dimension
    //             Varying each components value
    // normSq
    math::linear::Vector<double> v1(3);
    v1[0] = 0; v1[1] = 1; v1[2] = 2; // normSq:
    double expectedNormSq1 = manualNormSq(v1);
    double actualNormSq1 = v1.normSq();
    TEST_ASSERT_ALMOST_EQ(expectedNormSq1, actualNormSq1);

    math::linear::Vector<double> v2(3);
    v2[0] = -3; v2[1] = 39201; v2[2] = 0;
    double expectedNormSq2 = manualNormSq(v2);
    double actualNormSq2 = v2.normSq();
    TEST_ASSERT_ALMOST_EQ(expectedNormSq2, actualNormSq2);

    math::linear::Vector<double> v3(3);
    v3[0] = -4; v3[1] = 14; v3[2] = 92;
    double expectedNormSq3 = manualNormSq(v3);
    double actualNormSq3 = v3.normSq();
    TEST_ASSERT_ALMOST_EQ(expectedNormSq3, actualNormSq3);

    math::linear::Vector<double> v4(3);
    v4[0] = 0; v4[1] = 0; v4[2] = 0;
    double expectedNormSq4 = manualNormSq(v4);
    double actualNormSq4 = v4.normSq();
    TEST_ASSERT_ALMOST_EQ(expectedNormSq4, actualNormSq4);

    // norm
    double expectedNorm1 = std::sqrt(expectedNormSq1);
    double actualNorm1 = v1.norm();
    TEST_ASSERT_ALMOST_EQ(expectedNorm1, actualNorm1);

    double expectedNorm2 = std::sqrt(expectedNormSq2);
    double actualNorm2 = v2.norm();
    TEST_ASSERT_ALMOST_EQ(expectedNorm2, actualNorm2);

    double expectedNorm3 = std::sqrt(expectedNormSq3);
    double actualNorm3 = v3.norm();
    TEST_ASSERT_ALMOST_EQ(expectedNorm3, actualNorm3);

    double expectedNorm4 = std::sqrt(expectedNormSq4);
    double actualNorm4 = v4.norm();
    TEST_ASSERT_ALMOST_EQ(expectedNorm4, actualNorm4);
}
}
int main ()
{
    TEST_CHECK(VaryingDimension);
    TEST_CHECK(VaryingComponentVal);
    return 0;
}

