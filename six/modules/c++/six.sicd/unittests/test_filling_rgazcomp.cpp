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

TEST_CASE(DerivedRgAzComp)
{
    six::sicd::Timeline timeline;
    timeline.interPulsePeriod.reset(new six::sicd::InterPulsePeriod());
    six::sicd::TimelineSet set;
    set.interPulsePeriodPoly = six::Poly1D(3);
    for (size_t ii = 0; ii < set.interPulsePeriodPoly.size(); ++ii)
    {
        set.interPulsePeriodPoly[ii] = ii * 10.0;
    }
    timeline.interPulsePeriod->sets.push_back(set);

    six::sicd::Grid grid;
    grid.row->kCenter = 5;
    grid.row->deltaKCOAPoly = six::Poly2D(3, 3);
    for (size_t ii = 0; ii <= grid.row->deltaKCOAPoly.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= grid.row->deltaKCOAPoly.orderY(); ++jj)
        {
            grid.row->deltaKCOAPoly[ii][jj] = static_cast<double>(ii + jj);
        }
    }

    // Make this not symmetric to be sure we're doing row and column
    // ops in the right order
    grid.row->deltaKCOAPoly[3][1] = 8;

    six::sicd::SCPCOA scpcoa;
    scpcoa.scpTime = 123;
    for (size_t ii = 0; ii < 3; ++ii)
    {
        const double castedIndex = static_cast<double>(ii);
        scpcoa.arpPos[ii] = castedIndex;
        scpcoa.arpVel[ii] = std::pow(castedIndex, 3);
    }
    scpcoa.dopplerConeAngle = 66;
    scpcoa.slantRange = 88;

    six::sicd::GeoData geoData;
    for (size_t ii = 0; ii < geoData.scp.ecf.size(); ++ii)
    {
        geoData.scp.ecf[ii] = 100.0 * (ii + 1);
    }

    six::sicd::RgAzComp rgAzComp;
    rgAzComp.fillDerivedFields(geoData, grid, scpcoa, timeline);

    TEST_ASSERT_ALMOST_EQ(rgAzComp.azSF, -0.01038119);
    TEST_ASSERT_ALMOST_EQ(rgAzComp.kazPoly[0], 2.69484938e-05);
    TEST_ASSERT_ALMOST_EQ(rgAzComp.kazPoly[1], 3.06232884e-05);
    TEST_ASSERT_ALMOST_EQ(rgAzComp.kazPoly[2], 3.42980830e-05);
    TEST_ASSERT_ALMOST_EQ(rgAzComp.kazPoly[3], 4.04227407e-05);
}

TEST_MAIN(
    TEST_CHECK(DerivedRgAzComp);
)

