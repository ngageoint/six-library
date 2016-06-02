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

int main(int, char**)
{
    TEST_CHECK(DerivedDeltaKsNoImageData);
    TEST_CHECK(DerivedDeltaKsWithImageData);
    return 0;
}