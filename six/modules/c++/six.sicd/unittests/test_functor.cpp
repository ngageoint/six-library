/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2014, MDA Information Systems LLC
*
* six.sicd-c++ is free software; you can redistribute it and/or modify
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
#include <import/six/sicd.h>
#include "TestCase.h"

// Bessel function test data from
// http://keisan.casio.com/exec/system/1180573475
TEST_CASE(ZeroOrderBesselFunction)
{
    TEST_ASSERT_ALMOST_EQ(six::sicd::bessi(0, 5), 27.23987182);
    TEST_ASSERT_ALMOST_EQ(six::sicd::bessi(0, 2), 2.279585302);
}

TEST_CASE(OneOrderBesselFunction)
{
    TEST_ASSERT_ALMOST_EQ(six::sicd::bessi(1, 4), 9.759465154);
    TEST_ASSERT_ALMOST_EQ(six::sicd::bessi(1, 2), 1.590636855);
}

TEST_CASE(FifthOrderBesselFunction)
{
    TEST_ASSERT_ALMOST_EQ(six::sicd::bessi(5, 6), 7.96846774);
    TEST_ASSERT_ALMOST_EQ(six::sicd::bessi(5, 2), 0.00982567932);
}

TEST_CASE(RaisedCosReturnsCorrectlySizedVector)
{
    six::sicd::RaisedCos fun(0.4);
    TEST_ASSERT_EQ(fun(4).size(), 4);
    TEST_ASSERT_EQ(fun(5).size(), 5);
}

TEST_CASE(TestRaisedCosEvenOutput)
{
    six::sicd::RaisedCos fun(0.4);
    std::vector<double> expected;
    expected.push_back(-0.2);
    expected.push_back(0.7);
    expected.push_back(0.7);
    expected.push_back(-0.2);

    double error = 1e-3;
    std::vector<double> actual = fun(4);
    for (size_t ii = 0; ii < actual.size(); ++ii)
    {
        TEST_ASSERT_LESSER_EQ(std::abs(expected[ii] - actual[ii]), error);
    }
}

TEST_CASE(TestRaisedCosOddOutput)
{
    six::sicd::RaisedCos fun(0.4);
    std::vector<double> expected;
    expected.push_back(-0.2);
    expected.push_back(0.4);
    expected.push_back(1.0);
    expected.push_back(0.4);
    expected.push_back(-0.2);

    double error = 1e-3;
    std::vector<double> actual = fun(5);
    for (size_t ii = 0; ii < actual.size(); ++ii)
    {
        TEST_ASSERT_LESSER_EQ(std::abs(expected[ii] - actual[ii]), error);
    }
}

TEST_CASE(TestKaiserWindowFunction)
{
    six::sicd::Kaiser kaiser(2.3);
    std::vector<double> expected;
    expected.push_back(0.3534);
    expected.push_back(0.3534);
    
    double error = 1e-3;
    std::vector<double> actual = kaiser(2);
    for (size_t ii = 0; ii < actual.size(); ++ii)
    {
        TEST_ASSERT_LESSER_EQ(std::abs(expected[ii] - actual[ii]), error);
    }
}

int main(int, char**)
{
    TEST_CHECK(ZeroOrderBesselFunction);
    TEST_CHECK(OneOrderBesselFunction);
    TEST_CHECK(FifthOrderBesselFunction);
    TEST_CHECK(RaisedCosReturnsCorrectlySizedVector);
    TEST_CHECK(TestKaiserWindowFunction);
    return 0;
}