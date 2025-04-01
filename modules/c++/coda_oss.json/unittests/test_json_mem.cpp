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

#include <mem/ScopedCloneablePtr.h>
#include <mem/ScopedCopyablePtr.h>
#include <nlohmann/json.hpp>

#include <coda_oss/json/Types.h>
#include <coda_oss/json/Mem.h>

using json = nlohmann::json;

TEST_CASE(TestCloneablePtr)
{
    using RC = types::RowCol<int>;
    RC val(-1, 1);
    mem::ScopedCloneablePtr<RC> startVal(new RC(val));
    json expected = {{"row", val.row}, {"col", val.col}};
    json serialized = startVal;
    auto deserialized = serialized.template get<mem::ScopedCloneablePtr<RC>>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal == deserialized);
}

TEST_CASE(TestCopyablePtr)
{
    int val = 42;
    mem::ScopedCopyablePtr<int> startVal(new int(val));
    json expected = val;
    json serialized = startVal;
    auto deserialized = serialized.template get<mem::ScopedCopyablePtr<int>>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal == deserialized);
}
TEST_CASE(TestNullPtr)
{
    mem::ScopedCopyablePtr<size_t> startVal;
    json expected = nullptr;
    json serialized = startVal;
    auto deserialized = serialized.template get<mem::ScopedCopyablePtr<size_t>>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal == deserialized);
}

TEST_MAIN(
    TEST_CHECK(TestCloneablePtr);
    TEST_CHECK(TestCopyablePtr);
    TEST_CHECK(TestNullPtr);
)
