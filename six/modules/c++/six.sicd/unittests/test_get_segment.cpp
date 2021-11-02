/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

namespace
{
TEST_CASE(testCanFindSegment)
{
    six::sicd::AreaPlane areaPlane;
    mem::ScopedCloneablePtr<six::sicd::Segment> firstSegment(
            new six::sicd::Segment());
    firstSegment->identifier = "First Segment";
    firstSegment->startSample = 1;
    areaPlane.segmentList.push_back(firstSegment);

    mem::ScopedCloneablePtr<six::sicd::Segment> secondSegment(
            new six::sicd::Segment());
    secondSegment->identifier = "Second Segment";
    secondSegment->startSample = 2;
    areaPlane.segmentList.push_back(secondSegment);

    TEST_ASSERT_EQ(areaPlane.getSegment("First Segment").identifier,
            "First Segment");
    TEST_ASSERT_EQ(areaPlane.getSegment("First Segment").startSample,
            1);
    TEST_ASSERT_EQ(areaPlane.getSegment("Second Segment").identifier,
            "Second Segment");
    TEST_ASSERT_EQ(areaPlane.getSegment("Second Segment").startSample,
            2);
}

TEST_CASE(testThrowIfMissingSegment)
{
    six::sicd::AreaPlane areaPlane;
    mem::ScopedCloneablePtr<six::sicd::Segment> segment(
            new six::sicd::Segment());
    segment->identifier = "id";
    areaPlane.segmentList.push_back(segment);

    TEST_EXCEPTION(areaPlane.getSegment("Wrong id"));
}

TEST_CASE(testHandlesNullPointers)
{
    six::sicd::AreaPlane areaPlane;
    areaPlane.segmentList.resize(3);
    areaPlane.segmentList[2].reset(new six::sicd::Segment());
    areaPlane.segmentList[2]->identifier = "void";
    areaPlane.getSegment("void");
}
}

TEST_MAIN((void)argv; (void)argc;
    TEST_CHECK(testCanFindSegment);
    TEST_CHECK(testThrowIfMissingSegment);
    TEST_CHECK(testHandlesNullPointers);
    )

