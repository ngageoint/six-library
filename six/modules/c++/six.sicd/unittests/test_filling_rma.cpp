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

TEST_CASE(DerivedRMAT)
{
    six::sicd::GeoData geoData;
    for (size_t ii = 0; ii < geoData.scp.ecf.size(); ++ii)
    {
        geoData.scp.ecf[ii] = 100 * (ii + 1);
    }

    six::sicd::Position position;
    double fc(950);

    six::sicd::RMA rma;
    rma.rmat.reset(new six::sicd::RMAT());

    rma.fillDerivedFields(geoData, position, fc);

    TEST_ASSERT_ALMOST_EQ(rma.rmat->dopConeAngleRef, 100);
}

int main(int, char**)
{
    TEST_CHECK(DerivedRMAT);
    return 0;
}