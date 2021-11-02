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

TEST_CASE(testClockwiseBox)
{
    std::vector<six::RowColInt> vertices(4);
    vertices[0] = six::RowColInt(0, 0); // these are (row, col), not (x, y)!
    vertices[1] = six::RowColInt(1, 0);
    vertices[2] = six::RowColInt(1, 1);
    vertices[3] = six::RowColInt(0, 1);
    TEST_ASSERT(!six::sicd::Utilities::isClockwise(vertices));
    TEST_ASSERT(six::sicd::Utilities::isClockwise(vertices, true));

}

TEST_CASE(testCounterClockwiseTriangle)
{
    std::vector<six::RowColInt> vertices(3);
    vertices[0] = six::RowColInt(0, 0);
    vertices[1] = six::RowColInt(0, 1);
    vertices[2] = six::RowColInt(1, 0);
    TEST_ASSERT(!six::sicd::Utilities::isClockwise(vertices, true));
    TEST_ASSERT(six::sicd::Utilities::isClockwise(vertices));
}

TEST_MAIN((void)argv; (void)argc;
    TEST_CHECK(testClockwiseBox);
    TEST_CHECK(testCounterClockwiseTriangle);
)

