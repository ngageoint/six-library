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

namespace fs = std::filesystem;

static fs::path argv0()
{
    static const sys::OS os;
    static const fs::path retval = os.getSpecialEnv("0");
    return retval;
}

inline fs::path six_sidd_relative_path()
{
    return fs::path("six") / "modules" / "c++" / "six.sidd";
}
static fs::path sample_xml_relative_path(const fs::path& filename)
{
    return six_sidd_relative_path() / "tests" / "sample_xml" / filename;
}
static fs::path schema_relative_path()
{
    return six_sidd_relative_path() / "conf" / "schema";
}

inline fs::path get_sample_xml_path(const fs::path& filename)
{
    const auto root_dir = six::testing::buildRootDir(argv0());
    return root_dir / sample_xml_relative_path(filename);
}

inline std::vector<std::filesystem::path> getSchemaPaths()
{
    const auto root_dir = six::testing::buildRootDir(argv0());
    return std::vector<std::filesystem::path> { (root_dir / schema_relative_path()) };
}

static std::string testName;

static std::unique_ptr<six::sidd::DerivedData> test_assert_round_trip(const six::sidd::DerivedData& derivedData, const std::vector<fs::path>* pSchemaPaths)
{
    auto strXML = six::sidd::Utilities::toXMLString(derivedData, pSchemaPaths);
    TEST_ASSERT_FALSE(strXML.empty());
    return six::sidd::Utilities::parseDataFromString(strXML, pSchemaPaths);
}

TEST_CASE(test_createFakeDerivedData)
{
    const auto pFakeDerivedData = six::sidd::Utilities::createFakeDerivedData("3.0.0");
    auto Unmodeled = pFakeDerivedData->errorStatistics->Unmodeled;
    TEST_ASSERT_NULL(Unmodeled.get());

    // NULL schemaPaths, no validation
    auto pDerivedData = test_assert_round_trip(*pFakeDerivedData, nullptr /*pSchemaPaths*/);
    Unmodeled = pDerivedData->errorStatistics->Unmodeled;
    TEST_ASSERT_NULL(Unmodeled.get());

    // validate XML against schema
    const auto schemaPaths = getSchemaPaths();
    pDerivedData = test_assert_round_trip(*pFakeDerivedData, &schemaPaths);
    Unmodeled = pDerivedData->errorStatistics->Unmodeled;
    TEST_ASSERT_NULL(Unmodeled.get());
}

static void test_assert_unmodeled(const six::UnmodeledS& Unmodeled)
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

TEST_CASE(test_read_sidd300_xml)
{
    const auto pathname = get_sample_xml_path("sidd300.xml");

    // NULL schemaPaths, no validation
    auto pDerivedData = six::sidd::Utilities::parseDataFromFile(pathname, nullptr /*pSchemaPaths*/);
    auto Unmodeled = pDerivedData->errorStatistics->Unmodeled;
    TEST_ASSERT(Unmodeled.get() != nullptr);
    test_assert_unmodeled(*Unmodeled);

    pDerivedData = test_assert_round_trip(*pDerivedData, nullptr /*pSchemaPaths*/);
    Unmodeled = pDerivedData->errorStatistics->Unmodeled;
    TEST_ASSERT(Unmodeled.get() != nullptr);
    test_assert_unmodeled(*Unmodeled);

    // validate XML against schema
    const auto schemaPaths = getSchemaPaths();
    pDerivedData = six::sidd::Utilities::parseDataFromFile(pathname, &schemaPaths);
    Unmodeled = pDerivedData->errorStatistics->Unmodeled;
    TEST_ASSERT(Unmodeled.get() != nullptr);
    test_assert_unmodeled(*Unmodeled);

    pDerivedData = test_assert_round_trip(*pDerivedData, &schemaPaths);
    Unmodeled = pDerivedData->errorStatistics->Unmodeled;
    TEST_ASSERT(Unmodeled.get() != nullptr);
    test_assert_unmodeled(*Unmodeled);
}

TEST_MAIN((void)argc; (void)argv;
    TEST_CHECK(test_createFakeDerivedData);
    TEST_CHECK(test_read_sidd300_xml);
    )
