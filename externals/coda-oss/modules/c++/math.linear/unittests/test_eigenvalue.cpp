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

#include "TestCase.h"
#include <math/linear/Eigenvalue.h>

namespace
{
typedef math::linear::Matrix2D<double> Matrix;
typedef math::linear::Vector<double> Vector;
typedef math::linear::Eigenvalue<double> Eigenvalue;

TEST_CASE(testNonSymmetric)
{
    Matrix A(4, 4);
    A[0][0] = 28;  A[0][1] = 69;  A[0][2] = 44;  A[0][3] = 19;
    A[1][0] = 5;   A[1][1] = 32;  A[1][2] = 38;  A[1][3] = 49;
    A[2][0] = 10;  A[2][1] = 95;  A[2][2] = 77;  A[2][3] = 45;
    A[3][0] = 82;  A[3][1] = 3;   A[3][2] = 80;  A[3][3] = 65;

    Eigenvalue eig(A);

    const Matrix& V(eig.getV());
    Matrix D;
    eig.getD(D);

    // It should be the case that A*V == V*D
    const Matrix AV = A * V;
    const Matrix VD = V * D;

    TEST_ASSERT_EQ(AV.rows(), VD.rows());
    TEST_ASSERT_EQ(AV.cols(), VD.cols());

    for (size_t row = 0; row < AV.rows(); ++row)
    {
        for (size_t col = 0; col < VD.cols(); ++col)
        {
            TEST_ASSERT_ALMOST_EQ(AV[row][col], VD[row][col]);
        }
    }
}

TEST_CASE(testSymmetric)
{
    Matrix A(4, 4);
    A[0][0] = 28;  A[0][1] = 69;  A[0][2] = 44;  A[0][3] = 82;
    A[1][0] = 69;  A[1][1] = 32;  A[1][2] = 38;  A[1][3] = 49;
    A[2][0] = 44;  A[2][1] = 38;  A[2][2] = 77;  A[2][3] = 45;
    A[3][0] = 82;  A[3][1] = 49;  A[3][2] = 45;  A[3][3] = 65;

    Eigenvalue eig(A);

    const Matrix& V(eig.getV());
    Matrix D;
    eig.getD(D);

    // It should be the case that A*V == V*D
    const Matrix AV = A * V;
    const Matrix VD = V * D;

    TEST_ASSERT_EQ(AV.rows(), VD.rows());
    TEST_ASSERT_EQ(AV.cols(), VD.cols());

    for (size_t row = 0; row < AV.rows(); ++row)
    {
        for (size_t col = 0; col < VD.cols(); ++col)
        {
            TEST_ASSERT_ALMOST_EQ(AV[row][col], VD[row][col]);
        }
    }
}
}

int main(int, char**)
{
    TEST_CHECK(testNonSymmetric);
    TEST_CHECK(testSymmetric);
    return 0;
}
