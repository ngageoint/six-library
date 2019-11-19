/* =========================================================================
 * This file is part of types-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * types-c++ is free software; you can redistribute it and/or modify
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
#include <types/Range.h>

namespace
{
TEST_CASE(TestGetNumSharedElements)
{
    const types::Range range(100, 50);

    // Ends at lower bound of 'range' - will not share any elements
    TEST_ASSERT_EQ(range.getNumSharedElements(0, 100), 0);

    // Starts at upper bound of 'range' - shouldn't share any elements
    TEST_ASSERT_EQ(range.getNumSharedElements(150, 50), 0);

    // Intersects 'range' at its lower bound, but doesn't cover the
    // full extent - shares [100, 125]
    TEST_ASSERT_EQ(range.getNumSharedElements(50, 75), 25);

    // Intersects 'range' at its upper bound but doesn't cover the
    // full extent - shares [120, 150)
    TEST_ASSERT_EQ(range.getNumSharedElements(120, 45), 30);

    // Lays between the upper and lower bound of 'range' - 'range' should
    // share all elements [120, 135]
    TEST_ASSERT_EQ(range.getNumSharedElements(120, 15), 15);

    // Covers the entirety of 'range', should share [100, 150)
    TEST_ASSERT_EQ(range.getNumSharedElements(0, 200), 50);

    // Ranges are the same - should share [100, 150)
    TEST_ASSERT_EQ(range.getNumSharedElements(100, 50), 50);
}

TEST_CASE(TestTouches)
{
    // Ranges do not overlap or touch -- touches(...) returns false
    {
        const types::Range A(5, 4);  // [5, 8]
        const types::Range B(12, 1); // [12, 12]
        TEST_ASSERT_FALSE(A.touches(B));
        TEST_ASSERT_FALSE(B.touches(A));
    }

    // Ranges overlap -- touches(...) returns false
    {
        const types::Range A(5, 4); // [5, 8]
        const types::Range B(8, 1); // [8, 8]
        TEST_ASSERT_FALSE(A.touches(B));
        TEST_ASSERT_FALSE(B.touches(A));
    }

    // Ranges touch -- touches(...) returns true
    {
        const types::Range A(5, 4); // [5, 8]
        const types::Range B(9, 1); // [9, 9]
        TEST_ASSERT_TRUE(A.touches(B));
        TEST_ASSERT_TRUE(B.touches(A));
    }

    // One of the ranges is empty -- touches(...) returns false
    {
        const types::Range A(10, 0);  // [10, 0)
        const types::Range B(10, 10); // [10, 20)
        TEST_ASSERT_FALSE(A.touches(B));
        TEST_ASSERT_FALSE(B.touches(A));
    }

    // Both of the ranges are empty -- touches(...) returns false
    {
        const types::Range A(10, 0);  // [10, 0)
        const types::Range B(10, 0); // [10, 20)
        TEST_ASSERT_FALSE(A.touches(B));
        TEST_ASSERT_FALSE(B.touches(A));
    }
}
}

int main(int /*argc*/, char** /*argv*/)
{
    TEST_CHECK(TestGetNumSharedElements);
    TEST_CHECK(TestTouches);
    return 0;
}
