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


/*
        mx::VectorN<3> vec = mx::constantVector<3, double>(1);
        vec[0] = fromSample;
        vec[1] = fromLine;
        // M x P
        mx::VectorN<3> result = m3x3 * vec;
        sample = result[0];
        line   = result[1];

 */

typedef math::linear::MatrixMxN<2, 2> Matrix2x2;
typedef math::linear::MatrixMxN<3, 2> Matrix3x2;
typedef math::linear::MatrixMxN<3, 3> Matrix3x3;
typedef math::linear::MatrixMxN<4, 4> Matrix4x4;
typedef math::linear::VectorN<3> Vector3;
typedef math::linear::VectorN<2> Vector2;

TEST_CASE(testEquality)
{
    std::vector<double> v1(3, 3);
    math::linear::Vector<> v2(3, 3);
    Vector3 v3(3);
   
    if (v2 != v1)
        TEST_ASSERT(false);
    if (v3 != v1)
        TEST_ASSERT(false);
    
    TEST_ASSERT_EQ(v2, v3);
    TEST_ASSERT_EQ(v3, v2);
}

TEST_CASE(testPtrAssign)
{
    std::vector<double> v1(3, 42);
    math::linear::Vector<> v2(v1.size(), &v1[0]);
    Vector3 v3(&v1[0]);

    if (v2 != v1)
        TEST_ASSERT(false);
    TEST_ASSERT_EQ(v2, v3);
}

TEST_CASE(testSTLVectorAssign)
{
    std::vector<double> v1(3, 42);
    math::linear::Vector<> v2(v1);
    Vector3 v3(&v1[0]);

    if (v2 != v1)
        TEST_ASSERT(false);
    TEST_ASSERT_EQ(v2, v3);
}

TEST_CASE(testEmptyDim)
{
    math::linear::Matrix2D<double> AScale(3, 0);
    TEST_ASSERT_TRUE(true); // need to use hidden "testName" parameter
}

TEST_CASE(testPtrDecorator)
{
    std::vector<double> v1(9, 1);
    math::linear::Matrix2D<double> AScale(3, 3, &v1[0], false);

    AScale.scale(5);
    std::vector<double> v5(9, 5);

    // validate that the underlying memory has been mutated
    for (size_t i = 0; i < v1.size(); ++i)
        TEST_ASSERT_EQ(v1[i], v5[i]);
}

TEST_CASE(testPtrAdopt)
{
    // valgrind to ensure that we don't have a leak
    math::linear::Matrix2D<double> AScale(3, 3, new double[9], true);
    TEST_ASSERT_TRUE(true); // need to use hidden "testName" parameter
}

TEST_CASE(testArithmetic)
{
    auto A = math::linear::identityMatrix<double>(3);
    Matrix3x3 A2( &(A.get())[0] );

    auto x = math::linear::constantVector<double>(3, 1.2);
    x[2] = 2;
    Vector3 x2(x.matrix().row(0));
    
    Vector3 b2 = A2 * x2;
    TEST_ASSERT_EQ(b2, x2);
    TEST_ASSERT_EQ(x, x2);
    TEST_ASSERT_EQ(A2, A);

    math::linear::Vector<double> b = A * x;
    TEST_ASSERT_EQ(b, x);
}

TEST_CASE(testLinear)
{
    double q[] = 
    {
        0.681759432346851, -0.469118050335902, -0.561366485689289,
        0.484971425801214,  0.864311190345374, -0.133299971493248,
        0.547728758202852, -0.181368192064663,  0.816761156241381
    };

    math::linear::Matrix2D<> mx1(3, 3, q);
    Matrix3x3 mx2(q);


    // This one can do a raw pointer assign
    Vector3 u1(mx2.row(0));

    // This one doesnt know how big it is unless you tell it
    math::linear::Vector<> u2(3, mx1.row(0));
    TEST_ASSERT_EQ(u1, u2);

    // Notice that it doesnt matter which matrix the row came
    // from since its a raw pointer
    Vector3 u3(mx1.row(0));
    TEST_ASSERT_EQ(u2, u3);

    // Okay, time to get to work
    Vector3 v1(mx1.row(1));

    Vector3 w1 = mx2.row(2);
    Vector3 w2 = math::linear::cross<double>(u1, v1);
    TEST_ASSERT_EQ(w2, w1);

    Vector3 w3 = u1.dot(v1);
    Vector3 zeros((double)0);

    TEST_ASSERT_EQ(w3, zeros);
}

TEST_CASE(testNormalize)
{
    double d[]  = {0.276922984960890, 0.046171390631154, 0.097131781235848};
    double n[]  = {0.932169641471869, 0.155420716185689, 0.326962016904922};

    Vector3 v1(d);
    v1.normalize();
    
    math::linear::Vector<> v2(3, d);
    v2.normalize();

    
    TEST_ASSERT_EQ(v1, v2);
    TEST_ASSERT_EQ(v2, v1);

    Vector3 truth(n);
    
    TEST_ASSERT_ALMOST_EQ(v1[0], truth[0]);
    TEST_ASSERT_ALMOST_EQ(v1[1], truth[1]);
    TEST_ASSERT_ALMOST_EQ(v1[2], truth[2]);
}

TEST_MAIN(
    TEST_CHECK(testEquality);
    TEST_CHECK(testNormalize);
    TEST_CHECK(testPtrAssign);
    TEST_CHECK(testSTLVectorAssign);
    TEST_CHECK(testEmptyDim);
    TEST_CHECK(testPtrDecorator);
    TEST_CHECK(testPtrAdopt);
    TEST_CHECK(testArithmetic);
    TEST_CHECK(testLinear);
    /*
    Matrix3x3 A = mx::identityMatrix<3, double>();
    
    Vector3 v3 = mx::constantVector<3, double>(1.2);

    v3[2] = 2;
    
    std::cout << A * v3 << std::endl;


    std::cout << v3 + v3 << std::endl;

    v3.normalize();
    std::cout << v3 << std::endl;
    std::cout << v3.norm() << std::endl;

    std::cout << v3 * 4.0 << std::endl;
    
    v3 = mx::constantVector<3, double>(0);
    v3[0] = 1;

    _V3 y = mx::constantVector<3, double>(0);
    y[1] = 1;

    std::cout << cross(v3, y) << std::endl;
*/
)
