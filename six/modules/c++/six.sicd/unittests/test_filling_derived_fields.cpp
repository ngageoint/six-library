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

namespace
{
double calculateError(double actual, double expected)
{
    return std::abs((actual - expected) / actual);
}
}
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

TEST_CASE(DerivedScp)
{
    six::sicd::GeoData geoData;
    geoData.scp.llh = six::LatLonAlt(10, 20, 30);
    six::sicd::ImageData imageData;
    geoData.fillDerivedFields(imageData);
    std::vector<double> expectedData(3);
    expectedData[0] = 5.9031e6;
    expectedData[1] = 2.1485e6;
    expectedData[2] = 1.1003e6;
    six::Vector3 expected(expectedData);
    for (size_t ii = 0; ii < 3; ++ii)
    {
        TEST_ASSERT_LESSER(std::abs(expected[ii] - geoData.scp.ecf[ii]), 100);
    }
}

TEST_CASE(DerivedSCPCOA)
{
    six::sicd::SCPCOA scpcoa;
    scpcoa.scpTime = 123;

    six::sicd::Position position;
    std::vector<six::Vector3> data(4);

    for (size_t ii = 0; ii < data.size(); ++ii)
    {
        std::vector<double> current(3);
        for (size_t jj = 0; jj < current.size(); ++jj)
        {
            current[jj] = ii + jj;
        }
        data[ii] = six::Vector3(current);
    }

    position.arpPoly = six::PolyXYZ(data);

    six::sicd::Grid grid;
    six::sicd::GeoData geoData;
    for (size_t ii = 0; ii < geoData.scp.ecf.size(); ++ii)
    {
        geoData.scp.ecf[ii] = ii + 1;
    }

    scpcoa.fillDerivedFields(geoData, grid, position);

    TEST_ASSERT_EQ(scpcoa.arpPos[0], 5612982);
    TEST_ASSERT_EQ(scpcoa.arpPos[1], 7489102);
    TEST_ASSERT_EQ(scpcoa.arpPos[2], 9365222);

    TEST_ASSERT_EQ(scpcoa.arpVel[0], 136654);
    TEST_ASSERT_EQ(scpcoa.arpVel[1], 182288);
    TEST_ASSERT_EQ(scpcoa.arpVel[2], 227922);

    TEST_ASSERT_EQ(scpcoa.arpAcc[0], 2218);
    TEST_ASSERT_EQ(scpcoa.arpAcc[1], 2958);
    TEST_ASSERT_EQ(scpcoa.arpAcc[2], 3698);

    TEST_ASSERT_EQ(scpcoa.sideOfTrack.toString(), "LEFT");
    TEST_ASSERT_LESSER(calculateError(scpcoa.slantRange, 13240000), 1e-5);
    TEST_ASSERT_LESSER(calculateError(scpcoa.groundRange, 0.6953), 1e-4);
    TEST_ASSERT_LESSER(calculateError(scpcoa.dopplerConeAngle, 179.9922), 1e-5);
    TEST_ASSERT_LESSER(calculateError(scpcoa.grazeAngle, 79.2171), 1e-5);
    TEST_ASSERT_LESSER(calculateError(scpcoa.incidenceAngle, 10.7829), 1e-5);
    TEST_ASSERT_LESSER(calculateError(std::abs(scpcoa.twistAngle), 89.3273), 1e-5);
    TEST_ASSERT_LESSER(calculateError(scpcoa.slopeAngle, 89.8742), 1e-2);
    TEST_ASSERT_LESSER(calculateError(scpcoa.azimAngle, 222.4278), 1e-5);
    TEST_ASSERT_LESSER(calculateError(scpcoa.layoverAngle, 133.0886), 1e-5);
}

TEST_CASE(DerivedArpPoly)
{
    six::sicd::SCPCOA scpcoa;
    scpcoa.scpTime = 123;

    scpcoa.arpPos[0] = 0;
    scpcoa.arpPos[1] = 1;
    scpcoa.arpPos[2] = 2;

    scpcoa.arpVel[0] = 0;
    scpcoa.arpVel[1] = 2;
    scpcoa.arpVel[2] = 4;

    scpcoa.arpAcc[0] = 0;
    scpcoa.arpAcc[1] = 3;
    scpcoa.arpAcc[2] = 6;

    six::sicd::Position position;
    position.fillDerivedFields(scpcoa);

    TEST_ASSERT_EQ(position.arpPoly[0][0], 0);
    TEST_ASSERT_LESSER(std::abs(position.arpPoly[0][1] - 22449), 1);
    TEST_ASSERT_EQ(position.arpPoly[0][2], 44897);

    TEST_ASSERT_EQ(position.arpPoly[1][0], 0);
    TEST_ASSERT_EQ(position.arpPoly[1][1], -367);
    TEST_ASSERT_EQ(position.arpPoly[1][2], -734);

    TEST_ASSERT_EQ(position.arpPoly[2][0], 0);
    TEST_ASSERT_EQ(position.arpPoly[2][1], 1.5);
    TEST_ASSERT_EQ(position.arpPoly[2][2], 3);
}

int main(int, char**)
{
    TEST_CHECK(DerivedDeltaKsNoImageData);
    TEST_CHECK(DerivedDeltaKsWithImageData);
    TEST_CHECK(IdentityWeightFunction);
    TEST_CHECK(HammingWindow);
    TEST_CHECK(KaiserWindow);
    TEST_CHECK(DerivedScp);
    TEST_CHECK(DerivedArpPoly);
    TEST_CHECK(DerivedSCPCOA)
    return 0;
}