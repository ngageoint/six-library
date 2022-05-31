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
    TEST_ASSERT_ALMOST_EQ(position.arpPoly[0][1], 22448.5);
    TEST_ASSERT_EQ(position.arpPoly[0][2], 44897);

    TEST_ASSERT_EQ(position.arpPoly[1][0], 0);
    TEST_ASSERT_EQ(position.arpPoly[1][1], -367);
    TEST_ASSERT_EQ(position.arpPoly[1][2], -734);

    TEST_ASSERT_EQ(position.arpPoly[2][0], 0);
    TEST_ASSERT_EQ(position.arpPoly[2][1], 1.5);
    TEST_ASSERT_EQ(position.arpPoly[2][2], 3);
}

TEST_MAIN(
    TEST_CHECK(DerivedArpPoly);
)
