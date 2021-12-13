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

#include <std/string>
#include <clocale>
#include <std/filesystem>

#include "io/StringStream.h"
#include "io/FileInputStream.h"
#include "str/Convert.h"
#include "str/Encoding.h"
#include <TestCase.h>

#include "xml/lite/MinidomParser.h"

static const std::string text("TEXT");
static const std::string strXml = "<root><doc><a>" + text + "</a></doc></root>";
static const std::string iso88591Text("T\xc9XT");  // ISO8859-1, "TÉXT"
static const std::string utf8Text("T\xc3\x89XT");  // UTF-8,  "TÉXT"
static const auto strUtf8Xml = "<root><doc><a>" + utf8Text + "</a></doc></root>";

namespace fs = std::filesystem;

static fs::path findRoot(const fs::path& p)
{
    const fs::path LICENSE("LICENSE");
    const fs::path README_md("README.md");
    const fs::path CMakeLists_txt("CMakeLists.txt");
    if (fs::is_regular_file(p / LICENSE)  && fs::is_regular_file(p / README_md)  && fs::is_regular_file(p / CMakeLists_txt))
    {
        return p;
    }
    return findRoot(p.parent_path());
}
inline fs::path findRoot()
{
    return findRoot(fs::current_path());
}

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
        TEST_ASSERT_EQ(aElements.size(), static_cast<size_t>(1));
        const auto& a = *(aElements[0]);

        const auto characterData = a.getCharacterData();
        TEST_ASSERT_EQ(characterData, text);
        const auto encoding = a.getEncoding();
        TEST_ASSERT_FALSE(encoding.has_value());
    }
    
    const auto docElements = root->getElementsByTagName("doc");
    TEST_ASSERT_FALSE(docElements.empty());
    TEST_ASSERT_EQ(docElements.size(), static_cast<size_t>(1));
    {
        const auto aElements = docElements[0]->getElementsByTagName("a");
        TEST_ASSERT_EQ(aElements.size(), static_cast<size_t>(1));
        const auto& a = *(aElements[0]);

        const auto characterData = a.getCharacterData();
        TEST_ASSERT_EQ(characterData, text);
        const auto encoding = a.getEncoding();
        TEST_ASSERT_FALSE(encoding.has_value());
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
    TEST_ASSERT_EQ(actual.length(), static_cast<size_t>(4));
    #endif

    const auto encoding = a.getEncoding();
    TEST_ASSERT_FALSE(encoding.has_value());
}

TEST_CASE(testXmlUtf8_u8string)
{
    xml::lite::MinidomParser xmlParser(true /*storeEncoding*/);
    const auto& a = testXmlUtf8_(xmlParser);

    sys::U8string actual_;
    a.getCharacterData(actual_);
    const std::string actual = str::c_str<std::string::const_pointer>(actual_);
    TEST_ASSERT_EQ(actual, utf8Text);
}

TEST_CASE(testXmlUtf8)
{
    xml::lite::MinidomParser xmlParser(true /*storeEncoding*/);
    const auto& a = testXmlUtf8_(xmlParser);

    {
        const auto actual = a.getCharacterData();
        const auto encoding = a.getEncoding();
        TEST_ASSERT_TRUE(encoding.has_value());
        #ifdef _WIN32
        TEST_ASSERT_EQ(actual, iso88591Text);
        TEST_ASSERT(encoding.value() == xml::lite::StringEncoding::Windows1252);
        #else
        TEST_ASSERT_EQ(actual, utf8Text);
        TEST_ASSERT(encoding.value() == xml::lite::StringEncoding::Utf8);
        #endif
    }
    {
        std::string actual;
        const auto encoding = a.getCharacterData(actual);
        TEST_ASSERT_TRUE(encoding.has_value());
        #ifdef _WIN32
        TEST_ASSERT_EQ(actual, iso88591Text);
        TEST_ASSERT(encoding.value() == xml::lite::StringEncoding::Windows1252);
        #else
        TEST_ASSERT_EQ(actual, utf8Text);
        TEST_ASSERT(encoding.value() == xml::lite::StringEncoding::Utf8);
        #endif
    }
}

TEST_CASE(testXml_setCharacterData)
{
    xml::lite::MinidomParser xmlParser;
    auto& a = testXmlUtf8_(xmlParser);

    const sys::U8string characters = str::c_str<sys::U8string::const_pointer>(utf8Text); // copy
    a.setCharacterData(characters);
    auto encoding = a.getEncoding();
    TEST_ASSERT_TRUE(encoding.has_value());
    TEST_ASSERT(encoding == xml::lite::StringEncoding::Utf8);

    std::string actual;
    encoding = a.getCharacterData(actual);
    TEST_ASSERT_TRUE(encoding.has_value());
    TEST_ASSERT(encoding == xml::lite::StringEncoding::Utf8);
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

    const auto pRootElement = pDocument->createElement("root", "" /*uri*/, iso88591Text, xml::lite::StringEncoding::Windows1252);

    io::StringStream output;
    pRootElement->print(output, xml::lite::StringEncoding::Utf8); // write UTF-8
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
    pRootElement->print(output, xml::lite::StringEncoding::Utf8); // write UTF-8
    const auto actual = output.stream().str();
    TEST_ASSERT_EQ(actual, strUtf8Xml);
}

static void testReadEncodedXmlFile(const std::string& testName, const std::string& xmlFile)
{
    const auto coda_oss = findRoot();
    const auto unittests = coda_oss / "modules" / "c++" / "xml.lite" / "unittests";

    const auto path = unittests / xmlFile;
    if (!exists(path))  // running in "externals" of a different project
    {
        std::clog << "Path does not exist: '" << path << "'\n";
        return;
    }
    io::FileInputStream input(path.string());

    xml::lite::MinidomParser xmlParser(true /*storeEncoding*/);
    xmlParser.preserveCharacterData(true);
    xmlParser.parse(input);
    const auto& root = getRootElement(*xmlParser.getDocument());

    const auto aElements = root.getElementsByTagName("a", true /*recurse*/);
    TEST_ASSERT_EQ(aElements.size(), static_cast<size_t>(1));
    const auto& a = *(aElements[0]);

    const auto characterData = a.getCharacterData();
    TEST_ASSERT_EQ(characterData, sys::Platform == sys::PlatformType::Linux ? utf8Text : iso88591Text);
    const auto encoding = a.getEncoding();
    TEST_ASSERT_TRUE(encoding.has_value());
    const auto expected_encoding = sys::Platform == sys::PlatformType::Linux ? xml::lite::StringEncoding::Utf8 : xml::lite::StringEncoding::Windows1252;
    TEST_ASSERT(encoding.value() == expected_encoding);

    std::u8string u8_characterData;
    a.getCharacterData(u8_characterData);
    const std::string u8_characterData_(str::c_str<std::string::const_pointer>(u8_characterData));
    TEST_ASSERT_EQ(utf8Text, u8_characterData_);     

}
TEST_CASE(testReadEncodedXmlFiles)
{
    // these have "<?xml version="1.0" encoding="..." ?>"
    testReadEncodedXmlFile(testName, "encoding_utf-8.xml");
    testReadEncodedXmlFile(testName, "encoding_windows-1252.xml");
}

static void testReadXmlFile(const std::string& testName, const std::string& xmlFile)
{
    const auto coda_oss = findRoot();
    const auto unittests = coda_oss / "modules" / "c++" / "xml.lite" / "unittests";

    const auto path = unittests / xmlFile;
    if (!exists(path))  // running in "externals" of a different project
    {
        std::clog << "Path does not exist: '" << path << "'\n";
        return;
    }
    io::FileInputStream input(path.string());

    xml::lite::MinidomParser xmlParser(true /*storeEncoding*/);
    xmlParser.preserveCharacterData(true);
    xmlParser.parse(input);
    const auto& root = getRootElement(*xmlParser.getDocument());

    const auto aElements = root.getElementsByTagName("a", true /*recurse*/);
    TEST_ASSERT_EQ(aElements.size(), static_cast<size_t>(1));
    const auto& a = *(aElements[0]);

    const auto characterData = a.getCharacterData();
    TEST_ASSERT_EQ(characterData, sys::Platform == sys::PlatformType::Linux ? utf8Text : iso88591Text);
    const auto encoding = a.getEncoding();
    TEST_ASSERT_TRUE(encoding.has_value());
    const auto expected_encoding = sys::Platform == sys::PlatformType::Linux ? xml::lite::StringEncoding::Utf8 : xml::lite::StringEncoding::Windows1252;
    TEST_ASSERT(encoding.value() == expected_encoding);

    std::u8string u8_characterData;
    a.getCharacterData(u8_characterData);
    const std::string u8_characterData_(str::c_str<std::string::const_pointer>(u8_characterData));
    TEST_ASSERT_EQ(utf8Text, u8_characterData_);
}
TEST_CASE(testReadXmlFiles)
{
    // These do NOT have "<?xml version="1.0" encoding="..." ?>"
    testReadXmlFile(testName, "utf-8.xml");
    testReadXmlFile(testName, "windows-1252.xml");
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
    
    TEST_CHECK(testReadEncodedXmlFiles);
    TEST_CHECK(testReadXmlFiles);    
}
