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

    TEST_ASSERT_EQ(scpcoa.sideOfTrack.toString(), "RIGHT");
    TEST_ASSERT_ALMOST_EQ(scpcoa.slantRange, 13240071.80623738);
    TEST_ASSERT_ALMOST_EQ(scpcoa.groundRange, 0.69533621);
    TEST_ASSERT_ALMOST_EQ(scpcoa.dopplerConeAngle, 179.9922244);
    TEST_ASSERT_ALMOST_EQ(scpcoa.grazeAngle, 79.21708938);
    TEST_ASSERT_ALMOST_EQ(scpcoa.incidenceAngle, 10.78291061);
    TEST_ASSERT_ALMOST_EQ(scpcoa.slopeAngle, 89.87415027);
    TEST_ASSERT_ALMOST_EQ(scpcoa.twistAngle, 89.32730959);
    TEST_ASSERT_ALMOST_EQ(scpcoa.azimAngle, 222.4277615);
    TEST_ASSERT_ALMOST_EQ(scpcoa.layoverAngle, 133.0885753);
}

int main(int, char**)
{
    TEST_CHECK(DerivedSCPCOA);
    return 0;
}