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

namespace
{
double calculateError(double actual, double expected)
{
    return std::abs((actual - expected) / actual);
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

    TEST_CHECK(DerivedArpPoly);
    TEST_CHECK(DerivedSCPCOA);
    return 0;
}