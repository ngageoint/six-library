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
#include "sys/OS.h"
#include "str/EncodedStringView.h"
#include <TestCase.h>

#include "xml/lite/MinidomParser.h"

static const std::string text("TEXT");
static const std::string strXml = "<root><doc><a>" + text + "</a></doc></root>";
static const std::string iso88591Text_("T\xc9XT");  // ISO8859-1, "TÉXT"
static const std::string utf8Text_("T\xc3\x89XT");  // UTF-8,  "TÉXT"
static const auto utf8Text8 = str::EncodedStringView::create<sys::U8string>(utf8Text_).to_u8string();
static const auto strUtf8Xml = "<root><doc><a>" + utf8Text_ + "</a></doc></root>";
constexpr auto PlatformEncoding = sys::Platform == sys::PlatformType::Windows
        ? xml::lite::StringEncoding::Windows1252
        : xml::lite::StringEncoding::Utf8;
static const auto platfromText_ = sys::Platform == sys::PlatformType::Linux ? utf8Text_ : iso88591Text_;

namespace fs = std::filesystem;

static fs::path findRoot(const fs::path& p)
{
    if (fs::is_regular_file(p / "LICENSE")  && fs::is_regular_file(p / "README.md")  && fs::is_regular_file(p / "CMakeLists.txt"))
    {
        return p;
    }
    return findRoot(p.parent_path());
}
inline fs::path findRoot()
{
    return findRoot(fs::current_path());
}

static void test_a_element(const std::string& testName, const xml::lite::Element& root)
{
    const auto aElements = root.getElementsByTagName("a", true /*recurse*/);
    TEST_ASSERT_EQ(aElements.size(), static_cast<size_t>(1));
    const auto& a = *(aElements[0]);

    const auto characterData = a.getCharacterData();
    TEST_ASSERT_EQ(characterData, text);
}
TEST_CASE(testXmlParseSimple)
{
    io::StringStream ss;
    ss.stream() << strXml;
    TEST_ASSERT_EQ(ss.stream().str(), strXml);

    xml::lite::MinidomParser xmlParser;
    xmlParser.parse(ss);
    const auto& root = getRootElement(getDocument(xmlParser));
    test_a_element(testName, root);
        
    const auto docElements = root.getElementsByTagName("doc");
    TEST_ASSERT_FALSE(docElements.empty());
    TEST_ASSERT_EQ(docElements.size(), static_cast<size_t>(1));
    test_a_element(testName, *docElements[0]);
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

    auto aElements = getRootElement(xmlParser.getDocument()).getElementsByTagName("a", true /*recurse*/);
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
    TEST_ASSERT_EQ(actual, iso88591Text_);
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

    sys::U8string actual;
    a.getCharacterData(actual);
    TEST_ASSERT_EQ(actual, utf8Text8);
}

TEST_CASE(testXmlUtf8)
{
    xml::lite::MinidomParser xmlParser(true /*storeEncoding*/);
    const auto& a = testXmlUtf8_(xmlParser);

    auto actual = a.getCharacterData();
    const auto expected = platfromText_;
    TEST_ASSERT_EQ(actual, expected);

    auto encoding = a.getEncoding();
    TEST_ASSERT_TRUE(encoding.has_value());
    TEST_ASSERT(encoding == PlatformEncoding);

    // different getCharacterData() API
    encoding = a.getCharacterData(actual);
    TEST_ASSERT_EQ(actual, expected);
    TEST_ASSERT(encoding == PlatformEncoding);
}

TEST_CASE(testXml_setCharacterData)
{
    xml::lite::MinidomParser xmlParser;
    auto& a = testXmlUtf8_(xmlParser);

    a.setCharacterData(utf8Text8);
    auto encoding = a.getEncoding();
    TEST_ASSERT_TRUE(encoding.has_value());
    TEST_ASSERT(encoding == xml::lite::StringEncoding::Utf8);

    std::string actual;
    encoding = a.getCharacterData(actual);
    TEST_ASSERT_TRUE(encoding.has_value());
    TEST_ASSERT(encoding == xml::lite::StringEncoding::Utf8);
    TEST_ASSERT_EQ(actual, utf8Text_);
}

static std::string testXmlPrint_(std::string& expected, const std::string& characterData)
{
    xml::lite::MinidomParser xmlParser;
    auto& document = getDocument(xmlParser);

    const auto pRootElement = document.createElement(xml::lite::QName(xml::lite::Uri(), "root"), characterData);

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
    const auto actual = testXmlPrint_(expected, iso88591Text_);
    TEST_ASSERT_EQ(actual, expected);
}

TEST_CASE(testXmlPrintUtf8)
{
    xml::lite::MinidomParser xmlParser;
    auto& document = getDocument(xmlParser);

    const auto pRootElement = document.createElement(xml::lite::QName(xml::lite::Uri(), "root"), iso88591Text_, xml::lite::StringEncoding::Windows1252);

    io::StringStream output;
    pRootElement->print(output, xml::lite::StringEncoding::Utf8); // write UTF-8
    const auto actual = output.stream().str();
    const auto expected = "<root>" + utf8Text_ + "</root>";
    TEST_ASSERT_EQ(actual, expected);
}

TEST_CASE(testXmlParseAndPrintUtf8)
{
    io::StringStream input;
    input.stream() << strUtf8Xml;

    xml::lite::MinidomParser xmlParser(true /*storeEncoding*/);
    xmlParser.preserveCharacterData(true);
    xmlParser.parse(input);
    const auto pRootElement = getDocument(xmlParser).getRootElement();

    io::StringStream output;
    pRootElement->print(output, xml::lite::StringEncoding::Utf8); // write UTF-8
    const auto actual = output.stream().str();
    TEST_ASSERT_EQ(actual, strUtf8Xml);
}

static void testReadEncodedXmlFile(const std::string& testName, const std::string& xmlFile, bool preserveCharacterData)
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
    xmlParser.preserveCharacterData(preserveCharacterData);
    xmlParser.parse(input);
    const auto& root = getRootElement(getDocument(xmlParser));

    const auto& a = root.getElementByTagName("a", true /*recurse*/);
    auto characterData = a.getCharacterData();
    const auto encoding = a.getEncoding();
    TEST_ASSERT(encoding == PlatformEncoding);
    TEST_ASSERT_EQ(characterData, platfromText_);

    std::u8string u8_characterData;
    a.getCharacterData(u8_characterData);
    TEST_ASSERT_EQ(utf8Text8, u8_characterData);     

    const auto& textXML = root.getElementByTagName("text", true /*recurse*/);
    characterData = textXML.getCharacterData();
    const auto expectedText = preserveCharacterData ? "\tt\te\tx\tt\t" : "t\te\tx\tt";
    TEST_ASSERT_EQ(characterData, expectedText);

    const auto& whitespaceXML = root.getElementByTagName("whitespace", true /*recurse*/);
    characterData = whitespaceXML.getCharacterData();
    const auto expectedWhitespace = preserveCharacterData ? "             " : "";
    TEST_ASSERT_EQ(characterData, expectedWhitespace);

    const auto& emptyXML = root.getElementByTagName("empty", true /*recurse*/);
    characterData = emptyXML.getCharacterData();
    TEST_ASSERT_EQ(characterData, "");
}
TEST_CASE(testReadEncodedXmlFiles)
{
    // these have "<?xml version="1.0" encoding="..." ?>"
    testReadEncodedXmlFile(testName, "encoding_utf-8.xml", true /*preserveCharacterData*/);
    testReadEncodedXmlFile(testName, "encoding_utf-8.xml", false /*preserveCharacterData*/);
    testReadEncodedXmlFile(testName, "encoding_windows-1252.xml", true /*preserveCharacterData*/);
    testReadEncodedXmlFile(testName, "encoding_windows-1252.xml", false /*preserveCharacterData*/);
}

static void testReadXmlFile(const std::string& testName, const std::string& xmlFile, bool preserveCharacterData)
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
    xmlParser.preserveCharacterData(preserveCharacterData);
    xmlParser.parse(input);
    const auto& root = getRootElement(getDocument(xmlParser));

    const auto aElements = root.getElementsByTagName("a", true /*recurse*/);
    TEST_ASSERT_EQ(aElements.size(), static_cast<size_t>(1));
    const auto& a = *(aElements[0]);

    auto characterData = a.getCharacterData();
    const auto encoding = a.getEncoding();
    TEST_ASSERT(encoding == PlatformEncoding);
    TEST_ASSERT_EQ(characterData, platfromText_);

    std::u8string u8_characterData;
    a.getCharacterData(u8_characterData);
    TEST_ASSERT_EQ(utf8Text8, u8_characterData);

    const auto& textXML = root.getElementByTagName("text", true /*recurse*/);
    characterData = textXML.getCharacterData();
    const auto expectedText = preserveCharacterData ? "\tt\te\tx\tt\t" : "t\te\tx\tt";
    TEST_ASSERT_EQ(characterData, expectedText);

    const auto& whitespaceXML = root.getElementByTagName("whitespace", true /*recurse*/);
    characterData = whitespaceXML.getCharacterData();
    const auto expectedWhitespace = preserveCharacterData ? "             " : "";
    TEST_ASSERT_EQ(characterData, expectedWhitespace);

    const auto& emptyXML = root.getElementByTagName("empty", true /*recurse*/);
    characterData = emptyXML.getCharacterData();
    TEST_ASSERT_EQ(characterData, "");
}
TEST_CASE(testReadXmlFiles)
{
    // These do NOT have "<?xml version="1.0" encoding="..." ?>"
    testReadXmlFile(testName, "utf-8.xml", true /*preserveCharacterData*/);
    testReadXmlFile(testName, "utf-8.xml", false /*preserveCharacterData*/);
    testReadXmlFile(testName, "windows-1252.xml", true /*preserveCharacterData*/);
    testReadXmlFile(testName, "windows-1252.xml", false /*preserveCharacterData*/);
}

static bool find_string(io::FileInputStream& stream, const std::string& s)
{
    const auto pos = stream.tell();

    constexpr sys::Off_T offset = 0x0000558e;
    std::string streamAsString;
    {
        stream.seek(offset, io::Seekable::START);
        io::StringStream stringStream;
        stream.streamTo(stringStream);
        streamAsString = stringStream.stream().str();
    }
    const auto result = streamAsString.find(s);
    if ((result != std::string::npos) && (result == 0))
    {
        stream.seek(offset, io::Seekable::START);
        return true;
    }

    stream.seek(pos, io::Seekable::START);
    return false;
}

TEST_CASE(testReadEmbeddedXml)
{
    // This is a binary file with XML burried in it somewhere
    const auto coda_oss = findRoot();
    const auto unittests = coda_oss / "modules" / "c++" / "xml.lite" / "unittests";

    const auto path = unittests / "embedded_xml.bin";
    if (!exists(path))  // running in "externals" of a different project
    {
        std::clog << "Path does not exist: '" << path << "'\n";
        return;
    }
    io::FileInputStream input(path.string());
    const auto result = find_string(input, "<SICD ");
    TEST_ASSERT_TRUE(result);
    
    xml::lite::MinidomParser xmlParser(true /*storeEncoding*/);
    xmlParser.parse(input);
    const auto& root = getRootElement(getDocument(xmlParser));
    const auto& classificationXML = root.getElementByTagName("Classification", true /*recurse*/);

     const auto encoding = classificationXML.getEncoding();
    TEST_ASSERT(encoding == PlatformEncoding);

     // UTF-8 characters in 50x50.nitf
    const std::string classificationText_iso8859_1("NON CLASSIFI\xc9 / UNCLASSIFIED");  // ISO8859-1 "NON CLASSIFIÉ / UNCLASSIFIED"
    const std::string classificationText_utf_8("NON CLASSIFI\xc3\x89 / UNCLASSIFIED");  // UTF-8 "NON CLASSIFIÉ / UNCLASSIFIED"
    const auto classificationText_platform = sys::Platform == sys::PlatformType::Linux ? classificationText_utf_8 : classificationText_iso8859_1;
    const auto characterData = classificationXML.getCharacterData();
    TEST_ASSERT_EQ(characterData, classificationText_platform);

    const auto expectedCharDataView = str::EncodedStringView::create<std::u8string>(classificationText_utf_8);
    std::u8string u8_characterData;
    classificationXML.getCharacterData(u8_characterData);
    TEST_ASSERT_EQ(u8_characterData, expectedCharDataView);
    std::string u8_characterData_;
    str::EncodedStringView(u8_characterData).toUtf8(u8_characterData_);
    TEST_ASSERT_EQ(classificationText_utf_8, u8_characterData_);
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
    TEST_CHECK(testReadEmbeddedXml);
}
