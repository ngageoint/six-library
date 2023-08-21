/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <nitf/Field.hpp>
#include <nitf/TestingTest.hpp>
#include <nitf/FieldDescriptor.hpp>
#include "TestCase.h"

TEST_CASE(testCastOperator)
{
    nitf::Field field(20, nitf::Field::BCS_A);

    // Test unsigned values
    field.set(123);
    const uint8_t valUint8 = field;
    TEST_ASSERT_EQ(valUint8, static_cast<uint8_t>(123));

    field.set(12345);
    const uint16_t valUint16 = field;
    TEST_ASSERT_EQ(valUint16, static_cast<uint16_t>(12345));

    field.set(1234567890);
    const uint32_t valUint32 = field;
    TEST_ASSERT_EQ(valUint32, static_cast<uint32_t>(1234567890));
#if SIZEOF_SIZE_T == 4
    const size_t valSizeT = field;
    TEST_ASSERT_EQ(valSizeT, 1234567890);
#endif

    field.set(uint64_t(1234567890987));
    const uint64_t valUint64 = field;
    TEST_ASSERT_EQ(valUint64, static_cast<uint64_t>(1234567890987));

#if SIZEOF_SIZE_T == 8
    const size_t valSizeT = field;
    TEST_ASSERT_EQ(valSizeT, static_cast<size_t>(1234567890987));
#endif

    // Test signed values
    field.set(-123);
    const int8_t valInt8 = field;
    TEST_ASSERT_EQ(valInt8, gsl::narrow<int8_t>(-123));

    field.set(-12345);
    const int16_t valInt16 = field;
    TEST_ASSERT_EQ(valInt16, gsl::narrow<int16_t>(-12345));

    field.set(-1234567890);
    const int32_t valInt32 = field;
    TEST_ASSERT_EQ(valInt32, -1234567890);
#if SIZEOF_SIZE_T == 4
    const size_t valSSizeT = field;
    TEST_ASSERT_EQ(valSSizeT, static_cast<size_t>(-1234567890));
#endif

    // TODO: I think the %lld isn't working, at least in VS, in
    //       nitf_Field_setInt64(), so need to do this via string
    field.set("-1234567890987");
    const int64_t valInt64 = field;
    TEST_ASSERT_EQ(valInt64, -1234567890987);
#if SIZEOF_SIZE_T == 8
    const size_t valSSizeT = field;
    TEST_ASSERT_EQ(valSSizeT, static_cast<size_t>(-1234567890987));
#endif

    // Test float values
    field.set(-123.45f);
    const float valFloat = field;
    TEST_ASSERT_EQ(valFloat, -123.45f);

    field.set(-567.89);
    const double valDouble = field;
    TEST_ASSERT_EQ(valDouble, -567.89);

    // Test arbitrary string
    field.set("ABCxyz");
    const std::string valStr = field;
    TEST_ASSERT_EQ(valStr, std::string("ABCxyz              "));
}
TEST_CASE(testDescriptors)
{
    nitf::testing::Test1a test1a;
    test1a.setF1("1234");

    const auto descriptors = test1a.getDescriptors();
    TEST_ASSERT_EQ(gsl::narrow<size_t>(1), descriptors.size());
    for (const auto& descriptor : descriptors)
    {
        TEST_ASSERT_EQ(std::string("f1"), descriptor.name());
        const auto field = descriptor.getField(test1a);
        const std::string value = field; // nitf::Field will implicitly cast
        TEST_ASSERT_EQ(std::string("1234"), value);
    }
}

TEST_MAIN(
    TEST_CHECK(testCastOperator);
    TEST_CHECK(testDescriptors);
    )
