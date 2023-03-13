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

#include <TestCase.h>
#include "xml/lite/Document.h"
#include "xml/lite/Element.h"
#include "xml/lite/QName.h"
static const std::string test_text = "SOAP Test";

struct SOAPBody : public xml::lite::Element
{
    SOAPBody ()
    {}

    SOAPBody (const xml::lite::QName& qname)
    {
      setQName(qname);
    }
};

struct SOAP : public xml::lite::Document
{
    xml::lite::Element* createElement(const std::string & qname,
                                      const std::string& uri,
                                      std::string characterData = "") {
        xml::lite::Element * elem;
        xml::lite::QName asQName(uri, qname);
        elem = new SOAPBody(asQName);
        elem->setCharacterData(test_text);
        return elem;
    }
};

TEST_CASE(test_overrideCreateElement)
{
    xml::lite::Document *soap_test = new SOAP();
    xml::lite::Element * a = soap_test->createElement("a","b","Not SOAP Test");
    SOAPBody* b = dynamic_cast<SOAPBody*>(a);
    TEST_ASSERT_NOT_NULL(b);
    TEST_ASSERT_EQ(a->getCharacterData(), test_text);
    TEST_ASSERT_EQ(b->getCharacterData(), test_text);
}

int main(int, char**)
{
    TEST_CHECK(test_overrideCreateElement);
}

