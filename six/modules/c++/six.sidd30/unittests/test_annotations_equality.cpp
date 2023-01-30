/* =========================================================================
* This file is part of six.sidd30-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
*
* six-c++ is free software; you can redistribute it and/or modify
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

#include "TestCase.h"

#include <six/sidd30/Annotations.h>

TEST_CASE(SFAPoint)
{
    six::sidd30::SFATyped* pointTyped = new six::sidd30::SFAPoint(1, 2, 3, 4);
    six::sidd30::SFATyped* secondPointTyped = new six::sidd30::SFAPoint(1, 2, 3, 4);

    six::sidd30::SFAPoint rawPointOne(1, 2, 3, 4);
    six::sidd30::SFAPoint rawPointTwo(1, 2, 3, 4);

    six::sidd30::SFAPoint* pointPointerOne = new six::sidd30::SFAPoint(1, 2, 3, 4);
    six::sidd30::SFAPoint* pointPointerTwo = new six::sidd30::SFAPoint(1, 2, 3, 4);

    TEST_ASSERT(*pointTyped == *secondPointTyped);
    TEST_ASSERT(rawPointOne == rawPointTwo);
    TEST_ASSERT(rawPointOne == static_cast<six::sidd30::SFATyped&>(rawPointTwo));
    TEST_ASSERT(static_cast<six::sidd30::SFATyped&>(rawPointOne) == rawPointTwo);
    TEST_ASSERT(*pointPointerOne == *pointPointerTwo);
    TEST_ASSERT(*pointTyped == rawPointTwo);
    TEST_ASSERT(*pointTyped == *pointPointerOne);
    TEST_ASSERT(rawPointOne == *pointPointerOne);

    six::sidd30::SFAPoint different(2, 4, 6, 8);
    TEST_ASSERT(different != *pointTyped);
    TEST_ASSERT(different != rawPointOne);
    TEST_ASSERT(different != *pointPointerOne);
}

TEST_CASE(SFALineString)
{
    six::sidd30::SFALineString stringOne;
    stringOne.vertices.resize(2);
    stringOne.vertices[0].reset(new six::sidd30::SFAPoint(1, 2, 3, 4));
    stringOne.vertices[1].reset(new six::sidd30::SFAPoint(2, 4, 6, 8));

    six::sidd30::SFALineString stringTwo;
    stringTwo.vertices.resize(2);
    stringTwo.vertices[0].reset(new six::sidd30::SFAPoint(1, 2, 3, 4));
    stringTwo.vertices[1].reset(new six::sidd30::SFAPoint(3, 4, 5, 6));


    six::sidd30::SFALineString stringThree;
    stringThree.vertices.resize(2);
    stringThree.vertices[0].reset(new six::sidd30::SFAPoint(1, 2, 3, 4));
    stringThree.vertices[1].reset(new six::sidd30::SFAPoint(2, 4, 6, 8));

    TEST_ASSERT(stringOne == stringThree);
    TEST_ASSERT(stringOne != stringTwo);
}

TEST_CASE(SFALine)
{
    six::sidd30::SFALine lineOne;
    lineOne.vertices.resize(2);
    lineOne.vertices[0].reset(new six::sidd30::SFAPoint(1, 2, 3, 4));
    lineOne.vertices[1].reset(new six::sidd30::SFAPoint(2, 4, 6, 8));

    six::sidd30::SFALine lineTwo;
    lineTwo.vertices.resize(2);
    lineTwo.vertices[0].reset(new six::sidd30::SFAPoint(1, 2, 3, 4));
    lineTwo.vertices[1].reset(new six::sidd30::SFAPoint(3, 4, 5, 6));
    TEST_ASSERT(lineOne != lineTwo);

    six::sidd30::SFALine lineThree;
    lineThree.vertices.resize(2);
    lineThree.vertices[0].reset(new six::sidd30::SFAPoint(1, 2, 3, 4));
    lineThree.vertices[1].reset(new six::sidd30::SFAPoint(2, 4, 6, 8));
    TEST_ASSERT(lineOne == lineThree);

    // Make sure different types aren't equal
    six::sidd30::SFALineString stringOne;
    stringOne.vertices.resize(2);
    stringOne.vertices[0].reset(new six::sidd30::SFAPoint(1, 2, 3, 4));
    stringOne.vertices[1].reset(new six::sidd30::SFAPoint(2, 4, 6, 8));
    TEST_ASSERT(lineOne != stringOne);

    // Make sure pointers to different types aren't equal
    six::sidd30::SFALineString* stringTwo = new six::sidd30::SFALineString();
    stringTwo->vertices.resize(2);
    stringTwo->vertices[0].reset(new six::sidd30::SFAPoint(1, 2, 3, 4));
    stringTwo->vertices[1].reset(new six::sidd30::SFAPoint(2, 4, 6, 8));
    TEST_ASSERT(lineOne != *stringTwo);
}

TEST_MAIN(
    TEST_CHECK(SFAPoint);
    TEST_CHECK(SFALineString);
    TEST_CHECK(SFALine);
)