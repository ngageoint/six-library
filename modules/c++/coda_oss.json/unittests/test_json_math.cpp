/* =========================================================================
 * This file is part of coda-oss.json-c++
 * =========================================================================
 *
 * (C) Copyright 2025 ARKA Group, L.P. All rights reserved
 *
 * types-c++ is free software; you can redistribute it and/or modify
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
 */

#include "TestCase.h"

#include <nlohmann/json.hpp>
#include <coda_oss/json/Math.h>

using json = nlohmann::json;

TEST_CASE(TestVectorN)
{
    using VecI = math::linear::VectorN<2, int>;
    using VecD = math::linear::VectorN<3, double>;
    
    std::vector<int> v0 = {
        static_cast<int>(0), 
        static_cast<int>(1)};
    VecI startVal0(v0);
    json expected = v0;
    json serialized = startVal0;
    auto deserialized0 = serialized.template get<VecI>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal0 == deserialized0);
    
    std::vector<double> v1 = {10., 20., 30.};
    VecD startVal1(v1);
    expected = v1;
    serialized = startVal1;
    auto deserialized1 = serialized.template get<VecD>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal1 == deserialized1);
}
TEST_CASE(TestMatrixMxN)
{
    using TwoXTwo = math::linear::MatrixMxN<2, 2, int>;
    using ThreeXTwo = math::linear::MatrixMxN<3, 2, int>;
    using TwoXThree = math::linear::MatrixMxN<2, 3, double>;

    TwoXTwo startVal0({0, 1, 2, 3});
    json expected = std::vector<std::vector<int>>{{0, 1}, {2, 3}};
    json serialized = startVal0;
    auto deserialized0 = serialized.template get<TwoXTwo>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal0 == deserialized0);

    ThreeXTwo startVal1({0, 1, 2, 3, 4, 5});
    expected = std::vector<std::vector<int>>{{0, 1}, {2, 3}, {4, 5}};
    serialized = startVal1;
    auto deserialized1 = serialized.template get<ThreeXTwo>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal1 == deserialized1);

    TwoXThree startVal2({0., 1., 2., 3., 4., 5.});
    expected = std::vector<std::vector<double>>{{0., 1., 2.}, {3., 4., 5.}}; 
    serialized = startVal2;
    auto deserialized2 = serialized.template get<TwoXThree>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal2 == deserialized2);
}
TEST_CASE(TestPolyOneD)
{
    using OneD = math::poly::OneD<int>;
    std::vector<int> coeffs{0, 1, 2, 3};
    OneD startVal(coeffs);
    json serialized = startVal;
    json expected = coeffs;
    OneD deserialized = serialized.template get<OneD>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal == deserialized);
}
TEST_CASE(TestPolyTwoD)
{
    using OneD = math::poly::OneD<double>;
    using TwoD = math::poly::TwoD<double>;
    std::vector<double> c0 = {0.1, 0.2, 0.3};
    std::vector<double> c1 = {-0.3, -0.2, -0.1};
    OneD p0(c0);
    OneD p1(c1);
    TwoD startVal({p0, p1});
    json serialized = startVal;
    json expected = std::vector<std::vector<double>>{c0, c1};
    auto deserialized = serialized.template get<TwoD>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal == deserialized);
}
TEST_CASE(TestPolyXYZ)
{
    using XYZ = math::linear::VectorN<3>;
    using OneDXYZ = math::poly::OneD<XYZ>;
    std::vector<double> c0 = {0.1, 0.2, 0.3};
    std::vector<double> c1 = {-0.3, -0.2, -0.1};
    OneDXYZ startVal({XYZ(c0), XYZ(c1)});
    json serialized = startVal;
    json expected = {c0, c1};
    auto deserialized = serialized.template get<OneDXYZ>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal == deserialized);
}


TEST_MAIN(
    TEST_CHECK(TestVectorN);
    TEST_CHECK(TestMatrixMxN);
    TEST_CHECK(TestPolyOneD);
    TEST_CHECK(TestPolyTwoD);
    TEST_CHECK(TestPolyXYZ);
)
