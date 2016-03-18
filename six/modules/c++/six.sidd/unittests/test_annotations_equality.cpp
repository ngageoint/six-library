/* =========================================================================
* This file is part of six.sidd-c++
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

#include <six/sidd/Annotations.h>

TEST_CASE(SFAPoint)
{
    six::sidd::SFATyped* point = new six::sidd::SFAPoint(1, 2, 3, 4);
    six::sidd::SFATyped* second = new six::sidd::SFAPoint(1, 2, 3, 4);

    six::sidd::SFAPoint rawPointOne(1, 2, 3, 4);
    six::sidd::SFAPoint rawPointTwo(1, 2, 3, 4);

    six::sidd::SFAPoint* pointPointerOne = new six::sidd::SFAPoint(1, 2, 3, 4);
    six::sidd::SFAPoint* pointPointerTwo = new six::sidd::SFAPoint(1, 2, 3, 4);

    TEST_ASSERT(*point == *second);
    TEST_ASSERT(rawPointOne == rawPointTwo);
    TEST_ASSERT(*pointPointerOne == *pointPointerTwo);
    TEST_ASSERT(*point == rawPointTwo);
    TEST_ASSERT(*point == *pointPointerOne);
    TEST_ASSERT(rawPointOne == *pointPointerOne);

    six::sidd::SFAPoint different(2, 4, 6, 8);
    TEST_ASSERT(different != *point);
    TEST_ASSERT(different != rawPointOne);
    TEST_ASSERT(different != *pointPointerOne);
}

int main(int, char**)
{
    TEST_CHECK(SFAPoint);
}