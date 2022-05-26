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
    const auto test = [&](const std::string& strType, six::DualPolarizationType type)
    {
        six::DualPolarizationType pType(strType);
        TEST_ASSERT(pType == strType);
        TEST_ASSERT_EQ(pType, type);
    };
    test("UNKNOWN", six::DualPolarizationType::UNKNOWN);
    test("V_V", six::DualPolarizationType::V_V);
    test("E_V", six::DualPolarizationType::E_V); // SICD 1.3
}

TEST_CASE(ToType)
{
    const auto test = [&](const std::string& strType, const std::string& strCtorType, six::DualPolarizationType type)
    {
        auto fromToType = six::toType<six::DualPolarizationType>(strType);
        TEST_ASSERT_EQ(fromToType, type);

        six::DualPolarizationType fromCtor(strCtorType);
        TEST_ASSERT_EQ(fromToType, fromCtor);
    };
    test("UNKNOWN", "UNKNOWN", six::DualPolarizationType::UNKNOWN);
    test("V:V", "V_V", six::DualPolarizationType::V_V);
    test("E:V", "E_V", six::DualPolarizationType::E_V); // SICD 1.3
}

TEST_CASE(SixToString)
{
    const auto test = [&](const std::string& strType, const std::string& strCtorType, six::DualPolarizationType type)
    {
        six::DualPolarizationType pType(strCtorType);
        TEST_ASSERT_EQ(pType, type);
        std::string polarizationString = pType;
        TEST_ASSERT_EQ(strCtorType, polarizationString);

        polarizationString = six::toString(pType);
        TEST_ASSERT_EQ(strType, polarizationString);
    };
    test("UNKNOWN", "UNKNOWN", six::DualPolarizationType::UNKNOWN);
    test("V:V", "V_V", six::DualPolarizationType::V_V);
    test("E:V", "E_V", six::DualPolarizationType::E_V); // SICD 1.3
}

TEST_CASE(NotSet)
{
    six::DualPolarizationType pType;
    std::string polarizationString = pType;
    TEST_EXCEPTION(polarizationString = pType.toString(true /*throw_if_not_set*/));
}

TEST_CASE(EqInt)
{
    const auto test = [&](const std::string& strType, six::DualPolarizationType type, int enumValue)
    {
        const six::DualPolarizationType fromStrCtor(strType);
        TEST_ASSERT_EQ(enumValue, fromStrCtor);
        const int value = fromStrCtor;
        TEST_ASSERT_EQ(enumValue, value);

        const six::DualPolarizationType fromIntCtor(value);
        TEST_ASSERT_EQ(enumValue, fromIntCtor);
        TEST_ASSERT(fromIntCtor == strType);
        TEST_ASSERT_EQ(fromIntCtor, type);

        TEST_ASSERT_EQ(fromStrCtor, fromIntCtor);
    };
    test("UNKNOWN", six::DualPolarizationType::UNKNOWN, 18);
    test("V_V", six::DualPolarizationType::V_V, 2);
    test("E_V", six::DualPolarizationType::E_V, 51); // SICD 1.3
}

TEST_MAIN(
    TEST_CHECK(EnumConstructor);
    TEST_CHECK(ToType);
    TEST_CHECK(SixToString);
    TEST_CHECK(NotSet);
    TEST_CHECK(EqInt);
    )
