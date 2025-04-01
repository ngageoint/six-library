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

#include <nlohmann/json.hpp>

#include <coda_oss/json/Types.h>
#include <coda_oss/json/Std.h>

using json = nlohmann::json;
using namespace std;

TEST_CASE(TestStdComplex)
{
    complex<float> startVal(-1, 1);
    json expected = {{"real", startVal.real()}, {"imag", startVal.imag()}};
    json serialized = startVal;
    auto deserialized = serialized.template get<complex<float>>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal == deserialized);
}
TEST_CASE(TestStdSharedVal)
{
    shared_ptr<complex<float>> startVal(new complex<float>(-1, 1));
    json expected = {{"real", startVal->real()}, {"imag", startVal->imag()}};
    json serialized = startVal;
    auto deserialized = serialized.template get<shared_ptr<complex<float>>>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(*startVal == *deserialized);
}
TEST_CASE(TestStdSharedNull)
{
    shared_ptr<complex<float>> startVal;
    json expected = nullptr;
    json serialized = startVal;
    auto deserialized = serialized.template get<shared_ptr<complex<float>>>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal == deserialized);
}
TEST_CASE(TestStdUniqueVal)
{
    unique_ptr<complex<double>> startVal(new complex<double>(-1, 1));
    json expected = {{"real", startVal->real()}, {"imag", startVal->imag()}};
    json serialized = startVal;
    auto deserialized = serialized.template get<unique_ptr<complex<double>>>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(*startVal == *deserialized);
}
TEST_CASE(TestStdUniqueNull)
{
    unique_ptr<complex<double>> startVal;
    json expected = nullptr;
    json serialized = startVal;
    auto deserialized = serialized.template get<unique_ptr<complex<double>>>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal == deserialized);
}
TEST_CASE(TestStdOptionVal)
{
    optional<int> startVal(0);
    json expected = 0;
    json serialized = startVal;
    auto deserialized = serialized.template get<optional<int>>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal == deserialized);
}
TEST_CASE(TestStdOptionEmpty)
{
    optional<int> startVal;
    json expected = nullptr;
    json serialized = startVal;
    auto deserialized = serialized.template get<optional<int>>();
    TEST_ASSERT(serialized == expected);
    TEST_ASSERT(startVal == deserialized);
}

TEST_MAIN(
    TEST_CHECK(TestStdComplex);
    TEST_CHECK(TestStdSharedVal);
    TEST_CHECK(TestStdSharedNull);
    TEST_CHECK(TestStdUniqueVal);
    TEST_CHECK(TestStdUniqueNull);
    TEST_CHECK(TestStdOptionVal);
    TEST_CHECK(TestStdOptionEmpty);
)
