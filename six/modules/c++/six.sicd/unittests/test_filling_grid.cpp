/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
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

TEST_CASE(DerivedDeltaKsNoImageData)
{
    six::sicd::ImageData imageData;
    six::sicd::DirectionParameters row;
    row.impulseResponseBandwidth = 13;
    row.sampleSpacing = 5;
    row.deltaKCOAPoly = six::Poly2D(3, 3);
    for (size_t ii = 0; ii <= row.deltaKCOAPoly.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= row.deltaKCOAPoly.orderY(); ++jj)
        {
            row.deltaKCOAPoly[ii][jj] = ii + jj;
        }
    }
    row.fillDerivedFields(imageData);
    TEST_ASSERT_ALMOST_EQ(row.deltaK1, -0.1);
    TEST_ASSERT_ALMOST_EQ(row.deltaK2, 0.1);
}

TEST_CASE(DerivedDeltaKsWithImageData)
{
    six::sicd::ImageData imageData;
    imageData.validData.push_back(six::RowColInt(10, 20));
    imageData.validData.push_back(six::RowColInt(30, 40));
    imageData.validData.push_back(six::RowColInt(50, 60));
    six::sicd::DirectionParameters row;
    row.impulseResponseBandwidth = 13;
    row.sampleSpacing = 5;
    row.deltaKCOAPoly = six::Poly2D(3, 3);
    for (size_t ii = 0; ii <= row.deltaKCOAPoly.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= row.deltaKCOAPoly.orderY(); ++jj)
        {
            row.deltaKCOAPoly[ii][jj] = ii + jj;
        }
    }
    row.fillDerivedFields(imageData);
    //TODO: find test data where these numbers aren't identical to above test
    TEST_ASSERT_ALMOST_EQ(row.deltaK1, -0.1);
    TEST_ASSERT_ALMOST_EQ(row.deltaK2, 0.1);
}

TEST_CASE(IdentityWeightFunction)
{
    six::sicd::DirectionParameters row;
    row.weightType.reset(new six::sicd::WeightType());
    row.weightType->windowName = "UNIFORM";

    // This shouldn't affect anything, but it's a required argument
    six::sicd::ImageData imageData;
    row.fillDerivedFields(imageData);
    TEST_ASSERT(row.weights.empty());
}

TEST_CASE(HammingWindow)
{
    six::sicd::DirectionParameters row;
    row.weightType.reset(new six::sicd::WeightType());
    row.weightType->windowName = "HAMMING";

    six::sicd::ImageData imageData;
    row.fillDerivedFields(imageData);
    TEST_ASSERT_EQ(row.weights.size(), 512);
    //Just grabbing some random values to make sure they line up with MATLAB
    //TEST_ASSERT_ALMOST_EQ is too picky for what we have, so doing something
    //more manual.
    TEST_ASSERT_LESSER(std::abs(row.weights[163] - .7332), .0001);
    TEST_ASSERT_LESSER(std::abs(row.weights[300] - .9328), .0001)
}

TEST_CASE(KaiserWindow)
{
    six::sicd::DirectionParameters row;
    row.weightType.reset(new six::sicd::WeightType());
    row.weightType->windowName = "KAISER";
    six::Parameter param;
    param.setName("Name");
    param.setValue("10");
    row.weightType->parameters.push_back(param);

    six::sicd::ImageData imageData;
    row.fillDerivedFields(imageData);
    TEST_ASSERT_LESSER(std::abs(row.weights[10] - .0014), .0001);
    TEST_ASSERT_LESSER(std::abs(row.weights[300] - .8651), .0001);
    TEST_ASSERT_LESSER(std::abs(row.weights[460] - .0238), .0001);
}

int main(int, char**)
{
    TEST_CHECK(DerivedDeltaKsNoImageData);
    TEST_CHECK(DerivedDeltaKsWithImageData);
    TEST_CHECK(IdentityWeightFunction);
    TEST_CHECK(HammingWindow);
    TEST_CHECK(KaiserWindow);
    return 0;
}