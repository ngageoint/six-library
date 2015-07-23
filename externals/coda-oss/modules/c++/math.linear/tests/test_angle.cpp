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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <TestCase.h>
#include <math/linear/Vector.h>
#include <math/linear/VectorN.h>

namespace
{
TEST_CASE(testAngleVN)
{
    math::linear::VectorN<3> v1;
    v1[0] = 383; v1[1] = 886; v1[2] = 777;
    math::linear::VectorN<3> v2;
    v2[0] = 915; v2[1] = 793; v2[2] = 335;
    double expected1 = 0.566725971742884;
    double actual1 = v1.angle(v2);
    TEST_ASSERT_ALMOST_EQ(actual1, expected1);

    math::linear::VectorN<3> v3;
    v3[0] = 386; v3[1] = 492; v3[2] = 649;
    math::linear::VectorN<3> v4;
    v4[0] = 421; v4[1] = 362; v4[2] = 27;
    double expected2 = 0.774404308906766;
    double actual2 = v3.angle(v4);
    TEST_ASSERT_ALMOST_EQ(actual2, expected2);

    math::linear::VectorN<3> v5;
    v5[0] = 690; v5[1] = 59; v5[2] = 763;
    math::linear::VectorN<3> v6;
    v6[0] = 926; v6[1] = 540; v6[2] = 426;
    double expected3 = 0.57747993638172;
    double actual3 = v5.angle(v6);
    TEST_ASSERT_ALMOST_EQ(actual3, expected3);

    math::linear::VectorN<3> v7;
    v7[0] = 172; v7[1] = 736; v7[2] = 211;
    math::linear::VectorN<3> v8;
    v8[0] = 368; v8[1] = 567; v8[2] = 429;
    double expected4 = 0.429741111954024;
    double actual4 = v7.angle(v8);
    TEST_ASSERT_ALMOST_EQ(actual4, expected4);

    math::linear::VectorN<3> v9;
    v9[0] = 782; v9[1] = 530; v9[2] = 862;
    math::linear::VectorN<3> v10;
    v10[0] = 123; v10[1] = 67; v10[2] = 135;
    double expected5 = 0.0757306621542832;
    double actual5 = v9.angle(v10);
    TEST_ASSERT_ALMOST_EQ(actual5, expected5);

    math::linear::VectorN<3> v11;
    v11[0] = 929; v11[1] = 802; v11[2] = 22;
    math::linear::VectorN<3> v12;
    v12[0] = 58; v12[1] = 69; v12[2] = 167;
    double expected6 = 1.0648468726463;
    double actual6 = v11.angle(v12);
    TEST_ASSERT_ALMOST_EQ(actual6, expected6);

    math::linear::VectorN<3> v13;
    v13[0] = 393; v13[1] = 456; v13[2] = 11;
    math::linear::VectorN<3> v14;
    v14[0] = 42; v14[1] = 229; v14[2] = 373;
    double expected7 = 1.07890417806091;
    double actual7 = v13.angle(v14);
    TEST_ASSERT_ALMOST_EQ(actual7, expected7);

    math::linear::VectorN<3> v15;
    v15[0] = 421; v15[1] = 919; v15[2] = 784;
    math::linear::VectorN<3> v16;
    v16[0] = 537; v16[1] = 198; v16[2] = 324;
    double expected8 = 0.665014174279145;
    double actual8 = v15.angle(v16);
    TEST_ASSERT_ALMOST_EQ(actual8, expected8);

    math::linear::VectorN<3> v17;
    v17[0] = 315; v17[1] = 370; v17[2] = 413;
    math::linear::VectorN<3> v18;
    v18[0] = 526; v18[1] = 91; v18[2] = 980;
    double expected9 = 0.557001110850508;
    double actual9 = v17.angle(v18);
    TEST_ASSERT_ALMOST_EQ(actual9, expected9);

    math::linear::VectorN<3> v19;
    v19[0] = 956; v19[1] = 873; v19[2] = 862;
    math::linear::VectorN<3> v20;
    v20[0] = 170; v20[1] = 996; v20[2] = 281;
    double expected10 = 0.67423184139059;
    double actual10 = v19.angle(v20);
    TEST_ASSERT_ALMOST_EQ(actual10, expected10);
}
TEST_CASE(testAngleV)
{
    math::linear::Vector<double> v1(3);
    v1[0] = 383; v1[1] = 886; v1[2] = 777;
    math::linear::Vector<double> v2(3);
    v2[0] = 915; v2[1] = 793; v2[2] = 335;
    double expected1 = 0.566725971742884;
    double actual1 = v1.angle(v2);
    TEST_ASSERT_ALMOST_EQ(actual1, expected1);

    math::linear::Vector<double> v3(3);
    v3[0] = 386; v3[1] = 492; v3[2] = 649;
    math::linear::Vector<double> v4(3);
    v4[0] = 421; v4[1] = 362; v4[2] = 27;
    double expected2 = 0.774404308906766;
    double actual2 = v3.angle(v4);
    TEST_ASSERT_ALMOST_EQ(actual2, expected2);

    math::linear::Vector<double> v5(3);
    v5[0] = 690; v5[1] = 59; v5[2] = 763;
    math::linear::Vector<double> v6(3);
    v6[0] = 926; v6[1] = 540; v6[2] = 426;
    double expected3 = 0.57747993638172;
    double actual3 = v5.angle(v6);
    TEST_ASSERT_ALMOST_EQ(actual3, expected3);

    math::linear::Vector<double> v7(3);
    v7[0] = 172; v7[1] = 736; v7[2] = 211;
    math::linear::Vector<double> v8(3);
    v8[0] = 368; v8[1] = 567; v8[2] = 429;
    double expected4 = 0.429741111954024;
    double actual4 = v7.angle(v8);
    TEST_ASSERT_ALMOST_EQ(actual4, expected4);

    math::linear::Vector<double> v9(3);
    v9[0] = 782; v9[1] = 530; v9[2] = 862;
    math::linear::Vector<double> v10(3);
    v10[0] = 123; v10[1] = 67; v10[2] = 135;
    double expected5 = 0.0757306621542832;
    double actual5 = v9.angle(v10);
    TEST_ASSERT_ALMOST_EQ(actual5, expected5);

    math::linear::Vector<double> v11(3);
    v11[0] = 929; v11[1] = 802; v11[2] = 22;
    math::linear::Vector<double> v12(3);
    v12[0] = 58; v12[1] = 69; v12[2] = 167;
    double expected6 = 1.0648468726463;
    double actual6 = v11.angle(v12);
    TEST_ASSERT_ALMOST_EQ(actual6, expected6);

    math::linear::Vector<double> v13(3);
    v13[0] = 393; v13[1] = 456; v13[2] = 11;
    math::linear::Vector<double> v14(3);
    v14[0] = 42; v14[1] = 229; v14[2] = 373;
    double expected7 = 1.07890417806091;
    double actual7 = v13.angle(v14);
    TEST_ASSERT_ALMOST_EQ(actual7, expected7);

    math::linear::Vector<double> v15(3);
    v15[0] = 421; v15[1] = 919; v15[2] = 784;
    math::linear::Vector<double> v16(3);
    v16[0] = 537; v16[1] = 198; v16[2] = 324;
    double expected8 = 0.665014174279145;
    double actual8 = v15.angle(v16);
    TEST_ASSERT_ALMOST_EQ(actual8, expected8);

    math::linear::Vector<double> v17(3);
    v17[0] = 315; v17[1] = 370; v17[2] = 413;
    math::linear::Vector<double> v18(3);
    v18[0] = 526; v18[1] = 91; v18[2] = 980;
    double expected9 = 0.557001110850508;
    double actual9 = v17.angle(v18);
    TEST_ASSERT_ALMOST_EQ(actual9, expected9);

    math::linear::Vector<double> v19(3);
    v19[0] = 956; v19[1] = 873; v19[2] = 862;
    math::linear::Vector<double> v20(3);
    v20[0] = 170; v20[1] = 996; v20[2] = 281;
    double expected10 = 0.67423184139059;
    double actual10 = v19.angle(v20);
    TEST_ASSERT_ALMOST_EQ(actual10, expected10);
}
}
int main()
{
    TEST_CHECK(testAngleVN);
    TEST_CHECK(testAngleV);
    return 0;
}
