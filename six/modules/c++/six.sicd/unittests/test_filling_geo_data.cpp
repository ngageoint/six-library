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

TEST_CASE(ecfFromLlh)
{
    six::sicd::GeoData geoData;
    geoData.scp.llh = six::LatLonAlt(10, 20, 30);
    six::sicd::ImageData imageData;
    geoData.fillDerivedFields(imageData);
    std::vector<double> expectedData(3);
    expectedData[0] = 5903057.30519;
    expectedData[1] = 2148537.15026;
    expectedData[2] = 1100253.75718;
    six::Vector3 expected(expectedData);
    for (size_t ii = 0; ii < 3; ++ii)
    {
        // This is as much accuracy as the results from
        // MATLAB give.
        TEST_ASSERT_ALMOST_EQ_EPS(expected[ii], geoData.scp.ecf[ii], 1e-4);
    }
}

int main(int, char**)
{
    TEST_CHECK(ecfFromLlh);
    return 0;
}

