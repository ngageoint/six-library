/* =========================================================================
* This file is part of six.sidd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
*
* six-c++ is free software; you can redistribute it and/or modify
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
#include <six/Enums.h>
#include <six/Utilities.h>

TEST_CASE(EnumConstructor)
{
    six::DualPolarizationType pType("UNKNOWN");
    TEST_ASSERT_EQ(pType, "UNKNOWN");
}

TEST_CASE(ToType)
{
    six::DualPolarizationType fromToType = six::toType<six::DualPolarizationType>("UNKNOWN");
    six::DualPolarizationType fromCtor("UNKNOWN");
    TEST_ASSERT_EQ(fromToType, fromCtor);
}

TEST_CASE(SixToString)
{
    six::DualPolarizationType pType("UNKNOWN");
    const std::string polarizationString = pType;
    TEST_ASSERT_EQ("UNKNOWN", polarizationString);
}

TEST_CASE(NotSet)
{
    six::DualPolarizationType pType;
    std::string polarizationString = pType;
    TEST_EXCEPTION(polarizationString = pType.toString(true /*throw_if_not_set*/));
}

TEST_CASE(EqInt)
{
    const six::DualPolarizationType fromStrCtor("UNKNOWN");
    TEST_ASSERT_EQ(18, fromStrCtor);
    const int value = fromStrCtor;
    TEST_ASSERT_EQ(18, value);

    const six::DualPolarizationType fromIntCtor(value);
    TEST_ASSERT_EQ(18, fromIntCtor);
    TEST_ASSERT_EQ(fromIntCtor, "UNKNOWN");

    TEST_ASSERT_EQ(fromStrCtor, fromIntCtor);
}

TEST_MAIN((void)argv; (void)argc;
    TEST_CHECK(EnumConstructor);
    TEST_CHECK(ToType);
    TEST_CHECK(SixToString);
    TEST_CHECK(NotSet);
    TEST_CHECK(EqInt);
    )
