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
#include <import/math/linear.h>
#include "TestCase.h"

using namespace math::linear;

typedef MatrixMxN<2, 2> Matrix2x2;
typedef MatrixMxN<3, 2> Matrix3x2;
typedef MatrixMxN<3, 3> Matrix3x3;
typedef MatrixMxN<4, 4> Matrix4x4;
typedef MatrixMxN<5, 5> Matrix5x5;

#define foreach_ij(M, N) \
   for (unsigned int i = 0; i < M; ++i) \
        for (unsigned int j = 0; j < N; ++j)

TEST_CASE(testIdentityMxN)
{
    Matrix3x3 A = identityMatrix<3, double>();
    foreach_ij(3, 3)
    {
        if (i == j)
        {
            TEST_ASSERT_ALMOST_EQ(A(i, j), 1);
        }
        else
        {
            TEST_ASSERT_ALMOST_EQ(A(i, j), 0);
        }
    }
    Matrix4x4 B = identityMatrix<4, double>();
    foreach_ij(4, 4)
    {
        if (i == j)
        {
            TEST_ASSERT_ALMOST_EQ(B(i, j), 1);
        }
        else
        {
            TEST_ASSERT_ALMOST_EQ(B(i, j), 0);
        }
    }
    Matrix2D<> A2 = identityMatrix<double>(3);
    TEST_ASSERT_EQ(A, A2);

}

TEST_CASE(testScaleMultiplyMxN)
{
    Matrix3x3 A = identityMatrix<3, double>();
    Matrix3x2 B = constantMatrix<3, 2, double>(1);
 
    B.scale(0.5);

    foreach_ij(3, 2)
    {
        TEST_ASSERT_ALMOST_EQ(B(i, j), 0.5);
    }
    
    Matrix3x2 C = A * B;
    TEST_ASSERT_EQ(B, C);
}

TEST_CASE(testNegateMxN)
{
    const Matrix3x3 A = identityMatrix<3, double>();
    const Matrix3x3 B = -A;
    Matrix3x3 C = A;
    for (size_t ii = 0; ii < A.rows(); ++ii)
    {
        for (size_t jj = 0; jj < A.cols(); ++jj)
        {
            C[ii][jj] = -C[ii][jj];
        }
    }
    TEST_ASSERT_EQ(B, C);
}

TEST_CASE(testNegate)
{
    const Matrix2D<double> A = identityMatrix<3, double>();
    const Matrix2D<double> B = -A;
    Matrix2D<double> C = A;
    for (size_t ii = 0; ii < A.rows(); ++ii)
    {
        for (size_t jj = 0; jj < A.cols(); ++jj)
        {
            C[ii][jj] = -C[ii][jj];
        }
    }
    TEST_ASSERT_EQ(B, C);
}

TEST_CASE(testInvert2x2Complex)
{
    std::complex<double> q[4];
    q[0] = std::complex<double>(-0.326773864841957, -0.326773864841957);
    q[1] = std::complex<double>( 0.945102555946311,  0.945102555946311);
    q[2] = std::complex<double>( 0.945102555946311,  0.945102555946311);
    q[3] = std::complex<double>( 0.326773864841957,  0.326773864841957);

    //! TODO: Verify these numbers with an external tool --
    //        These numbers are inbred and a result of the
    //        module itself.
    std::complex<double> correct[4];
    correct[0] = std::complex<double>(-0.163387,  0.163387);
    correct[1] = std::complex<double>( 0.472551, -0.472551);
    correct[2] = std::complex<double>( 0.472551, -0.472551);
    correct[3] = std::complex<double>( 0.163387, -0.163387);

    MatrixMxN<2, 2, std::complex<double> > Q(q);

    MatrixMxN<2, 2, std::complex<double> > Qinv = inverse(Q);

    MatrixMxN<2, 2, std::complex<double> > C(correct);

    foreach_ij(2, 2)
    {
        TEST_ASSERT_ALMOST_EQ_EPS(std::abs(Qinv(i, j)), std::abs(C(i, j)), .0001);
    }
}
TEST_CASE(testInvert2x2)
{
    double q[] = 
    {
        -0.326773864841957, 0.945102555946311,
         0.945102555946311, 0.326773864841957
    };
    double correct[] =
    {
        -0.326773864841957, 0.945102555946311,
         0.945102555946311, 0.326773864841957
    };
    Matrix2x2 Q(q);

    Matrix2x2 Qinv = inverse(Q);

    Matrix2x2 C(correct);

    foreach_ij(2, 2)
    {
        TEST_ASSERT_ALMOST_EQ_EPS(Qinv(i, j), C(i, j), .0001);
    }
}
TEST_CASE(testInvert3x3)
{
    double q[] =
    { 
        -0.8335, -0.3467, 0.4302,
         0.5354, -0.3149, 0.7837,
        -0.1362,  0.8835, 0.4481
    };

    double correct[] =
    {
        -0.833535710413543,  0.535408997517066, -0.136218298488716,
        -0.346676224975467, -0.314923792289617,  0.883537548770105,
         0.430155570116382,  0.783683743883780,  0.448113797009368
    };

    Matrix3x3 Q(q);

    Matrix3x3 Qinv = inverse(Q);

    Matrix3x3 C(correct);

    foreach_ij(3, 3)
    {
        TEST_ASSERT_ALMOST_EQ_EPS(Qinv(i, j), C(i, j), .0001);
    }
  
}

TEST_CASE(testInvert4x4)
{

    double q[] = 
    {
        0.537004, -0.291456, -0.769743,  0.184864,
        0.377207,  0.812143, -0.145395, -0.420713,
        0.678185,  0.025277,  0.573694,  0.458585,
        0.330765, -0.504816,  0.239229, -0.760608
    };
    double qinv[] =
    {
         0.537004,   0.377207,  0.678185,  0.330765,
        -0.291456,   0.812143,  0.025277, -0.504816,
        -0.769743,  -0.145395,  0.573694,  0.239229,
         0.184864,  -0.420713,  0.458585, -0.760608
        
    };


    Matrix4x4 Q(q);
    Matrix4x4 Qinv = inverse<4, double>(Q);

    Matrix2D<> Q2(4, 4, q);
    Matrix2D<> Q2inv = inverse<double>(Q2);

    TEST_ASSERT_EQ(Qinv, Q2inv);

    Matrix4x4 Qtruth(qinv);

    foreach_ij(4, 4)
    {
        TEST_ASSERT_ALMOST_EQ_EPS(Qinv(i, j), Qtruth(i, j), 0.00001);
    }

    double m[] =
    {
        9,        7.34847,  7.34847,  6,
        7.34847,  9,        6,        7.34847,
        7.34847,  6,        9,        7.34847,
        6,        7.34847,  7.34847,  9
    };
    double minv[] =
    {
        1.0000,  -0.8165,  -0.8165,   0.6667,
       -0.8165,   1.0000,   0.6667,  -0.8165,
       -0.8165,   0.6667,   1.0000,  -0.8165,
        0.6667,  -0.8165,  -0.8165,   1.0000
    };
    MatrixMxN<4, 4> M(m);
    MatrixMxN<4, 4> Mtruth(minv);
    MatrixMxN<4, 4> Minv = inverse<4, double>(M);
    foreach_ij(4, 4)
    {
        TEST_ASSERT_ALMOST_EQ_EPS(Minv(i, j), Mtruth(i, j), 0.0001);
    }

}

TEST_CASE(testOrthoTranspose5x5)
{
    double q[] =
    {
         0.206598102328096,  -0.258732861775711, -0.366144101439697,  0.808267284061313,  0.320962653355259,
         0.079760655075890,  0.923824193884886,  0.107508570320437,  0.351779072204311,  -0.069860492136368,
        -0.209804190945734, -0.239598615251375,  0.855978685620244,  0.398355064550789, -0.084784652749218,
        -0.755232695854446, -0.020059148734545, -0.347740376127862,  0.229811055794963, -0.505454958536692,
        -0.580138982133012,  0.147635697417530,  0.027523169564286, -0.107030850956001,  0.793365826298614
    };

    Matrix5x5 Q(q);

    Matrix5x5 Qt = Q.transpose();

    Matrix5x5 I = identityMatrix<5, double>();

    Matrix5x5 sanitized = tidy(Qt * Q, 0.0001);
    TEST_ASSERT_EQ(sanitized, I);
   
    sanitized = tidy(Q * Qt, 0.0001);
    TEST_ASSERT_EQ(sanitized, I);

    // Now do the same for the Matrix2D<>
    Matrix2D<> Q2(5, 5, q);
    Matrix2D<> Q2t = Q2.transpose();

    Matrix2D<> sanitized2 = tidy(Q2 * Q2t, 0.0001);

    TEST_ASSERT_EQ(sanitized, sanitized2);

}

TEST_CASE(testInvert5x5)
{
    double q[] =
    {
        -0.1,  -0.0,   0.6,  -0.3,   0.6,
        -0.2,   0.6,  -0.1,   0.5,   0.4,
        -0.9,   0.0,   0.0,  -0.2,  -0.3,
         0.3,   0.7,  -0.0,  -0.5,  -0.1,
        -0.0,  -0.2,  -0.7,  -0.3,   0.4
    };

    double correct[] =
    { 
        -0.165907, -0.304698, -0.947308,  0.232976, -0.098678,
        -0.115846,  0.667319,  0.131571,  0.800976, -0.194628,
         0.654964,  -0.199442, -0.051345, -0.068673, -0.838682,
        -0.416854,  0.656088, -0.329889, -0.691226, -0.451030,
         0.775624,   0.476702, -0.271485, -0.238111,  0.596720
    };
    Matrix5x5 Q(q);

    Matrix5x5 C(correct);
    Matrix5x5 Qinv = inverse(Q);
    foreach_ij(5, 5)
    {
        TEST_ASSERT_ALMOST_EQ_EPS(Qinv(i, j), C(i, j), .00001);
    }
    
    
    Matrix5x5 M = tidy(Q * Qinv, 0.05);
    Matrix5x5 I = identityMatrix<5, double>();
    TEST_ASSERT_EQ(M, I);

    Matrix2D<> Q2(5, 5, q);
    foreach_ij(5, 5)
    {
        TEST_ASSERT_ALMOST_EQ(Q2(i, j), Q(i, j));
    }
    
    Matrix2D<> Q2inv = inverse(Q2);
    foreach_ij(5, 5)
    {
        TEST_ASSERT_ALMOST_EQ(Q2inv(i, j), Qinv(i, j));
    }
}

TEST_CASE(testSTLVectorAssign)
{
    std::vector<double> v(9);
    for (unsigned int i = 0; i < 9; ++i)
        v[i] = 2 * (i+1);

    Matrix2x2 A(v);
    
    TEST_ASSERT_ALMOST_EQ(A(0,0), 2);
    TEST_ASSERT_ALMOST_EQ(A(0,1), 4);
    TEST_ASSERT_ALMOST_EQ(A(1,0), 6);
    TEST_ASSERT_ALMOST_EQ(A(1,1), 8);
    
    Matrix2D<> A2(2, 2, v);
    foreach_ij(2, 2)
    {
        TEST_ASSERT_ALMOST_EQ(A(i, j), A2(i, j));
    }

    Matrix3x3 B(v);

    TEST_ASSERT_ALMOST_EQ(B(0,0), 2);
    TEST_ASSERT_ALMOST_EQ(B(0,1), 4);
    TEST_ASSERT_ALMOST_EQ(B(0,2), 6);
    TEST_ASSERT_ALMOST_EQ(B(1,0), 8);
    TEST_ASSERT_ALMOST_EQ(B(1,1), 10);
    TEST_ASSERT_ALMOST_EQ(B(1,2), 12);
    TEST_ASSERT_ALMOST_EQ(B(2,0), 14);
    TEST_ASSERT_ALMOST_EQ(B(2,1), 16);
    TEST_ASSERT_ALMOST_EQ(B(2,2), 18);

    Matrix2D<> B2(3, 3, v);
   
    foreach_ij(3, 3)
    {
        TEST_ASSERT_ALMOST_EQ(B(i, j), B2(i, j));
    }
}

TEST_CASE(testPtrAssign)
{
    double d[] = { 2, 4, 6, 8, 10, 12, 14, 16, 18 };

    Matrix2x2 A(d);
    
    TEST_ASSERT_ALMOST_EQ(A(0,0), 2);
    TEST_ASSERT_ALMOST_EQ(A(0,1), 4);
    TEST_ASSERT_ALMOST_EQ(A(1,0), 6);
    TEST_ASSERT_ALMOST_EQ(A(1,1), 8);
    
    Matrix2D<> A2(2, 2, d);
    foreach_ij(2, 2)
    {
        TEST_ASSERT_ALMOST_EQ(A(i, j), A2(i, j));
    }
    Matrix3x3 B(d);

    TEST_ASSERT_ALMOST_EQ(B(0,0), 2);
    TEST_ASSERT_ALMOST_EQ(B(0,1), 4);
    TEST_ASSERT_ALMOST_EQ(B(0,2), 6);
    TEST_ASSERT_ALMOST_EQ(B(1,0), 8);
    TEST_ASSERT_ALMOST_EQ(B(1,1), 10);
    TEST_ASSERT_ALMOST_EQ(B(1,2), 12);
    TEST_ASSERT_ALMOST_EQ(B(2,0), 14);
    TEST_ASSERT_ALMOST_EQ(B(2,1), 16);
    TEST_ASSERT_ALMOST_EQ(B(2,2), 18);
    Matrix2D<> B2(3, 3, d);
   
    foreach_ij(3, 3)
    {
        TEST_ASSERT_ALMOST_EQ(B(i, j), B2(i, j));
    }
  
}

TEST_CASE(testPermuteInvert2x2)
{
    std::vector<size_t> p(2);
    p[0] = 1;
    p[1] = 0;

    Matrix2x2 I = identityMatrix<2, double>();
    Matrix2x2 G = I.permute(p);
    TEST_ASSERT_ALMOST_EQ(G(0,0), 0);
    TEST_ASSERT_ALMOST_EQ(G(0,1), 1);
    TEST_ASSERT_ALMOST_EQ(G(1,1), 0);
    TEST_ASSERT_ALMOST_EQ(G(1,0), 1);
    Matrix2x2 H = inverse<2, double>(G);

    Matrix2x2 F = H.permute(p);
    TEST_ASSERT_EQ(I, F);

    Matrix2D<> I2 = identityMatrix<double>(2);
    TEST_ASSERT_EQ(I, I2);

    std::vector<size_t> p2(2, 0);
    p2[0] = 1;
    Matrix2D<> G2 = I2.permute(p2);
    TEST_ASSERT_EQ(G2, G);

    Matrix2D<> H2 = inverse<double>(G2);

    TEST_ASSERT_EQ(H, H2);

}

TEST_CASE(testSetCols)
{
    MatrixMxN<3, 2> A(0.0);
    Matrix2D<> A2(3, 2);
    // Make it so each column vector has the same values
    std::vector<double> v(3);
    v[0] = 1;
    v[1] = 2;
    v[2] = 3;
    A.col(0, v);
    A.col(1, &v[0]);
    
    A2.col(0, v);
    A2.col(1, &v[0]);
    foreach_ij(3, 2)
    {
        TEST_ASSERT_ALMOST_EQ(A(i, j), i + 1);
        TEST_ASSERT_ALMOST_EQ(A2(i, j), i + 1);
    }
}

TEST_CASE(testSetRows)
{
    MatrixMxN<3, 2> A;
    Matrix2D<> A2(3, 2);
    std::vector<double> v(3, 0);
    v[0] = 1;
    v[1] = 2;

    A.row(0, v);
    A.row(1, &v[0]);
    A(2, 0) = v[0];
    A(2, 1) = v[1];

    A2.row(0, v);
    A2.row(1, &v[0]);
    A2(2, 0) = v[0];
    A2(2, 1) = v[1];
    
    foreach_ij(3, 2)
    {
        TEST_ASSERT_ALMOST_EQ(A(i, j), j + 1);
        TEST_ASSERT_ALMOST_EQ(A2(i, j), j + 1);
    }
    //std::cout << A << std::endl;
}
TEST_CASE(testGrabCols)
{
    MatrixMxN<3, 2> A;
    Matrix2D<> A2(3, 2);
    foreach_ij(3, 2)
    {
        A(i, j) = (i+1)*(j+1);
        A2(i, j) = A(i, j);
    }
  
    for (size_t j = 0; j < A.cols(); ++j)
    {
        std::vector<double> cols = A.col(j);
        std::vector<double> cols2 = A2.col(j);
        
        for (unsigned int i = 0; i < A2.rows(); ++i)
        {
            TEST_ASSERT_ALMOST_EQ(cols[i], cols2[i]);
        }   
    }
}

TEST_CASE(testGrabRows)
{
    MatrixMxN<3, 2> A;
    Matrix2D<> A2(3, 2);
    foreach_ij(3, 2)
    {
        A(i, j) = (i+1)*(j+1);
        A2(i, j) = A(i, j);
    }
    for (size_t i = 0; i < A.rows(); ++i)
    {
        double* rows = A.row(i);
        double* rows2 = A2.row(i);
        TEST_ASSERT_EQ(A[i], rows);
        TEST_ASSERT_EQ(A2[i], rows2);

        for (unsigned int j = 0; j < A2.cols(); ++j)
        {
            TEST_ASSERT_ALMOST_EQ(rows[j], rows2[j]);
        }   
    }
}



TEST_CASE(testArithmeticMxN)
{
    // 2 1
    // 1 3
    Matrix2x2 D(1);
    D(0, 0) = 2;
    D(1, 1) = 3;
 
    // 3 2
    // 2 2
    Matrix2x2 E(2);
    E(0, 0) = 3;

    Matrix2x2 F = identityMatrix<2, double>();


    Matrix2x2 G = (D * E) - F;

    TEST_ASSERT_ALMOST_EQ(G(0,0), 7);
    TEST_ASSERT_ALMOST_EQ(G(0,1), 6);
    TEST_ASSERT_ALMOST_EQ(G(1,1), 7);
    TEST_ASSERT_ALMOST_EQ(G(1,0), 9);

    Matrix2D<> D2(2, 2, 1);
    D2(0, 0) = 2;
    D2(1, 1) = 3;
    TEST_ASSERT_EQ(D, D2);
    // 3 2
    // 2 2
    Matrix2D<> E2(2, 2, 2);
    E2(0, 0) = 3;
    TEST_ASSERT_EQ(E, E2);
    
    Matrix2D<> F2 = identityMatrix<double>(2);
    TEST_ASSERT_EQ(F, F2);
    
    Matrix2D<> G2 = (D2 * E2) - F2;
    TEST_ASSERT_EQ(G2, G);
    
    
}

int main()
{

    TEST_CHECK(testIdentityMxN);
    TEST_CHECK(testScaleMultiplyMxN);
    TEST_CHECK(testSetRows);
    TEST_CHECK(testGrabRows);
    TEST_CHECK(testSetCols);
    TEST_CHECK(testGrabCols);
    TEST_CHECK(testArithmeticMxN);
    TEST_CHECK(testPermuteInvert2x2);
    TEST_CHECK(testInvert2x2Complex);
    TEST_CHECK(testInvert2x2);
    TEST_CHECK(testInvert3x3);
    TEST_CHECK(testInvert4x4);
    TEST_CHECK(testInvert5x5);
    TEST_CHECK(testPtrAssign);
    TEST_CHECK(testSTLVectorAssign);
    TEST_CHECK(testOrthoTranspose5x5);
    TEST_CHECK(testNegateMxN);
    TEST_CHECK(testNegate);

    return 0;
}
