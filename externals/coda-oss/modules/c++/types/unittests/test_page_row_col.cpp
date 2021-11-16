/* =========================================================================
 * This file is part of types-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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
#include <cmath>
#include <limits>
#include <types/RowCol.h>
#include <types/PageRowCol.h>

namespace
{
TEST_CASE(TestPageRowColSizeT)
{
    // default constructor should initialize all to 0
    const types::PageRowCol<size_t> pageRowColDefault;
    TEST_ASSERT_EQ(pageRowColDefault.page, static_cast<size_t>(0));
    TEST_ASSERT_EQ(pageRowColDefault.row, static_cast<size_t>(0));
    TEST_ASSERT_EQ(pageRowColDefault.col, static_cast<size_t>(0));


    const types::PageRowCol<size_t> pageRowColA(3,5,11);
    const types::PageRowCol<size_t> pageRowColB(1,2,3);

    // test equality operators
    TEST_ASSERT(pageRowColA == pageRowColA);
    TEST_ASSERT(!(pageRowColA != pageRowColA));

    // test page + RowCol constructor
    const types::RowCol<size_t> rowCol(5, 11);
    const types::PageRowCol<size_t> pageRowColFromRowCol(3, rowCol);
    TEST_ASSERT(pageRowColFromRowCol == pageRowColA);

    // test copy construction
    const types::PageRowCol<size_t> pageRowColCopy(pageRowColA);
    TEST_ASSERT(pageRowColCopy == pageRowColA);

    // test copy assignment
    types::PageRowCol<size_t> pageRowColAssign;
    pageRowColAssign = pageRowColA;
    TEST_ASSERT(pageRowColAssign == pageRowColA);

    TEST_ASSERT_EQ(pageRowColA.volume(), static_cast<size_t>(3 * 5 * 11));

    // test arithmetic operators
    const types::PageRowCol<size_t> sum = pageRowColA + pageRowColB;
    TEST_ASSERT(sum == types::PageRowCol<size_t>(4, 7, 14));

    const types::PageRowCol<size_t> diff = pageRowColA - pageRowColB;
    TEST_ASSERT(diff == types::PageRowCol<size_t>(2, 3, 8));

    const types::PageRowCol<size_t> prod = pageRowColA * pageRowColB;
    TEST_ASSERT(prod == types::PageRowCol<size_t>(3, 10, 33));

    const types::PageRowCol<size_t> div = pageRowColA / pageRowColB;
    TEST_ASSERT(div == types::PageRowCol<size_t>(3, 2, 3));
}

TEST_CASE(TestPageRowColDouble)
{
    const types::PageRowCol<double> pageRowColA(3.1, 5.2, 11.3);
    const types::PageRowCol<double> pageRowColB(1.1, 2.2, 3.3);

    // test equality operators
    TEST_ASSERT(pageRowColA == pageRowColA);
    TEST_ASSERT(!(pageRowColA != pageRowColA));

    // test copy construction
    const types::PageRowCol<double> pageRowColCopy(pageRowColA);
    TEST_ASSERT(pageRowColCopy == pageRowColA);

    // test copy assignment
    types::PageRowCol<double> pageRowColAssign;
    pageRowColAssign = pageRowColA;
    TEST_ASSERT(pageRowColAssign == pageRowColA);

    // test approximate equality
    pageRowColAssign.page += std::numeric_limits<double>::epsilon() / 2.0;
    TEST_ASSERT(pageRowColAssign == pageRowColA);

    pageRowColAssign.page += std::numeric_limits<double>::epsilon() * 2.0;
    TEST_ASSERT(pageRowColAssign != pageRowColA);

    TEST_ASSERT_EQ(pageRowColA.volume(), 3.1 * 5.2 * 11.3);
    TEST_ASSERT_EQ(pageRowColA.normL2(), std::sqrt(3.1 * 3.1 +  5.2 * 5.2 + 11.3 * 11.3));

    // test arithmetic operators
    const types::PageRowCol<double> sum = pageRowColA + pageRowColB;
    TEST_ASSERT(sum == types::PageRowCol<double>(3.1 + 1.1, 5.2 + 2.2, 11.3 + 3.3));

    const types::PageRowCol<double> diff = pageRowColA - pageRowColB;
    TEST_ASSERT(diff == types::PageRowCol<double>(3.1 - 1.1, 5.2 - 2.2, 11.3 - 3.3));

    const types::PageRowCol<double> prod = pageRowColA * pageRowColB;
    TEST_ASSERT(prod == types::PageRowCol<double>(3.1 * 1.1, 5.2 * 2.2, 11.3 * 3.3));

    const types::PageRowCol<double> div = pageRowColA / pageRowColB;
    TEST_ASSERT(div == types::PageRowCol<double>(3.1 / 1.1, 5.2 / 2.2, 11.3 / 3.3));
}
}

int main(int /*argc*/, char** /*argv*/)
{
    TEST_CHECK(TestPageRowColSizeT);
    TEST_CHECK(TestPageRowColDouble);
    return 0;
}
