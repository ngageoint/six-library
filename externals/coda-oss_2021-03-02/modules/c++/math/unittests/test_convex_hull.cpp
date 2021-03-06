/* =========================================================================
 * This file is part of math-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * math-c++ is free software; you can redistribute it and/or modify
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

#include <math/ConvexHull.h>
#include <sys/Conf.h>
#include "TestCase.h"

namespace
{
TEST_CASE(testConvexHull)
{
    // Add in all the points
    std::vector<types::RowCol<sys::Int64_T> > inputPoints;
    inputPoints.push_back(types::RowCol<sys::Int64_T>(90, 97));
    inputPoints.push_back(types::RowCol<sys::Int64_T>(10, 27));
    inputPoints.push_back(types::RowCol<sys::Int64_T>(8, 59));
    inputPoints.push_back(types::RowCol<sys::Int64_T>(19, 58));
    inputPoints.push_back(types::RowCol<sys::Int64_T>(90, 85));
    inputPoints.push_back(types::RowCol<sys::Int64_T>(91, 62));
    inputPoints.push_back(types::RowCol<sys::Int64_T>(42, 94));
    inputPoints.push_back(types::RowCol<sys::Int64_T>(68, 84));
    inputPoints.push_back(types::RowCol<sys::Int64_T>(21, 16));
    inputPoints.push_back(types::RowCol<sys::Int64_T>(14, 49));
    inputPoints.push_back(types::RowCol<sys::Int64_T>(84, 31));
    inputPoints.push_back(types::RowCol<sys::Int64_T>(25, 40));
    inputPoints.push_back(types::RowCol<sys::Int64_T>(95, 59));
    inputPoints.push_back(types::RowCol<sys::Int64_T>(89, 55));
    inputPoints.push_back(types::RowCol<sys::Int64_T>(95, 81));
    inputPoints.push_back(types::RowCol<sys::Int64_T>(46, 22));
    inputPoints.push_back(types::RowCol<sys::Int64_T>(80, 27));
    inputPoints.push_back(types::RowCol<sys::Int64_T>(90, 18));
    inputPoints.push_back(types::RowCol<sys::Int64_T>(37, 59));
    inputPoints.push_back(types::RowCol<sys::Int64_T>(45, 38));

    // Compute the convex hull
    /// @note  This is destructive to input points
    std::vector<types::RowCol<sys::Int64_T> > convexHull;
    math::ConvexHull<sys::Int64_T>(inputPoints, convexHull);

    // Generate the expected points
    std::vector<types::RowCol<sys::Int64_T> > expectedConvexHull;
    expectedConvexHull.push_back(types::RowCol<sys::Int64_T>(21, 16));
    expectedConvexHull.push_back(types::RowCol<sys::Int64_T>(10, 27));
    expectedConvexHull.push_back(types::RowCol<sys::Int64_T>(8, 59));
    expectedConvexHull.push_back(types::RowCol<sys::Int64_T>(42, 94));
    expectedConvexHull.push_back(types::RowCol<sys::Int64_T>(90, 97));
    expectedConvexHull.push_back(types::RowCol<sys::Int64_T>(95, 81));
    expectedConvexHull.push_back(types::RowCol<sys::Int64_T>(95, 59));
    expectedConvexHull.push_back(types::RowCol<sys::Int64_T>(90, 18));
    expectedConvexHull.push_back(types::RowCol<sys::Int64_T>(21, 16));

    // Check that they match
    TEST_ASSERT_EQ(convexHull.size(), expectedConvexHull.size());
    for (size_t ii = 0; ii < convexHull.size(); ++ii)
    {
        TEST_ASSERT_EQ(convexHull[ii].row, expectedConvexHull[ii].row);
        TEST_ASSERT_EQ(convexHull[ii].col, expectedConvexHull[ii].col);
    }
}
}

int main(int, char**)
{
    TEST_CHECK(testConvexHull);
    return 0;
}
