/* =========================================================================
 * This file is part of xml.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
  * (C) Copyright 2023, Maxar Technologies, Inc.
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

#include <TestCase.h>
#include "xml/lite/Document.h"
#include "xml/lite/Element.h"
#include "xml/lite/QName.h"

static const std::string& test_text()
{
    static const std::string retval = "SOAP Test";
    return retval;
}

struct SOAPBody final : public xml::lite::Element
{ 
    SOAPBody() = default;
    SOAPBody (const xml::lite::QName& qname)
    {
      setQName(qname);
    }
};

struct SOAP final : public xml::lite::Document
{
    xml::lite::Element* createElement(const std::string & qname,
                                      const std::string& uri,
                                      std::string characterData = "") override {
        const xml::lite::QName asQName(uri, qname);
        xml::lite::Element*  elem = new SOAPBody(asQName);
        elem->setCharacterData(characterData); // avoid unused parameter warning
        elem->setCharacterData(test_text());
        return elem;
    }
};

TEST_CASE(test_overrideCreateElement)
{
    SOAP soap_test;
    std::unique_ptr<xml::lite::Element> a(soap_test.createElement("a","b","Not SOAP Test"));
    auto b = dynamic_cast<const SOAPBody*>(a.get());
    TEST_ASSERT_NOT_NULL(b);
    TEST_ASSERT_EQ(a->getCharacterData(), test_text());
    TEST_ASSERT_EQ(b->getCharacterData(), test_text());
}

TEST_MAIN
(
    TEST_CHECK(test_overrideCreateElement);
)

