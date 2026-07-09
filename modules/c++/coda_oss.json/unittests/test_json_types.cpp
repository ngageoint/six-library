/* =========================================================================
 * This file is part of coda-oss.json-c++
 * =========================================================================
 *
 * (C) Copyright 2025 ARKA Group, L.P. All rights reserved
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
 */

#include "TestCase.h"

#include <coda_oss/json/Types.h>

using json = nlohmann::json;

TEST_CASE(TestComplex)
{
    types::Complex<int32_t> startVal(1, 2);
    json expected = {{"real", startVal.real()}, {"imag", startVal.imag()}};
    json serialized = startVal;
    auto deserialized = serialized.template get<types::Complex<int32_t>>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal == deserialized);
}
TEST_CASE(TestRange)
{
    types::Range startVal(0, 10);
    json expected = {
        {"mStartElement", startVal.mStartElement},
        {"mNumElements", startVal.mNumElements}};
    json serialized = startVal;
    auto deserialized = serialized.template get<types::Range>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal == deserialized);
}
TEST_CASE(TestRangeList)
{
    types::RangeList startVal;
    types::Range r0(0, 10);
    types::Range r1(15, 15);
    startVal.insert(r0);
    startVal.insert(r1);
    json expected = {
        {
            {"mStartElement", r0.mStartElement},
            {"mNumElements", r0.mNumElements}
        },
        {
            {"mStartElement", r1.mStartElement},
            {"mNumElements", r1.mNumElements}
        },

    };
    json serialized = startVal;
    auto deserialized = serialized.template get<types::RangeList>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal == deserialized);
}
TEST_CASE(TestRgAz)
{
    types::RgAz<double> startVal(0.0, 1.0);
    json expected = {{"rg", startVal.rg}, {"az", startVal.az}};
    json serialized = startVal;
    auto deserialized = serialized.template get<types::RgAz<double>>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal == deserialized);
}
TEST_CASE(TestRowCol)
{
    types::RowCol<float> startVal(0.0, 1.0);
    json expected = {{"row", startVal.row}, {"col", startVal.col}};
    json serialized = startVal;
    auto deserialized = serialized.template get<types::RowCol<float>>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal == deserialized);
}
TEST_CASE(TestPageRowCol)
{
    types::PageRowCol<uint32_t> startVal(0, 1, 2);
    json expected = {
        {"page", startVal.page},
        {"row", startVal.row},
        {"col", startVal.col}
    };
    json serialized = startVal;
    auto deserialized = serialized.template get<types::PageRowCol<uint32_t>>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal == deserialized);
}

TEST_MAIN(
    TEST_CHECK(TestComplex);
    TEST_CHECK(TestRange);
    TEST_CHECK(TestRangeList);
    TEST_CHECK(TestRgAz);
    TEST_CHECK(TestRowCol);
    TEST_CHECK(TestPageRowCol);
)
