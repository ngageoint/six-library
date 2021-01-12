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
    position.arpPoly = six::PolyXYZ(4);
    position.arpPoly[0][0] = 0;
    position.arpPoly[0][1] = 1;
    position.arpPoly[0][2] = 2;

    position.arpPoly[1][0] = 1;
    position.arpPoly[1][1] = 2;
    position.arpPoly[1][2] = 3;

    position.arpPoly[2][0] = 2;
    position.arpPoly[2][1] = 3;
    position.arpPoly[2][2] = 4;

    position.arpPoly[3][0] = 3;
    position.arpPoly[3][1] = 4;
    position.arpPoly[3][2] = 5;

    six::sicd::Grid grid; // WHY?
    six::sicd::GeoData geoData;

    geoData.scp.ecf[0] = -1015099;
    geoData.scp.ecf[1] = 451951;
    geoData.scp.ecf[2] = 6259542;

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

    TEST_ASSERT_EQ(scpcoa.sideOfTrack, "RIGHT");
    TEST_ASSERT_ALMOST_EQ(scpcoa.slantRange, 1.015372838920571e7);
    TEST_ASSERT_ALMOST_EQ(scpcoa.groundRange, 5.327326157987932e6);
    TEST_ASSERT_ALMOST_EQ(scpcoa.dopplerConeAngle, 152.272143845);
    TEST_ASSERT_ALMOST_EQ(scpcoa.grazeAngle, 14.27738036);
    TEST_ASSERT_ALMOST_EQ(scpcoa.incidenceAngle, 75.72261963);
    TEST_ASSERT_ALMOST_EQ(scpcoa.slopeAngle, 90.05513834);
    TEST_ASSERT_ALMOST_EQ(scpcoa.twistAngle, -89.9431043);
    TEST_ASSERT_ALMOST_EQ(scpcoa.azimAngle, 291.9837145);
    TEST_ASSERT_ALMOST_EQ(scpcoa.layoverAngle, 201.9977459);
}

TEST_MAIN(
    TEST_CHECK(DerivedSCPCOA);
)
