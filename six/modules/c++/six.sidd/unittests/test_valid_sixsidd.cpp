/* =========================================================================
* This file is part of six.sidd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
* (C) Copyright 2021, Maxar Technologies, Inc.
*
* six.sicd-c++ is free software; you can redistribute it and/or modify
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

#include <stdlib.h>

#include <string>
#include <string>
#include <std/filesystem>
#include <std/span>

#include <io/FileInputStream.h>
#include <logging/NullLogger.h>
#include <import/sys.h>

#include <six/Utilities.h>
#include <import/six/sidd.h>

#include "TestCase.h"

#if _MSC_VER
#pragma warning(disable: 4459) //  declaration of '...' hides global declaration
#endif

static std::filesystem::path argv0()
{
    static const sys::OS os;
    static const auto retval = os.getSpecialEnv("0");
    return retval;
}

static inline std::filesystem::path six_sidd_relative_path()
{
    return std::filesystem::path("six") / "modules" / "c++" / "six.sidd";
}
static std::filesystem::path schema_relative_path()
{
    return six_sidd_relative_path() / "conf" / "schema";
}

static std::filesystem::path get_sample_xml_path(const std::filesystem::path& filename)
{
    static const auto modulePath = six_sidd_relative_path() / "tests" / "sample_xml";
    return sys::test::findGITModuleFile("six", modulePath, filename);
}

static std::vector<std::filesystem::path> getSchemaPaths()
{
    static const auto filename = sys::test::findGITModuleFile("six", schema_relative_path(), "SIDD_schema_V2.0.0_2020_06_02.xsd");
    static const auto schemaPath = filename.parent_path();
    return std::vector<std::filesystem::path> { schemaPath };
}

static std::unique_ptr<six::sidd::DerivedData> test_assert_round_trip(const std::string& testName,
    const six::sidd::DerivedData& derivedData, const std::vector<std::filesystem::path>* pSchemaPaths)
{
    auto strXML = six::sidd::Utilities::toXMLString(derivedData, pSchemaPaths);
    TEST_ASSERT_FALSE(strXML.empty());
    return six::sidd::Utilities::parseDataFromString(strXML, pSchemaPaths);
}

TEST_CASE(test_createFakeDerivedData)
{
    const auto pFakeDerivedData = six::sidd::Utilities::createFakeDerivedData("2.0.0");

    // NULL schemaPaths, no validation
    auto pDerivedData = test_assert_round_trip(testName, *pFakeDerivedData, nullptr /*pSchemaPaths*/);

    // validate XML against schema
    const auto schemaPaths = getSchemaPaths();
    pDerivedData = test_assert_round_trip(testName, *pFakeDerivedData, &schemaPaths);
}

static void test_assert_unmodeled(const std::string& testName, const six::sidd::DerivedData& derivedData)
{
    auto&& errorStatistics = derivedData.errorStatistics;
    TEST_ASSERT(errorStatistics.get() != nullptr); 
    return;
}
TEST_CASE(test_read_sidd200_xml)
{
    const auto pathname = get_sample_xml_path("sidd200.xml");

    // NULL schemaPaths, no validation
    auto pDerivedData = six::sidd::Utilities::parseDataFromFile(pathname, nullptr /*pSchemaPaths*/);
    test_assert_unmodeled(testName, *pDerivedData);

    pDerivedData = test_assert_round_trip(testName, *pDerivedData, nullptr /*pSchemaPaths*/);
    test_assert_unmodeled(testName, *pDerivedData);

    // validate XML against schema
    const auto schemaPaths = getSchemaPaths();
    pDerivedData = six::sidd::Utilities::parseDataFromFile(pathname, &schemaPaths);
    test_assert_unmodeled(testName, *pDerivedData);

    pDerivedData = test_assert_round_trip(testName, *pDerivedData, &schemaPaths);
    test_assert_unmodeled(testName, *pDerivedData);
}

TEST_MAIN(
    TEST_CHECK(test_createFakeDerivedData);
    TEST_CHECK(test_read_sidd200_xml);
    )
