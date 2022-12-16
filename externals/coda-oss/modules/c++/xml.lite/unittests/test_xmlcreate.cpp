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

#include <tuple>
#include <std/string>
#include <std/filesystem>
#include <std/optional>

#include "io/StringStream.h"
#include <TestCase.h>

#include "xml/lite/MinidomParser.h"
#include "xml/lite/Element.h"
#include "xml/lite/QName.h"

TEST_CASE(testXmlCreateRoot)
{
    using namespace xml::lite::literals;  // _q and _u for QName and Uri

    xml::lite::MinidomParser xmlParser;
    auto& document = getDocument(xmlParser);

    auto documents_ = document.createElement(xml::lite::QName(""_u, "abc"), "abc");
    auto& documents = *documents_;
    io::StringStream output;
    documents.print(output);
    auto actual = output.stream().str();
    TEST_ASSERT_EQ("<abc>abc</abc>", actual);

    documents = "test"; // setCharacterData()
    documents = xml::lite::QName(""_u, "documents"); // setChild()

    output.reset();
    documents.print(output);
    actual = output.stream().str();
    TEST_ASSERT_EQ("<documents>test</documents>", actual);
}

TEST_CASE(testXmlCreateNested)
{  
    using namespace xml::lite::literals; // _q and _u for QName and Uri

    xml::lite::MinidomParser xmlParser;
    auto& document = getDocument(xmlParser);

    auto documents_ = document.createElement(xml::lite::QName(""_u, "documents"), "");
    auto& documents = *documents_;
    std::ignore = addChild(documents, "html");
    io::StringStream output;
    documents.print(output);
    auto actual = output.stream().str();
    const auto expected0 = "<documents><html/></documents>";
    TEST_ASSERT_EQ(expected0, actual);

    documents += xml::lite::AttributeNode("count"_q, "1");  // addAttribute()
    auto& html = setChild(documents, xml::lite::Element::create("html"));
    std::ignore =  addChild(html, "title"_q, "Title");
    html += xml::lite::Element::create("title"_q, "Title");
    auto& body = addChild(html, "body");
    auto& p = addChild(body, "p");
    p = "paragraph";
    std::ignore = addAttribute(p, "a"_q, "abc");
    body += "br"; // addChild()

    output.reset();
    documents.print(output);
    actual = output.stream().str();
    const auto expected1 = // can't use a "raw" string because a string comparision is done, not a "XML comparision"
        "<documents count=\"1\">"
            "<html>"
                "<title>Title</title>"
                "<title>Title</title>"
                "<body>"
                    "<p a=\"abc\">paragraph</p>"
                    "<br/>"
                "</body>"
            "</html>"
        "</documents>";
    TEST_ASSERT_EQ(expected1, actual);
}

int main(int, char**)
{
    TEST_CHECK(testXmlCreateRoot);
    TEST_CHECK(testXmlCreateNested);
}