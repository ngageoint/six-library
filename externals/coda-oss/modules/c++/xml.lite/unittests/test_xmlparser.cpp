/* =========================================================================
 * This file is part of xml.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * xml.lite-c++ is free software; you can redistribute it and/or modify
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
#include <std/filesystem>
#include <std/optional>

#include "io/StringStream.h"
#include "io/FileInputStream.h"
#include "str/Convert.h"
#include "str/Encoding.h"
#include "str/EncodedString.h"
#include <TestCase.h>

#include "xml/lite/MinidomParser.h"
#include "xml/lite/Validator.h"

static const std::string text("TEXT");
static const std::string strXml = "<root><doc><a>" + text + "</a></doc></root>";

static const str::EncodedString iso88591Text(str::cast<str::W1252string::const_pointer>("T\xc9XT"));  // ISO8859-1, "T�XT"
static const auto iso88591Text1252 = str::EncodedStringView::details::w1252string(iso88591Text.view());
static const auto pIso88591Text_ = str::c_str<std::string>(iso88591Text1252);

static const str::EncodedString utf8Text(str::cast<coda_oss::u8string::const_pointer>("T\xc3\x89XT"));  // UTF-8,  "T�XT"
static const auto utf8Text8 = utf8Text.u8string();
static const auto pUtf8Text_ = str::c_str<std::string>(utf8Text8);

static const auto strUtf8Xml8 = str::fromUtf8("<root><doc><a>") + utf8Text8 + str::fromUtf8("</a></doc></root>");
static const std::string strUtf8Xml = str::c_str<std::string>(strUtf8Xml8);

constexpr auto PlatformEncoding = xml::lite::PlatformEncoding;
static const std::string  platfromText_ = PlatformEncoding == xml::lite::StringEncoding::Utf8 ? pUtf8Text_ : pIso88591Text_;

namespace fs = std::filesystem;

static fs::path findRoot(const fs::path& p)
{
    if (is_regular_file(p / "LICENSE")  && is_regular_file(p / "README.md")  && is_regular_file(p / "CMakeLists.txt"))
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
    TEST_ASSERT_EQ(actual, pIso88591Text_);
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

    coda_oss::u8string actual;
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

    std::optional<xml::lite::StringEncoding> encoding; // avoid compiler warning about possible uninitialized variable
    encoding = a.getEncoding();
    TEST_ASSERT_TRUE(encoding.has_value());
    TEST_ASSERT(*encoding == PlatformEncoding);

    // different getCharacterData() API
    encoding = a.getCharacterData(actual);
    TEST_ASSERT_EQ(actual, expected);
    TEST_ASSERT_TRUE(encoding.has_value());
    TEST_ASSERT(*encoding == PlatformEncoding);
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
    TEST_ASSERT_EQ(actual, pUtf8Text_);
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
    const auto actual = testXmlPrint_(expected, pIso88591Text_);
    TEST_ASSERT_EQ(actual, expected);
}

TEST_CASE(testXmlPrintUtf8)
{
    xml::lite::MinidomParser xmlParser;
    auto& document = getDocument(xmlParser);

    const auto pRootElement = document.createElement(xml::lite::QName(xml::lite::Uri(), "root"), pIso88591Text_, xml::lite::StringEncoding::Windows1252);

    io::StringStream output;
    pRootElement->print(output, xml::lite::StringEncoding::Utf8); // write UTF-8
    const auto actual = output.stream().str();
    const auto expected = std::string("<root>") + pUtf8Text_ + "</root>";
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
    const auto unittests = findRoot() / "modules" / "c++" / "xml.lite" / "unittests";

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
    const auto unittests =  findRoot() / "modules" / "c++" / "xml.lite" / "unittests";

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
    const auto unittests = findRoot() / "modules" / "c++" / "xml.lite" / "unittests";

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
    const std::string classificationText_iso8859_1("NON CLASSIFI\xc9 / UNCLASSIFIED");  // ISO8859-1 "NON CLASSIFI� / UNCLASSIFIED"
    const std::string classificationText_utf_8("NON CLASSIFI\xc3\x89 / UNCLASSIFIED");  // UTF-8 "NON CLASSIFI� / UNCLASSIFIED"
    const auto classificationText_platform = sys::Platform == sys::PlatformType::Linux ? classificationText_utf_8 : classificationText_iso8859_1;
    const auto characterData = classificationXML.getCharacterData();
    TEST_ASSERT_EQ(characterData, classificationText_platform);

    const str::EncodedStringView expectedCharDataView(str::c_str<std::u8string>(classificationText_utf_8));
    std::u8string u8_characterData;
    classificationXML.getCharacterData(u8_characterData);
    TEST_ASSERT_EQ(u8_characterData, expectedCharDataView);
    std::string u8_characterData_;
    str::EncodedStringView(u8_characterData).toUtf8(u8_characterData_);
    TEST_ASSERT_EQ(classificationText_utf_8, u8_characterData_);
}

static void testValidateXmlFile_(const std::string& testName, const std::string& xmlFile, const xml::lite::StringEncoding* pEncoding)
{
    const auto unittests = findRoot() / "modules" / "c++" / "xml.lite" / "unittests";

    const auto xsd = unittests / "doc.xsd";
    if (!exists(xsd))  // running in "externals" of a different project
    {
        std::clog << "Path does not exist: '" << xsd << "'\n";
        return;
    }
    const auto path = unittests / xmlFile;

    const std::vector<std::filesystem::path> schemaPaths{xsd.parent_path()}; // fs::path -> new string-conversion code
    const xml::lite::Validator validator(schemaPaths, nullptr /*log*/);

    io::FileInputStream fis(path);
    std::vector<xml::lite::ValidationInfo> errors;
    const auto result = (pEncoding == nullptr) ? validator.validate(fis, path.string() /*xmlID*/, errors) :
        validator.validate(fis, *pEncoding, path.string() /*xmlID*/, errors);
    for (const auto& error : errors)
    {
        std::clog << error.toString() << "\n";
    }
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_TRUE(errors.empty());
}
static void testValidateXmlFile(const std::string& testName, const std::string& xmlFile)
{
    testValidateXmlFile_(testName, xmlFile, nullptr /*pEncoding*/);
}
static void testValidateXmlFile(const std::string& testName, const std::string& xmlFile, xml::lite::StringEncoding encoding)
{
    testValidateXmlFile_(testName, xmlFile, &encoding);
}
TEST_CASE(testValidateXmlFile)
{
    testValidateXmlFile(testName, "ascii.xml");
    testValidateXmlFile(testName, "ascii_encoding_utf-8.xml");

    // legacy validate() API, new string conversion
    testValidateXmlFile(testName, "utf-8.xml");
    testValidateXmlFile(testName, "encoding_utf-8.xml");

    // new validate() API
    testValidateXmlFile(testName, "utf-8.xml", xml::lite::StringEncoding::Utf8);
    testValidateXmlFile(testName, "encoding_utf-8.xml", xml::lite::StringEncoding::Utf8);
    testValidateXmlFile(testName, "windows-1252.xml", xml::lite::StringEncoding::Windows1252);
    testValidateXmlFile(testName, "encoding_windows-1252.xml", xml::lite::StringEncoding::Windows1252);
}

static void testValidateXmlFileLegacy(const std::string& testName, const std::string& xmlFile, bool success=true)
{
    const auto unittests = findRoot() / "modules" / "c++" / "xml.lite" / "unittests";

    const auto xsd = unittests / "doc.xsd";
    if (!exists(xsd))  // running in "externals" of a different project
    {
        std::clog << "Path does not exist: '" << xsd << "'\n";
        return;
    }
    const auto path = unittests / xmlFile;

    const std::vector<std::string> schemaPaths{xsd.parent_path().string()}; // std::string -> legacy behavior
    const xml::lite::Validator validator(schemaPaths, nullptr /*log*/);

    io::FileInputStream fis(path);
    std::vector<xml::lite::ValidationInfo> errors;
    const auto result = validator.validate(fis, path.string() /*xmlID*/, errors);
    for (const auto& error : errors)
    {
        std::clog << error.toString() << "\n";
    }
    if (success)
    {
        TEST_ASSERT_FALSE(result);
        TEST_ASSERT_TRUE(errors.empty());
    }
    else
    {
        TEST_ASSERT_TRUE(result); // errors
        TEST_ASSERT_FALSE(errors.empty());
    }
}
TEST_CASE(testValidateXmlFileLegacy)
{
    // these two work on all platforms
    testValidateXmlFile(testName, "ascii.xml");
    testValidateXmlFile(testName, "ascii_encoding_utf-8.xml");

    // These are OK on Windows but fail on Linux; this is as-expected with "legacy" string conversion.
    constexpr auto success = sys::Platform == sys::PlatformType::Windows ? true : false;
    testValidateXmlFileLegacy(testName, "utf-8.xml", success);
    testValidateXmlFileLegacy(testName, "encoding_utf-8.xml", success);
    testValidateXmlFileLegacy(testName, "windows-1252.xml", success);
    testValidateXmlFileLegacy(testName, "encoding_windows-1252.xml", success);
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

    TEST_CHECK(testValidateXmlFile);
    TEST_CHECK(testValidateXmlFileLegacy);
}
