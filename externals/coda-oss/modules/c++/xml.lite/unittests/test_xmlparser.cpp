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
#include <clocale>

#include "io/StringStream.h"
#include "str/Convert.h"
#include <TestCase.h>

#include "xml/lite/MinidomParser.h"

static const std::string text("TEXT");
static const std::string strXml = "<root><doc><a>" + text + "</a></doc></root>";
static const std::string iso88591Text("T\xc9XT");  // ISO8859-1, "TÉXT"
static const std::string utf8Text("T\xc3\x89XT");  // UTF-8,  "TÉXT"
static const auto strUtf8Xml = "<root><doc><a>" + utf8Text + "</a></doc></root>";

TEST_CASE(testXmlParseSimple)
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
    
    {
        const auto aElements = root->getElementsByTagName("a", true /*recurse*/);
        TEST_ASSERT_EQ(aElements.size(), 1);
        const auto& a = *(aElements[0]);

        const auto characterData = a.getCharacterData();
        TEST_ASSERT_EQ(characterData, text);
        const auto pEncoding = a.getEncoding();
        TEST_ASSERT_NULL(pEncoding);
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

TEST_CASE(testXmlPreserveCharacterData)
{
    io::StringStream stream;
    stream.stream() << strUtf8Xml;
    TEST_ASSERT_EQ(stream.stream().str(), strUtf8Xml);

    xml::lite::MinidomParser xmlParser;
    // This is needed in Windows, because the default locale is *.1252 (more-or-less ISO8859-1)
    // Unfortunately, there doesn't seem to be a way of testing this ...
    // calling parse() w/o preserveCharacterData() throws ASSERTs, even after calling
    // _set_error_mode(_OUT_TO_STDERR) so there's no way to use TEST_EXCEPTION
    xmlParser.preserveCharacterData(true);
    xmlParser.parse(stream);
    TEST_ASSERT_TRUE(true);
}

static xml::lite::Element& testXmlUtf8_(xml::lite::MinidomParser& xmlParser)
{
    io::StringStream stream;
    stream.stream() << strUtf8Xml;

    xmlParser.preserveCharacterData(true);
    xmlParser.parse(stream);

    auto aElements =
        xmlParser.getDocument()->getRootElement()->getElementsByTagName("a", true /*recurse*/);
    auto& a = *(aElements[0]);
    return a;
}

TEST_CASE(testXmlUtf8Legacy)
{
    xml::lite::MinidomParser xmlParser;
    const auto& a = testXmlUtf8_(xmlParser);

    // This is LEGACY behavior, it is INCORRECT on Linux!
    const auto actual = a.getCharacterData();
    #ifdef _WIN32
    TEST_ASSERT_EQ(actual, iso88591Text);
    #else
    TEST_ASSERT_EQ(actual.length(), 4);
    #endif

    const auto pEncoding = a.getEncoding();
    TEST_ASSERT_NULL(pEncoding);
}

TEST_CASE(testXmlUtf8_u8string)
{
    xml::lite::MinidomParser xmlParser(true /*storeEncoding*/);
    const auto& a = testXmlUtf8_(xmlParser);

    sys::U8string actual_;
    a.getCharacterData(actual_);
    const std::string actual = str::toString(actual_);
    TEST_ASSERT_EQ(actual, utf8Text);
}

TEST_CASE(testXmlUtf8)
{
    xml::lite::MinidomParser xmlParser(true /*storeEncoding*/);
    const auto& a = testXmlUtf8_(xmlParser);

    {
        const auto actual = a.getCharacterData();
        const auto pEncoding = a.getEncoding();
        TEST_ASSERT(pEncoding != nullptr);
        #ifdef _WIN32
        TEST_ASSERT_EQ(actual, iso88591Text);
        TEST_ASSERT(*pEncoding == xml::lite::string_encoding::windows_1252);
        #else
        TEST_ASSERT_EQ(actual, utf8Text);
        TEST_ASSERT(*pEncoding == xml::lite::string_encoding::utf_8);
        #endif
    }
    {
        std::string actual;
        const auto pEncoding = a.getCharacterData(actual);
        TEST_ASSERT(pEncoding != nullptr);
        #ifdef _WIN32
        TEST_ASSERT_EQ(actual, iso88591Text);
        TEST_ASSERT(*pEncoding == xml::lite::string_encoding::windows_1252);
        #else
        TEST_ASSERT_EQ(actual, utf8Text);
        TEST_ASSERT(*pEncoding == xml::lite::string_encoding::utf_8);
        #endif
    }
}

TEST_CASE(testXml_setCharacterData)
{
    xml::lite::MinidomParser xmlParser;
    auto& a = testXmlUtf8_(xmlParser);

    const sys::U8string characters = str::castToU8string(utf8Text);
    a.setCharacterData(characters);
    auto pEncoding = a.getEncoding();
    TEST_ASSERT(pEncoding != nullptr);
    TEST_ASSERT(*pEncoding == xml::lite::string_encoding::utf_8);

    std::string actual;
    pEncoding = a.getCharacterData(actual);
    TEST_ASSERT(pEncoding != nullptr);
    TEST_ASSERT(*pEncoding == xml::lite::string_encoding::utf_8);
    TEST_ASSERT_EQ(actual, utf8Text);
}

static std::string testXmlPrint_(std::string& expected, const std::string& characterData)
{
    xml::lite::MinidomParser xmlParser;
    auto pDocument = xmlParser.getDocument();

    const auto pRootElement = pDocument->createElement("root", "" /*uri*/, characterData);

    io::StringStream output;
    pRootElement->print(output);
    expected = "<root>" + characterData + "</root>";
    return output.stream().str();
}
TEST_CASE(testXmlPrintSimple)
{
    std::string expected;
    const auto actual = testXmlPrint_(expected, text);
    TEST_ASSERT_EQ(actual, expected);
}
TEST_CASE(testXmlPrintLegacy)
{
    // This is LEGACY behavior, it generates bad XML
    std::string expected;
    const auto actual = testXmlPrint_(expected, iso88591Text);
    TEST_ASSERT_EQ(actual, expected);
}

TEST_CASE(testXmlPrintUtf8)
{
    xml::lite::MinidomParser xmlParser;
    auto pDocument = xmlParser.getDocument();

    const auto encoding = xml::lite::string_encoding::windows_1252;
    const auto pRootElement = pDocument->createElement("root", "" /*uri*/, iso88591Text, &encoding);

    io::StringStream output;
    pRootElement->print(output, xml::lite::string_encoding::utf_8); // write UTF-8
    const auto actual = output.stream().str();
    const auto expected = "<root>" + utf8Text + "</root>";
    TEST_ASSERT_EQ(actual, expected);
}

TEST_CASE(testXmlParseAndPrintUtf8)
{
    io::StringStream input;
    input.stream() << strUtf8Xml;

    xml::lite::MinidomParser xmlParser(true /*storeEncoding*/);
    xmlParser.preserveCharacterData(true);
    xmlParser.parse(input);
    const auto pRootElement = xmlParser.getDocument()->getRootElement();

    io::StringStream output;
    pRootElement->print(output, xml::lite::string_encoding::utf_8); // write UTF-8
    const auto actual = output.stream().str();
    TEST_ASSERT_EQ(actual, strUtf8Xml);
}

int main(int, char**)
{
    TEST_CHECK(testXmlParseSimple);
    TEST_CHECK(testXmlPreserveCharacterData);
    TEST_CHECK(testXmlUtf8Legacy);
    TEST_CHECK(testXmlUtf8);
    TEST_CHECK(testXmlUtf8_u8string);    
    TEST_CHECK(testXml_setCharacterData);    

    TEST_CHECK(testXmlPrintSimple);
    TEST_CHECK(testXmlPrintLegacy);
    TEST_CHECK(testXmlParseAndPrintUtf8);
    TEST_CHECK(testXmlPrintUtf8);
}
