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
#include <scene/ProjectionModel.h>
#include "TestCase.h"

namespace
{

bool isNaN(double val)
{
    // This only works with IEEE 754 floats
    return (val != val);
}

TEST_CASE(fillValidData)
{
    six::sicd::GeoData geoData;
    geoData.scp.ecf = six::Vector3(3);

    six::sicd::SCPCOA scpcoa;
    scpcoa.arpPos = six::Vector3(1);
    scpcoa.arpVel = six::Vector3(2);

    six::sicd::Grid grid;
    grid.row.reset(new six::sicd::DirectionParameters());
    grid.row->unitVector[0] = 1;
    grid.col.reset(new six::sicd::DirectionParameters());
    grid.col->unitVector[1] = 1;

    six::PolyXYZ arpPoly(1);
    six::Poly2D timeCOAPoly(1, 1);

    scene::PlaneProjectionModel model(scpcoa.slantPlaneNormal(geoData.scp.ecf),
            grid.row->unitVector, grid.col->unitVector,
            geoData.scp.ecf, arpPoly, timeCOAPoly,
            scpcoa.look(geoData.scp.ecf));

    six::sicd::ImageData imageData;
    imageData.validData.resize(3);
    imageData.validData[0] = six::RowColInt(10, 20);
    imageData.validData[1] = six::RowColInt(30, 40);
    imageData.validData[2] = six::RowColInt(50, 60);

    imageData.numRows = 123;
    imageData.numCols = 456;

    // TODO: Figure out how to make this populate with actual numbers
    geoData.fillDerivedFields(imageData, model);

    for (size_t ii = 0; ii < 3; ++ii)
    {
        // If this fails, try with -ffast-math not set
        TEST_ASSERT(isNaN(geoData.validData[ii].getLat()));
        TEST_ASSERT(isNaN(geoData.validData[ii].getLon()));
    }

    for (size_t ii = 0; ii < 4; ++ii)
    {
        // If this fails, try with -ffast-math not set
        TEST_ASSERT(isNaN(geoData.imageCorners.getCorner(ii).getLat()));
        TEST_ASSERT(isNaN(geoData.imageCorners.getCorner(ii).getLon()));
    }
}

TEST_CASE(ecfFromLlh)
{
    six::sicd::GeoData geoData;
    geoData.scp.llh = six::LatLonAlt(10, 20, 30);

    // This data doesn't matter; just need the object to pass to the function
    six::sicd::ImageData imageData;
    scene::PlaneProjectionModel model(six::Vector3({0, 0, 0}),
                                      six::Vector3({0, 0, 0}),
                                      six::Vector3({0, 0, 0}),
                                      six::Vector3({0, 0, 0}),
                                      six::PolyXYZ(1),
                                      six::Poly2D(1, 1),
                                      -1);

    geoData.fillDerivedFields(imageData, model);
    const six::Vector3 expected({5903057.30519,
                                 2148537.15026,
                                 1100253.75718});

    for (size_t ii = 0; ii < 3; ++ii)
    {
        // This is as much accuracy as the results from
        // MATLAB give.
        TEST_ASSERT_ALMOST_EQ_EPS(expected[ii], geoData.scp.ecf[ii], 1e-4);
    }
}
}

TEST_MAIN(
    TEST_CHECK(ecfFromLlh);
    TEST_CHECK(fillValidData);
    )
