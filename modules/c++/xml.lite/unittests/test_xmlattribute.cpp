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

#include <string>

#include "io/StringStream.h"
#include <TestCase.h>

#include "xml/lite/MinidomParser.h"
#include "xml/lite/QName.h"

static const std::string& strUri()
{
    static const std::string retval("urn:example.com");
    return retval;
}
static const xml::lite::Uri& uri()
{
    static const xml::lite::Uri retval(strUri());
    return retval;
}

static const auto& strXml()
{
    static const std::string strXml_1_ = R"(
<root>
    <doc name="doc">
        <a a="a">TEXT</a>
        <values int="314" double="3.14" string="abc" bool="yes" empty=""/>
        <ns_values xmlns:ns=")";
    static const std::string strXml_2_ = R"(" ns:int="314" />
    </doc>
</root>)";
    static const auto retval = strXml_1_ + strUri() + strXml_2_;
    return retval;
}

struct test_MinidomParser final
{
    xml::lite::MinidomParser xmlParser;
    xml::lite::Element* getRootElement()
    {
        io::StringStream ss;
        ss.stream() << strXml();

        xmlParser.parse(ss);
        return getDocument(xmlParser).getRootElement();
    }
};

TEST_CASE(test_getAttribute)
{
    using namespace xml::lite::literals;  // _q and _u for QName and Uri

    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    const auto& a = root->getElementByTagName("a", true /*recurse*/);
    const auto& attributes = a.getAttributes();

    std::string value;
    value = attributes.getValue("a");
    TEST_ASSERT_EQ("a", value);
    value = attributes["a"];
    TEST_ASSERT_EQ("a", value);
    value = attributes["a"_q];
    TEST_ASSERT_EQ("a", value);

    const auto result = attributes.getValue("a", value);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQ("a", value);
}

TEST_CASE(test_getAttributeByNS)
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    const auto& e = root->getElementByTagName("ns_values", true /*recurse*/);
    const auto& attributes = e.getAttributes();

    using namespace xml::lite;

    std::string strValue;
    strValue = attributes.getValue(xml::lite::QName(uri(), "int"));
    TEST_ASSERT_EQ("314", strValue);
    strValue = getValue<std::string>(attributes, uri(), "int");
    TEST_ASSERT_EQ("314", strValue);
    const auto key = xml::lite::QName(uri(), "int");
    strValue = getValue<std::string>(attributes, key);
    TEST_ASSERT_EQ("314", strValue);

    int value;
    auto result = getValue<int>(attributes, uri(), "int", value);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQ(314, value);
    value = getValue<int>(attributes, uri(), "int");
    TEST_ASSERT_EQ(314, value);

    result = getValue<int>(attributes, key, value);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQ(314, value);
    value = getValue<int>(attributes, key);
    TEST_ASSERT_EQ(314, value);
}

TEST_CASE(test_getAttributeByIndex)
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    const auto& a = root->getElementByTagName("a", true /*recurse*/);
    const auto& attributes = a.getAttributes();

    std::string value;
    value = attributes.getValue(0);
    TEST_ASSERT_EQ("a", value);

    const auto result = attributes.getValue(0, value);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQ("a", value);
}

TEST_CASE(test_getAttributeNotFound)
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    const auto& a = root->getElementByTagName("a", true /*recurse*/);
    const auto& attributes = a.getAttributes();

    std::string value;
    auto result = attributes.getValue("not_found", value);
    TEST_ASSERT_FALSE(result);
    result = attributes.getValue(999, value);
    TEST_ASSERT_FALSE(result);

    TEST_SPECIFIC_EXCEPTION(attributes.getValue("not_found"), except::NoSuchKeyException);        
    TEST_THROWS(attributes.getValue(999));
}

TEST_CASE(test_getAttributeValue)
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    const auto& values = root->getElementByTagName("values", true /*recurse*/);
    const auto& attributes = values.getAttributes();

    using namespace xml::lite;
    {
        int value;
        const auto result = getValue(attributes, "int", value);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ(314, value);

        value = getValue<int>(attributes, "int");
        TEST_ASSERT_EQ(314, value);
    }
    {
        double value;
        const auto result = getValue(attributes, "double", value);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ(3.14, value);

        value = getValue<double>(attributes, "double");
        TEST_ASSERT_EQ(3.14, value);
    }
    {
        std::string value;
        const auto result = getValue(attributes, "string", value);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ("abc", value);

        value = getValue<std::string>(attributes, "string");
        TEST_ASSERT_EQ("abc", value);
    }
    {
        std::string value;
        const auto result = getValue(attributes, "empty", value);
        TEST_ASSERT_FALSE(result);
    }
    {
        auto toType = [](const std::string& value) { return value == "yes"; };
        bool value = false;
        auto result = castValue(attributes, "bool", value, toType);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ(true, value);

        value = castValue(attributes, "bool", toType);
        TEST_ASSERT_EQ(true, value);

        std::string strValue;
        result = getValue(attributes, "bool", strValue);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ("yes", strValue);

        strValue = getValue<std::string>(attributes, "bool");
        TEST_ASSERT_EQ("yes", strValue);
    }
    {
        std::string value;
        const auto result = getValue(attributes, "not_found", value);
        TEST_ASSERT_FALSE(result);
    }
}

TEST_CASE(test_getAttributeValueFailure)
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    const auto& values = root->getElementByTagName("values", true /*recurse*/);
    const auto& attributes = values.getAttributes();

    {
        int value;
        const auto result = getValue(attributes, "string", value);
        TEST_ASSERT_FALSE(result);
    }
    {
        double value;
        const auto result = getValue(attributes, "string", value);
        TEST_ASSERT_FALSE(result);
    }
    {
        std::string value;
        const auto result = getValue(attributes, "empty", value);
        TEST_ASSERT_FALSE(result);
        value = attributes.getValue("empty");
        TEST_ASSERT_TRUE(value.empty());
    }
}

TEST_CASE(test_getAttributeValueThrows)
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    const auto& values = root->getElementByTagName("values", true /*recurse*/);
    const auto& attributes = values.getAttributes();

    using namespace xml::lite;
    TEST_SPECIFIC_EXCEPTION(getValue<int>(attributes, "string"), except::BadCastException);
    TEST_SPECIFIC_EXCEPTION(getValue<double>(attributes, "string"), except::BadCastException);
    TEST_SPECIFIC_EXCEPTION(getValue<std::string>(attributes, "empty"), except::BadCastException);

    TEST_SPECIFIC_EXCEPTION(getValue<std::string>(attributes, "not_found"), except::NoSuchKeyException);
}

TEST_CASE(test_getAttributeValueByIndex)
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    const auto& values = root->getElementByTagName("values", true /*recurse*/);
    const auto& attributes = values.getAttributes();

    using namespace xml::lite;
    {
        int value;
        const auto result = getValue(attributes, 0, value);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ(314, value);

        value = getValue<int>(attributes, 0);
        TEST_ASSERT_EQ(314, value);
    }
    {
        double value;
        const auto result = getValue(attributes, 1, value);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ(3.14, value);
        
        value = getValue<double>(attributes, 1);
        TEST_ASSERT_EQ(3.14, value);
    }
    {
        std::string value;
        const auto result = getValue(attributes, 2, value);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ("abc", value);

        value = getValue<std::string>(attributes, 2);
        TEST_ASSERT_EQ("abc", value);
    }
    {
        std::string value;
        auto result = getValue(attributes, -1, value);
        TEST_ASSERT_FALSE(result);
        TEST_SPECIFIC_EXCEPTION(getValue<std::string>(attributes, -1), except::NoSuchKeyException);

        result = getValue(attributes, 999, value);
        TEST_ASSERT_FALSE(result);
        TEST_SPECIFIC_EXCEPTION(getValue<std::string>(attributes, 999), except::NoSuchKeyException);
    }
}

TEST_CASE(test_setAttributeValue)
{
    test_MinidomParser xmlParser;
    auto root = xmlParser.getRootElement();

    auto& values = root->getElementByTagName("values", true /*recurse*/);
    auto& attributes = values.getAttributes();

    {
        auto result = setValue(attributes, "int", 123);
        TEST_ASSERT_TRUE(result);
        int value;
        result = getValue(attributes, "int", value);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ(123, value);
    }
    {
        auto result = setValue(attributes, "double", 1.23);
        TEST_ASSERT_TRUE(result);
        double value;
        result = getValue(attributes, "double", value);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ(1.23, value);
    }
    {
        auto result = setValue(attributes, "string", "xyz");
        TEST_ASSERT_TRUE(result);
        std::string value;
        result = getValue(attributes, "string", value);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ("xyz", value);

        attributes["string"] = "abc";
        value = attributes["string"];
        TEST_ASSERT_EQ("abc", value);

        using namespace xml::lite::literals;  // _q and _u for QName and Uri
        attributes["string"_q] = "123";
        value = attributes["string"_q];
        TEST_ASSERT_EQ("123", value);
    }
    {
        auto toString = [](const bool& value) { return value ? "yes" : "no"; };
        auto result = setValue(attributes, "bool", true, toString);
        TEST_ASSERT_TRUE(result);

        auto toType = [](const std::string& value) { return value == "yes"; };
        bool value = false;
        result = castValue(attributes, "bool", value, toType);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ(true, value);
        std::string strValue;
        result = getValue(attributes, "bool", strValue);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ("yes", strValue);
    }

    {
        const auto result = setValue(attributes, "not_found", 999);
        TEST_ASSERT_FALSE(result);
    }
}

TEST_CASE(test_setAttributeValueByIndex)
{
    test_MinidomParser xmlParser;
    auto root = xmlParser.getRootElement();

    auto& values = root->getElementByTagName("values", true /*recurse*/);
    auto& attributes = values.getAttributes();

    {
        auto result = setValue(attributes, 0, 123);
        TEST_ASSERT_TRUE(result);
        int value;
        result = getValue(attributes, 0, value);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ(123, value);
    }
    {
        auto result = setValue(attributes, 1, 1.23);
        TEST_ASSERT_TRUE(result);
        double value;
        result = getValue(attributes, 1, value);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ(1.23, value);
    }
    {
        auto result = setValue(attributes, 2, "xyz");
        TEST_ASSERT_TRUE(result);
        std::string value;
        result = getValue(attributes, 2, value);
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQ("xyz", value);
    }
    {
        auto result = setValue(attributes, -1, -1);
        TEST_ASSERT_FALSE(result);
        result = setValue(attributes, 999, 999);
        TEST_ASSERT_FALSE(result);
    }
}

int main(int, char**)
{
    TEST_CHECK(test_getAttribute);
    TEST_CHECK(test_getAttributeByIndex);
    TEST_CHECK(test_getAttributeByNS);
    TEST_CHECK(test_getAttributeNotFound);
    TEST_CHECK(test_getAttributeValue);
    TEST_CHECK(test_getAttributeValueFailure);
    TEST_CHECK(test_getAttributeValueThrows);
    TEST_CHECK(test_getAttributeValueByIndex);
    TEST_CHECK(test_setAttributeValue);
    TEST_CHECK(test_setAttributeValueByIndex);
}
