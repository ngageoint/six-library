/* =========================================================================
 * This file is part of io-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * io-c++ is free software; you can redistribute it and/or modify
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

#include <string>

#include "io/StringStream.h"
#include <TestCase.h>

#include "xml/lite/MinidomParser.h"

static const std::string text = "TEXT";
static const std::string strXml1_ = R"(
<root>
    <doc name="doc">
        <a a="a">)";
static const std::string strXml2_ = R"(</a><b/><b/>
        <values int="314" double="3.14" string="abc"/>
        <int>314</int>
        <double>3.14</double>
        <string>abc</string>
        <empty></empty>
    </doc>
</root>)";
static const auto strXml = strXml1_ + text + strXml2_;

struct test_MinidomParser final
{
    xml::lite::MinidomParser xmlParser;
    const xml::lite::Element* getRootElement()
    {
        io::StringStream ss;
        ss.stream() << strXml;

        xmlParser.parse(ss);
        const auto doc = xmlParser.getDocument();
        return doc->getRootElement();    
    }
};

TEST_CASE(test_getRootElement)
{
    io::StringStream ss;
    ss.stream() << strXml;
    TEST_ASSERT_EQ(ss.stream().str(), strXml);

    xml::lite::MinidomParser xmlParser;
    xmlParser.parse(ss);
    const auto doc = xmlParser.getDocument();
    TEST_ASSERT(doc != nullptr);
    const auto root = doc->getRootElement();
    TEST_ASSERT(root != nullptr);
}

TEST_CASE(test_getElementsByTagName)
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();
    
    {
        const auto aElements = root->getElementsByTagName("a", true /*recurse*/);
        TEST_ASSERT_EQ(aElements.size(), 1);
        const auto& a = *(aElements[0]);

        const auto characterData = a.getCharacterData();
        TEST_ASSERT_EQ(characterData, text);
    }
    
    const auto docElements = root->getElementsByTagName("doc");
    TEST_ASSERT_FALSE(docElements.empty());
    TEST_ASSERT_EQ(docElements.size(), 1);
    {
        const auto aElements = docElements[0]->getElementsByTagName("a");
        TEST_ASSERT_EQ(aElements.size(), 1);
        const auto& a = *(aElements[0]);

        const auto characterData = a.getCharacterData();
        TEST_ASSERT_EQ(characterData, text);
        const auto pEncoding = a.getEncoding();
        TEST_ASSERT_NULL(pEncoding);
    }
}

TEST_CASE(test_getElementsByTagName_b)
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    {
        const auto bElements = root->getElementsByTagName("b", true /*recurse*/);
        TEST_ASSERT_EQ(bElements.size(), 2);
        const auto& b = *(bElements[0]);

        const auto characterData = b.getCharacterData();
        TEST_ASSERT_TRUE(characterData.empty());
    }

    const auto docElements = root->getElementsByTagName("doc");
    TEST_ASSERT_FALSE(docElements.empty());
    TEST_ASSERT_EQ(docElements.size(), 1);
    {
        const auto bElements = docElements[0]->getElementsByTagName("b");
        TEST_ASSERT_EQ(bElements.size(), 2);
        const auto& b = *(bElements[0]);

        const auto characterData = b.getCharacterData();
        TEST_ASSERT_TRUE(characterData.empty());
    }
}

TEST_CASE(test_getElementByTagName)
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    {
        const auto& a = root->getElementByTagName("a", true /*recurse*/);
        const auto characterData = a.getCharacterData();
        TEST_ASSERT_EQ(characterData, text);
    }

    const auto& doc = root->getElementByTagName("doc");
    {
        const auto& a = doc.getElementByTagName("a");
        const auto characterData = a.getCharacterData();
        TEST_ASSERT_EQ(characterData, text);
    }
}

TEST_CASE(test_getElementByTagName_nothrow)
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    {
        const auto pNotFound = root->getElementByTagName(std::nothrow, "not_found", true /*recurse*/);
        TEST_ASSERT_NULL(pNotFound);
    }

    const auto& doc = root->getElementByTagName("doc");
    {
        const auto pNotFound = doc.getElementByTagName(std::nothrow, "not_found");
        TEST_ASSERT_NULL(pNotFound);
    }
}

TEST_CASE(test_getElementByTagName_b)
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();
    
    TEST_SPECIFIC_EXCEPTION(root->getElementByTagName("b", true /*recurse*/), xml::lite::XMLException);

    const auto& doc = root->getElementByTagName("doc");
    TEST_SPECIFIC_EXCEPTION(doc.getElementByTagName("b"), xml::lite::XMLException);
}

TEST_CASE(test_getValue)
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    {
        const auto& e = root->getElementByTagName("int", true /*recurse*/);
        int value;
        const auto result = getValue(e, value);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ(314, value);
    }
    {
        const auto& e = root->getElementByTagName("double", true /*recurse*/);
        double value;
        const auto result = getValue(e, value);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ(3.14, value);
    }
    {
        const auto& e = root->getElementByTagName("string", true /*recurse*/);
        std::string value;
        const auto result = getValue(e, value);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ("abc", value);
    }
    {
        const auto& e = root->getElementByTagName("empty", true /*recurse*/);
        std::string value;
        auto result = getValue(e, value);
        TEST_ASSERT_FALSE(result);
        value = e.getCharacterData();
        TEST_ASSERT_TRUE(value.empty());
    }
}


TEST_CASE(test_getValueFailure)
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    {
        const auto& e = root->getElementByTagName("string", true /*recurse*/);
        int value;
        const auto result = getValue(e, value);
        TEST_ASSERT_FALSE(result);
    }
    {
        const auto& e = root->getElementByTagName("string", true /*recurse*/);
        double value;
        const auto result = getValue(e, value);
        TEST_ASSERT_FALSE(result);
    }
}

TEST_CASE(test_setValue)
{
    test_MinidomParser xmlParser;
    auto root = xmlParser.getRootElement();

    {
        auto& e = root->getElementByTagName("int", true /*recurse*/);
        setValue(e, 123);
        int value;
        const auto result = getValue(e, value);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ(123, value);
    }
    {
        auto& e = root->getElementByTagName("double", true /*recurse*/);
        setValue(e, 1.23);
        double value;
        const auto result = getValue(e, value);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ(1.23, value);
    }
    {
        auto& e = root->getElementByTagName("string", true /*recurse*/);
        setValue(e, "xyz");
        std::string value;
        const auto result = getValue(e, value);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ("xyz", value);
    }
}

int main(int, char**)
{
    TEST_CHECK(test_getRootElement);
    TEST_CHECK(test_getElementsByTagName);
    TEST_CHECK(test_getElementsByTagName_b);
    TEST_CHECK(test_getElementByTagName);
    TEST_CHECK(test_getElementByTagName_nothrow);
    TEST_CHECK(test_getElementByTagName_b);

    TEST_CHECK(test_getValue);
    TEST_CHECK(test_getValueFailure);
    TEST_CHECK(test_setValue);
}
