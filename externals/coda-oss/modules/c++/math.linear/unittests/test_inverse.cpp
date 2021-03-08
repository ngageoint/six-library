/* =========================================================================
 * This file is part of math.linear-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

#include <math/linear/Matrix2D.h>
#include "TestCase.h"

namespace
{
using math::linear::Matrix2D;

TEST_CASE(testLeftRealProperties)
{
    Matrix2D<double> matrix(4, 2);
    matrix(0, 0) = 2;
    matrix(0, 1) = 11.3;
    matrix(1, 0) = 1;
    matrix(1, 1) = 0.004;
    matrix(2, 0) = 5;
    matrix(2, 1) = 8;
    matrix(3, 0) = 1.1;
    matrix(3, 1) = 3.4;
    const Matrix2D<double> inverse = math::linear::leftInverse(matrix);

    // Identities that should hold
    TEST_ASSERT_EQ(matrix, matrix * inverse * matrix);
    TEST_ASSERT_EQ(inverse, inverse * matrix * inverse);
    TEST_ASSERT_EQ(matrix, math::linear::rightInverse(inverse));

    // We want this to be able to solve linear equations
    Matrix2D<double> xx(2, 1);
    xx(0, 0) = 1;
    xx(1, 0) = 5;

    const Matrix2D<double> yy = matrix * xx;
    TEST_ASSERT_EQ(inverse * yy, xx);
}

TEST_CASE(testRightRealProperties)
{
    Matrix2D<double> matrix(4, 2);
    matrix(0, 0) = 2;
    matrix(0, 1) = 11.3;
    matrix(1, 0) = 1;
    matrix(1, 1) = 0.004;
    matrix(2, 0) = 5;
    matrix(2, 1) = 8;
    matrix(3, 0) = 1.1;
    matrix(3, 1) = 3.4;

    // A matrix is right-invertible iff its transpose is left-invertible
    matrix = matrix.transpose();

    // Identities that should hold
    const Matrix2D<double> inverse = math::linear::rightInverse(matrix);
    TEST_ASSERT_EQ(matrix, matrix * inverse * matrix);
    TEST_ASSERT_EQ(inverse, inverse * matrix * inverse);
    TEST_ASSERT_EQ(matrix, math::linear::leftInverse(inverse));

    Matrix2D<double> xx(1, 2);
    xx(0, 0) = 1;
    xx(0, 1) = 5;

    // We want this to be able to solve linear equations
    const Matrix2D<double> yy = xx * matrix;
    TEST_ASSERT_EQ(yy * inverse, xx);
}
}

int main(int /*argc*/, char** /*argv*/)
{
    TEST_CHECK(testLeftRealProperties);
    TEST_CHECK(testRightRealProperties);
    return 0;
}

