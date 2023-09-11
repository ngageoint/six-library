/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
*
* six.sicd-c++ is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this program; If not,
* see <http://www.gnu.org/licenses/>.
*
*/

#include <string>
#include <vector>
#include <std/optional>

#include <xml/lite/MinidomParser.h>
#include <xml/lite/Document.h>
#include <io/StringStream.h>
#include <str/Convert.h>

#include <import/six/sicd.h>
#include <six/XmlLite.h>
#include <six/sicd/DataParser.h>

#ifdef _MSC_VER
#pragma warning(disable: 4464) // relative include path contains '..'
#endif
#include "../tests/TestUtilities.h"

#include "TestCase.h"

static void test_DummyData_parameters(const std::string& testName, const six::ParameterCollection& parameters,
    std::optional<bool> preserveCharacterData = std::optional<bool>())
{
    const auto& testParameter = parameters.findParameter("TestParameter");
    TEST_ASSERT_EQ(testParameter.str(), "setValue() for TestParameter");

    // Check whitepspace in parameters
    const auto& emptyParameter = parameters.findParameter("TestParameterEmpty");
    TEST_ASSERT_EQ(emptyParameter.str(), "");

    const auto& threeSpacesParameter = parameters.findParameter("TestParameterThreeSpaces");
    std::string expected("   ");
    if (preserveCharacterData.has_value()) // only has a value when parsing XML
    {
        if (!(*preserveCharacterData))
        {
            expected.clear(); // result of trim() is empty string
        }
    }
    TEST_ASSERT_EQ(threeSpacesParameter.str(), expected);
}
TEST_CASE(DummyData)
{
    const auto data = createData<six::zfloat>(types::RowCol<size_t>(10, 10));

    test_DummyData_parameters(testName, data->collectionInformation->parameters);

    const std::vector<std::filesystem::path>* pSchemaPaths = nullptr;
    six::sicd::DataParser parser(pSchemaPaths);

    const auto xmlStr = parser.toXML(*data);
    TEST_ASSERT_FALSE(xmlStr.empty());

    // Parse the XML we just made.
    {
        constexpr bool preserveCharacterData = false;
        parser.preserveCharacterData(preserveCharacterData);
        const auto pComplexData = parser.fromXML(xmlStr);
        test_DummyData_parameters(testName, pComplexData->collectionInformation->parameters, preserveCharacterData);
    }
    {
        constexpr bool preserveCharacterData = true;
        parser.preserveCharacterData(preserveCharacterData);
        const auto pComplexData = parser.fromXML(xmlStr);
        test_DummyData_parameters(testName, pComplexData->collectionInformation->parameters, preserveCharacterData);
    }
}

TEST_CASE(FakeComplexData)
{
    const auto data = six::sicd::Utilities::createFakeComplexData();

    const std::vector<std::string> schemaPaths;
    const auto result = six::sicd::Utilities::toXMLString(*data, nullptr);
    TEST_ASSERT_FALSE(result.empty());
}

TEST_CASE(Classification)
{
    const std::string classificationText("UNCLASSIFIED");

    auto data = createData<six::zfloat>(types::RowCol<size_t>(10, 10));
    data->collectionInformation->setClassificationLevel(classificationText);
    TEST_ASSERT_TRUE(data->getClassification().isUnclassified());

    io::U8StringStream ss;
    ss.stream() << six::sicd::Utilities::toXMLString(*data, nullptr /*pSchemaPaths*/);

    six::MinidomParser xmlParser;
    xmlParser.parse(ss);
    const auto& doc = getDocument(xmlParser);
    const auto root = doc.getRootElement();
    TEST_ASSERT(root != nullptr);

    const auto classificationElements = root->getElementsByTagName("Classification", true /*recurse*/);
    TEST_ASSERT_FALSE(classificationElements.empty());
    TEST_ASSERT_EQ(classificationElements.size(), static_cast<size_t>(1));
    const auto& classification = *(classificationElements[0]);
    const auto characterData = classification.getCharacterData();
    TEST_ASSERT_EQ(characterData, classificationText);
}

TEST_CASE(ClassificationCanada)
{
#ifdef _WIN32
    const std::string classificationText("NON CLASSIFI\xc9 / UNCLASSIFIED"); // ISO8859-1 "NON CLASSIFIÉ / UNCLASSIFIED"
    const auto E_ = str::u8FromNative("\xc9"); // ISO8859-1  "É"
#else
    const std::string classificationText("NON CLASSIFI\xc3\x89 / UNCLASSIFIED"); // UTF-8 "NON CLASSIFIÉ / UNCLASSIFIED"
    const auto E_ = str::u8FromNative("\xc3\x89"); // UTF-8  "É"
#endif

    auto data = createData<six::zfloat>(types::RowCol<size_t>(10, 10));
    data->collectionInformation->setClassificationLevel(classificationText);
    TEST_ASSERT_TRUE(data->getClassification().isUnclassified());

    const auto strXml = six::sicd::Utilities::toXMLString(*data, nullptr /*pSchemaPaths*/);

    const auto NON_CLASSIFI = strXml.find(str::u8FromNative("NON CLASSIFI"));
    TEST_ASSERT(NON_CLASSIFI != std::string::npos);
    const auto UNCLASSIFIED = strXml.find(str::u8FromNative(" / UNCLASSIFIED"));
    TEST_ASSERT(UNCLASSIFIED != std::string::npos);
    const auto utf8 = strXml.substr(NON_CLASSIFI, UNCLASSIFIED - NON_CLASSIFI);
    TEST_ASSERT_EQ(utf8.size(), std::string("NON CLASSIFI\xc3\x89").size()); // UTF-8, "NON CLASSIFIÉ"
    const auto posE_ = utf8.find(E_);
    TEST_ASSERT(posE_ != std::string::npos);

    io::U8StringStream ss;
    ss.stream() << strXml;
    six::MinidomParser xmlParser;
    xmlParser.parse(ss);
    const auto& doc = getDocument(xmlParser);
    const auto root = doc.getRootElement();
    TEST_ASSERT(root != nullptr);

    const auto classificationElements = root->getElementsByTagName("Classification", true /*recurse*/);
    TEST_ASSERT_FALSE(classificationElements.empty());
    TEST_ASSERT_EQ(classificationElements.size(), static_cast<size_t>(1));
    const auto& classification = *(classificationElements[0]);
    const auto characterData = classification.getCharacterData();
    TEST_ASSERT_EQ(characterData, classificationText);
}

TEST_MAIN(
    TEST_CHECK(DummyData);
    TEST_CHECK(FakeComplexData);
    TEST_CHECK(Classification);
    TEST_CHECK(ClassificationCanada);
    )

