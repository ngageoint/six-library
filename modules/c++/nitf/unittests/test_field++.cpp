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

#include <config/coda_oss_config.h>
#include <nitf/Field.hpp>
#include "TestCase.h"

namespace
{
TEST_CASE(testCastOperator)
{
    nitf_Error error;
    nitf::Field field(nitf_Field_construct(20, NITF_BCS_A, &error));

    // Test unsigned values
    field.set(123);
    const nitf::Uint8 valUint8 = field;
    TEST_ASSERT_EQ(valUint8, 123);

    field.set(12345);
    const nitf::Uint16 valUint16 = field;
    TEST_ASSERT_EQ(valUint16, 12345);

    field.set(1234567890);
    const nitf::Uint32 valUint32 = field;
    TEST_ASSERT_EQ(valUint32, 1234567890);
#if SIZEOF_SIZE_T == 4
    const size_t valSizeT = field;
    TEST_ASSERT_EQ(valSizeT, 1234567890);
#endif

    field.set(nitf::Uint64(1234567890987));
    const nitf::Uint64 valUint64 = field;
    TEST_ASSERT_EQ(valUint64, 1234567890987);

#if SIZEOF_SIZE_T == 8
    const size_t valSizeT = field;
    TEST_ASSERT_EQ(valSizeT, 1234567890987);
#endif

    // Test signed values
    field.set(-123);
    const nitf::Int8 valInt8 = field;
    TEST_ASSERT_EQ(valInt8, -123);

    field.set(-12345);
    const nitf::Int16 valInt16 = field;
    TEST_ASSERT_EQ(valInt16, -12345);

    field.set(-1234567890);
    const nitf::Int32 valInt32 = field;
    TEST_ASSERT_EQ(valInt32, -1234567890);
#if SIZEOF_SIZE_T == 4
    const size_t valSSizeT = field;
    TEST_ASSERT_EQ(valSSizeT, static_cast<size_t>(-1234567890));
#endif

    // TODO: I think the %lld isn't working, at least in VS, in
    //       nitf_Field_setInt64(), so need to do this via string
    field.set("-1234567890987");
    const nitf::Int64 valInt64 = field;
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
    TEST_ASSERT_EQ(valStr, "ABCxyz              ");
}
}

int main(int , char** )
{
    TEST_CHECK(testCastOperator);
    return 0;
}
