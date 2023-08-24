/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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
#include <iostream>
#include <memory>

#include <cphd/Dwell.h>
#include <xml/lite/MinidomParser.h>
#include <six/Init.h>
#include <cphd/Enums.h>
#include <cphd/Types.h>

#include "TestCase.h"

TEST_CASE(testDerivativeCODValid)
{
    cphd::Dwell dwell;
    cphd::COD cod1;
    size_t numCODTimes = 1;
    dwell.cod.resize(numCODTimes);
    dwell.cod[0] = cod1;

    cphd::Poly2D p2D(1, 1);
    dwell.cod[0].codTimePoly = p2D;
    dwell.cod[0].codTimePoly[0][0] = 1;
    dwell.cod[0].codTimePoly[1][0] = 2;
    dwell.cod[0].codTimePoly[0][1] = 3;
    dwell.cod[0].codTimePoly[1][1] = 4;

    // Test Derivative in terms of x
    cphd::Poly2D derivativeX2D(1,1);
    derivativeX2D[0][0] = 2;
    derivativeX2D[0][1] = 4;
    TEST_ASSERT_EQ(dwell.cod[0].codTimePoly.derivativeX(),derivativeX2D);
}

TEST_CASE(testDerivativeDwellValid)
{
    cphd::Dwell dwell;
    cphd::DwellTime dtime1;
    size_t numDwellTimes = 1;
    dwell.dtime.resize(numDwellTimes);
    dwell.dtime[0] = dtime1;

    cphd::Poly2D p2D(1, 1);
    dwell.dtime[0].dwellTimePoly = p2D;
    dwell.dtime[0].dwellTimePoly[0][0] = 1;
    dwell.dtime[0].dwellTimePoly[1][0] = 2;
    dwell.dtime[0].dwellTimePoly[0][1] = 3;
    dwell.dtime[0].dwellTimePoly[1][1] = 4;

    // Test Derivative in terms of x
    cphd::Poly2D derivativeY2D(1,1);
    derivativeY2D[0][0] = 3;
    derivativeY2D[1][0] = 4;
    TEST_ASSERT_EQ(dwell.dtime[0].dwellTimePoly.derivativeY(),derivativeY2D);
}


TEST_CASE(testEquality)
{
    cphd::Dwell dwell;
    cphd::DwellTime dtime1, dtime2;

    size_t numDwellTimes = 2;
    dwell.dtime.resize(numDwellTimes);
    dwell.dtime[0] = dtime1;
    dwell.dtime[1] = dtime2;

    cphd:: Poly2D p2D_1(2,1), p2D_2(2,1);

    dwell.dtime[0].dwellTimePoly = p2D_1;
    dwell.dtime[1].dwellTimePoly = p2D_2;
    for (size_t i = 0; i < dwell.dtime.size(); ++i) {
        dwell.dtime[i].dwellTimePoly[0][0] = 1;
        dwell.dtime[i].dwellTimePoly[0][1] = 2;
        dwell.dtime[i].dwellTimePoly[1][0] = 3;
        dwell.dtime[i].dwellTimePoly[1][1] = 4;
        dwell.dtime[i].dwellTimePoly[2][0] = 5;
    }

    cphd:: Poly2D derivX(1,1);
    derivX[0][0] = 3;
    derivX[0][1] = 4;
    derivX[1][0] = 10;

    // Test identical polynomial vectors
    TEST_ASSERT_EQ(dwell.dtime[0].dwellTimePoly.derivativeX(), derivX);
    TEST_ASSERT_TRUE((dwell.dtime[0] == dwell.dtime[1]));

    dwell.dtime[1].dwellTimePoly = dwell.dtime[1].dwellTimePoly.flipXY();

    // Test different polynomial vectors
    TEST_ASSERT_NOT_EQ(dwell.dtime[0].dwellTimePoly.derivativeX(), dwell.dtime[1].dwellTimePoly.derivativeX());
    TEST_ASSERT_TRUE((dwell.dtime[0] != dwell.dtime[1]));
}

TEST_MAIN(
        TEST_CHECK(testDerivativeCODValid);
        TEST_CHECK(testDerivativeDwellValid);
        TEST_CHECK(testEquality);
        )
