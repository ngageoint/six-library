/* =========================================================================
* This file is part of six.sicd-c++
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
#include <import/six/sicd.h>

#include "TestCase.h"

static std::filesystem::path argv0()
{
    static const sys::OS os;
    static const auto retval = os.getSpecialEnv("0");
    return retval;
}

static inline std::filesystem::path six_sicd_relative_path()
{
    return std::filesystem::path("six") / "modules" / "c++" / "six.sicd";
}
static std::filesystem::path sample_xml_relative_path(const std::filesystem::path& filename)
{
    return six_sicd_relative_path() / "tests" / "sample_xml" / filename;
}
static std::filesystem::path schema_relative_path()
{
    return six_sicd_relative_path() / "conf" / "schema";
}

static std::filesystem::path get_sample_xml_path(const std::filesystem::path& filename)
{
    const auto root_dir = six::testing::buildRootDir(argv0());
    return root_dir / sample_xml_relative_path(filename);
}

static std::vector<std::filesystem::path> getSchemaPaths()
{
    const auto root_dir = six::testing::buildRootDir(argv0());
    return std::vector<std::filesystem::path> { (root_dir / schema_relative_path()) };
}

static std::unique_ptr<six::sicd::ComplexData> test_assert_round_trip(const std::string& testName,
    const six::sicd::ComplexData& complexData, const std::vector<std::filesystem::path>* pSchemaPaths)
{
    auto strXML = six::sicd::Utilities::toXMLString(complexData, pSchemaPaths);
    TEST_ASSERT_FALSE(strXML.empty());
    return six::sicd::Utilities::parseDataFromString(strXML, pSchemaPaths);
}

inline static const six::UnmodeledS* get_Unmodeled(const six::sicd::ComplexData& complexData, const std::string& strVersion)
{
    if (strVersion != "1.3.0") // Unmodeled added in SICD 1.3
    {
        return nullptr;
    }
    else
    {
        return complexData.errorStatistics->Unmodeled.get();
    }
}

static void test_createFakeComplexData_(const std::string& testName, const std::string& strVersion)
{
    const auto pFakeComplexData = six::sicd::Utilities::createFakeComplexData(strVersion, six::PixelType::RE32F_IM32F, false /*makeAmplitudeTable*/);
    auto Unmodeled = get_Unmodeled(*pFakeComplexData, strVersion);
    TEST_ASSERT_NULL(Unmodeled); // not part of the fake data, only added in SICD 1.3

    // NULL schemaPaths, no validation
    auto pComplexData = test_assert_round_trip(testName , *pFakeComplexData, nullptr /*pSchemaPaths*/);
    Unmodeled = get_Unmodeled(*pComplexData, strVersion);
    TEST_ASSERT_NULL(Unmodeled);  // not part of the fake data, only added in SICD 1.3

    // validate XML against schema
    const auto schemaPaths = getSchemaPaths();
    pComplexData = test_assert_round_trip(testName , *pFakeComplexData, &schemaPaths);
    Unmodeled = get_Unmodeled(*pComplexData, strVersion);
    TEST_ASSERT_NULL(Unmodeled);  // not part of the fake data, only added in SICD 1.3
}

TEST_CASE(test_createFakeComplexData)
{
    test_createFakeComplexData_(testName, "1.2.1");
    test_createFakeComplexData_(testName, "1.3.0");
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
static void test_assert(const std::string& testName, const six::sicd::ComplexData& complexData)
{
    auto&& errorStatistics = complexData.errorStatistics;
    if (complexData.getVersion() != "1.3.0")
    {
        TEST_ASSERT_NULL(errorStatistics.get());
        return;
    }
    TEST_ASSERT(errorStatistics.get() != nullptr);
    auto Unmodeled = errorStatistics->Unmodeled;
    TEST_ASSERT(Unmodeled.get() != nullptr);
    test_assert_unmodeled_(testName, *Unmodeled);

    // for SICD 1.3, also check the polarization type; this is set either in the fake data or scid130.xml
    const auto txRcvPolarizationProc = complexData.imageFormation->txRcvPolarizationProc;
    TEST_ASSERT_EQ(txRcvPolarizationProc, six::DualPolarizationType::OTHER_OTHER);
    const auto strTxRcvPolarizationProc = six::toString(txRcvPolarizationProc);
    TEST_ASSERT_EQ(strTxRcvPolarizationProc,"OTHER_TxRcvPolarizationProc:OTHER_TxRcvPolarizationProc");
}

static void test_read_sicd_xml(const std::string& testName, const std::filesystem::path& path)
{
    const auto pathname = get_sample_xml_path(path);

    // NULL schemaPaths, no validation
    auto pComplexData = six::sicd::Utilities::parseDataFromFile(pathname, nullptr /*pSchemaPaths*/);
    test_assert(testName, *pComplexData);

    pComplexData = test_assert_round_trip(testName , *pComplexData, nullptr /*pSchemaPaths*/);
    test_assert(testName, *pComplexData);

    // validate XML against schema
    const auto schemaPaths = getSchemaPaths();
    pComplexData = six::sicd::Utilities::parseDataFromFile(pathname, &schemaPaths);
    test_assert(testName, *pComplexData);

    pComplexData = test_assert_round_trip(testName, *pComplexData, &schemaPaths);
    test_assert(testName, *pComplexData);
}

TEST_CASE(test_read_sicd110_xml)
{
    test_read_sicd_xml(testName, "sicd110.xml");
}

TEST_CASE(test_read_sicd130_xml)
{
    test_read_sicd_xml(testName, "sicd130.xml");
}

TEST_MAIN(
    TEST_CHECK(test_createFakeComplexData);
    TEST_CHECK(test_read_sicd110_xml);
    TEST_CHECK(test_read_sicd110_xml);
    )
