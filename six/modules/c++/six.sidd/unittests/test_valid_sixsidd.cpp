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

static inline std::filesystem::path six_sidd_relative_path()
{
    return std::filesystem::path("six") / "modules" / "c++" / "six.sidd";
}
static std::filesystem::path schema_relative_path()
{
    return six_sidd_relative_path() / "conf" / "schema"; // .../conf/schema
}

static std::filesystem::path get_sample_xml_path(const std::filesystem::path& filename)
{
    static const auto modulePath = six_sidd_relative_path() / "tests" / "sample_xml";
    return sys::test::findGITModuleFile("six", modulePath, filename);
}

static std::vector<std::filesystem::path> getSchemaPaths()
{
    static const auto xsdPath = sys::test::findGITModuleFile("six", schema_relative_path(), "SIDD_schema_V3.0.0.xsd");
    static const auto rootSchemaDir = xsdPath.parent_path(); // ".../conf/Schema"
    return std::vector<std::filesystem::path> { rootSchemaDir };
}

static std::unique_ptr<six::sidd::DerivedData> test_assert_round_trip(const std::string& testName,
    const six::sidd::DerivedData& derivedData, const std::vector<std::filesystem::path>* pSchemaPaths)
{
    const auto strXML = six::sidd::Utilities::toXMLString(derivedData, pSchemaPaths);
    TEST_ASSERT_FALSE(strXML.empty());

    const auto xml_ = str::EncodedStringView(strXML).native(); // for debugging
    TEST_ASSERT_FALSE(xml_.empty());

    return six::sidd::Utilities::parseDataFromString(strXML, pSchemaPaths);
}

TEST_CASE(test_createFakeDerivedData)
{
    if (strVersion != "3.0.0") // Unmodeled added in SIDD 3.0
    {
        return nullptr;
    }
    else
    {
        return derivedData.errorStatistics->Unmodeled.get();
    }
}

static void test_createFakeDerivedData_(const std::string& testName, const std::string& strVersion, bool validate)
{
    const auto pFakeDerivedData = six::sidd::Utilities::createFakeDerivedData(strVersion);
    auto Unmodeled = get_Unmodeled(*pFakeDerivedData, strVersion);
    TEST_ASSERT_NULL(Unmodeled); // not part of the fake data, only added in SIDD 3.0

    const auto schemaPaths = getSchemaPaths();
    const std::vector<std::filesystem::path>* pSchemaPaths = validate ? & schemaPaths: nullptr; // NULL schemaPaths, no validation

    auto pDerivedData = test_assert_round_trip(testName, *pFakeDerivedData, pSchemaPaths);
    Unmodeled = get_Unmodeled(*pDerivedData, strVersion);
    TEST_ASSERT_NULL(Unmodeled);  // not part of the fake data, only added in SIDD 3.0
}
TEST_CASE(test_createFakeDerivedData)
{
    test_createFakeDerivedData_(testName, "2.0.0", false /*validate*/);
    test_createFakeDerivedData_(testName, "3.0.0", false /*validate*/);
}
TEST_CASE(test_createFakeDerivedData_validate)
{
    test_createFakeDerivedData_(testName, "2.0.0", true /*validate*/);
    test_createFakeDerivedData_(testName, "3.0.0", true /*validate*/);
}

static void test_assert_unmodeled_(const std::string& testName, const six::UnmodeledS& Unmodeled)
{
    TEST_ASSERT_EQ(1.23, Unmodeled.Xrow);
    TEST_ASSERT_EQ(4.56, Unmodeled.Ycol);
    TEST_ASSERT_EQ(7.89, Unmodeled.XrowYcol);

    const auto& UnmodeledDecor = Unmodeled.UnmodeledDecorr;
    TEST_ASSERT(UnmodeledDecor.get() != nullptr);
    TEST_ASSERT_EQ(12.34, UnmodeledDecor->Xrow.CorrCoefZero);
    TEST_ASSERT_EQ(56.78, UnmodeledDecor->Xrow.DecorrRate);
    TEST_ASSERT_EQ(123.4, UnmodeledDecor->Ycol.CorrCoefZero);
    TEST_ASSERT_EQ(567.8, UnmodeledDecor->Ycol.DecorrRate);
}
static void test_assert_unmodeled(const std::string& testName, const six::sidd::DerivedData& derivedData)
{
    auto&& errorStatistics = derivedData.errorStatistics;
    TEST_ASSERT(errorStatistics.get() != nullptr); 
    return;
}

static void test_read_sidd_xml(const std::string& testName, const std::filesystem::path& path,
    const std::vector<std::filesystem::path>* pSchemaPaths)
{
    const auto pathname = get_sample_xml_path("sidd200.xml");

    auto pDerivedData = six::sidd::Utilities::parseDataFromFile(pathname, pSchemaPaths);
    test_assert_unmodeled(testName, *pDerivedData);

    pDerivedData = test_assert_round_trip(testName, *pDerivedData, pSchemaPaths);
    test_assert_unmodeled(testName, *pDerivedData);
}
static void test_read_sidd_xml(const std::string& testName, const std::filesystem::path& path)
{
    const std::vector<std::filesystem::path>* pSchemaPaths = nullptr; // NULL schemaPaths, no validation
    test_read_sidd_xml(testName, path, pSchemaPaths);

    // validate XML against schema
    const auto schemaPaths = getSchemaPaths();
    test_read_sidd_xml(testName, path, &schemaPaths);
}
TEST_CASE(test_read_sidd200_xml)
{
    test_read_sidd_xml(testName, "sidd200.xml");
}
TEST_CASE(test_read_sidd300_xml)
{
    test_read_sidd_xml(testName, "sidd300.xml");
}

TEST_MAIN(
    TEST_CHECK(test_createFakeDerivedData);
    TEST_CHECK(test_createFakeDerivedData_validate);
    TEST_CHECK(test_read_sidd200_xml);
    )
