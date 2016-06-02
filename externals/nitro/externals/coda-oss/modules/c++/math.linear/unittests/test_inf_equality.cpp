/* =========================================================================
* This file is part of math.linear-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
*
* math.linear-c++ is free software; you can redistribute it and/or modify
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

#include "TestCase.h"
#include <math/linear/MatrixMxN.h>

TEST_CASE(positiveInfinityEq)
{
    TEST_ASSERT(math::linear::equals(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()));
    TEST_ASSERT(math::linear::equals(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()));
}

TEST_CASE(negativeInfinityEq)
{
    TEST_ASSERT(math::linear::equals(-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()));
    TEST_ASSERT(math::linear::equals(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()));
}

TEST_CASE(differentSignsNe)
{
    TEST_ASSERT(!math::linear::equals(std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()));
    TEST_ASSERT(!math::linear::equals(std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()));
}

int main(int, char**)
{
    TEST_CHECK(positiveInfinityEq);
    TEST_CHECK(negativeInfinityEq);
    TEST_CHECK(differentSignsNe);
}

