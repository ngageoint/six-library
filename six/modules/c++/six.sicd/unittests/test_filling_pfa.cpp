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

TEST_CASE(DefaultPFA)
{
    six::sicd::GeoData geoData;
    for (size_t ii = 0; ii < geoData.scp.ecf.size(); ++ii)
    {
        geoData.scp.ecf[ii] = 100.0 * (ii + 1);
    }

    six::sicd::Grid grid;
    grid.imagePlane = six::ComplexImagePlaneType::SLANT;

    six::sicd::SCPCOA scpcoa;
    scpcoa.scpTime = 123;
    for (size_t ii = 0; ii < scpcoa.arpVel.size(); ++ii)
    {
        scpcoa.arpPos[ii] = static_cast<double>(ii);
        scpcoa.arpVel[ii] = ii + 1.0;
    }

    six::sicd::PFA pfa;
    pfa.fillDefaultFields(geoData, grid, scpcoa);

    TEST_ASSERT_ALMOST_EQ(pfa.imagePlaneNormal[0], -0.40824829);
    TEST_ASSERT_ALMOST_EQ(pfa.imagePlaneNormal[1], 0.81649658);
    TEST_ASSERT_ALMOST_EQ(pfa.imagePlaneNormal[2], -0.40824829);
    TEST_ASSERT_ALMOST_EQ(pfa.focusPlaneNormal[0], 0.26610694);
    TEST_ASSERT_ALMOST_EQ(pfa.focusPlaneNormal[1], 0.53221388);
    TEST_ASSERT_ALMOST_EQ(pfa.focusPlaneNormal[2], 0.80370110);
    TEST_ASSERT_EQ(pfa.polarAngleRefTime, 123);
}

TEST_MAIN((void)argv; (void)argc;
    TEST_CHECK(DefaultPFA);
)
