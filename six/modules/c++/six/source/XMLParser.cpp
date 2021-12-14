/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#include <six/XMLParser.h>

#include <assert.h>

#include <string>

#include <nitf/coda-oss.hpp>
#include <except/Exception.h>
#include <str/Convert.h>
#include <logging/NullLogger.h>
#include <six/Utilities.h>
#include <six/Init.h>
#include <six/XmlLite.h>

namespace
{
typedef xml::lite::Element* XMLElem;
}

namespace six
{
    XMLParser::XMLParser(const std::string& defaultURI, bool addClassAttributes,
        logging::Logger* log, bool ownLog) : mXmlLite(xml::lite::Uri(defaultURI), addClassAttributes, log, ownLog)
    {
    }
    XMLParser::XMLParser(const std::string& defaultURI, bool addClassAttributes, std::unique_ptr<logging::Logger>&& log) :
        mXmlLite(xml::lite::Uri(defaultURI), addClassAttributes, std::move(log)) { }
    XMLParser::XMLParser(const std::string& defaultURI, bool addClassAttributes, logging::Logger& log) :
        mXmlLite(xml::lite::Uri(defaultURI), addClassAttributes, log) { }

XMLElem XMLParser::newElement(const std::string& name, XMLElem parent) const
{
    return mXmlLite.newElement(name, parent);
}
xml::lite::Element& XMLParser::newElement(const std::string& name, xml::lite::Element& parent) const
{
    return mXmlLite.newElement(name, parent);
}

XMLElem XMLParser::newElement(const std::string& name,
        const std::string& uri, XMLElem parent)
{
    return XmlLite::newElement(xml::lite::QName(uri, name), parent);
}
xml::lite::Element& XMLParser::newElement(const std::string& name, const std::string& uri, xml::lite::Element& parent)
{
    return  XmlLite::newElement(xml::lite::QName(uri, name), parent);
}

XMLElem XMLParser::newElement(const std::string& name,
        const std::string& uri, const std::string& characterData,
        XMLElem parent)
{
    return XmlLite::newElement(xml::lite::QName(uri, name), characterData, parent);
}
#if CODA_OSS_lib_char8_t
XMLElem XMLParser::newElement(const std::string& name,
    const std::string& uri, const std::u8string& characterData,
    XMLElem parent)
{
    return XmlLite::newElement(xml::lite::QName(uri, name), characterData, parent);
}
#endif

XMLElem XMLParser::createString(const std::string& name,
        const std::string& uri, const std::string& p, XMLElem parent) const
{
    return mXmlLite.createString(name, xml::lite::Uri(uri), p, parent);
}

#if CODA_OSS_lib_char8_t
XMLElem XMLParser::createString(const std::string& name,
    const std::string& uri, const std::u8string& p, XMLElem parent) const
{
    return mXmlLite.createString(name, xml::lite::Uri(uri), p, parent);
}
#endif

XMLElem XMLParser::createDouble(const std::string& name,
        const std::string& uri, double p, XMLElem parent) const
{
    return mXmlLite.createDouble(name, xml::lite::Uri(uri), p, parent);
}
XMLElem XMLParser::createDouble(const std::string& name,
    const std::string& uri, const std::optional<double>& p, XMLElem parent) const
{
    return createDouble(name, uri, p.value(), parent);
}
xml::lite::Element& XMLParser::createDouble(const std::string& name, double p, xml::lite::Element& parent) const
{
    return mXmlLite.createDouble(name, p, parent);
}
XMLElem XMLParser::createDouble(const std::string& name, double p,
        XMLElem parent) const
{
    assert(parent != nullptr);
    return &createDouble(name, p, *parent);
}
XMLElem XMLParser::createDouble(const std::string& name, const std::optional<double>& p,
    XMLElem parent) const
{
    return createDouble(name, p.value(), parent);
}

XMLElem XMLParser::createOptionalDouble(const std::string& name,
    const std::string& uri, const double& p, XMLElem parent) const
{
    return mXmlLite.createOptionalDouble(name, xml::lite::Uri(uri), p, parent);
}
XMLElem XMLParser::createOptionalDouble(const std::string& name,
    const std::string& uri, const std::optional<double>& p, XMLElem parent) const
{
    return mXmlLite.createOptionalDouble(name, xml::lite::Uri(uri), p, parent);
}
XMLElem XMLParser::createOptionalDouble(const std::string& name, const double& p,
        XMLElem parent) const
{
    return mXmlLite.createOptionalDouble(name, p, parent);
}
XMLElem XMLParser::createOptionalDouble(const std::string& name, const std::optional<double>& p,
    XMLElem parent) const
{
    return mXmlLite.createOptionalDouble(name, p, parent);
}

XMLElem XMLParser::createBooleanType(const std::string& name,
        const std::string& uri, BooleanType p, XMLElem parent) const
{
    return mXmlLite.createBooleanType(name, xml::lite::Uri(uri), p, parent);
}
XMLElem XMLParser::createBooleanType(const std::string& name, BooleanType p,
        XMLElem parent) const
{
    return mXmlLite.createBooleanType(name, p, parent);
}

XMLElem XMLParser::createDateTime(const std::string& name,
        const std::string& uri, const DateTime& p, XMLElem parent) const
{
    return mXmlLite.createDateTime(name, xml::lite::Uri(uri), p, parent);
}

XMLElem XMLParser::createDateTime(const std::string& name, const DateTime& p,
        XMLElem parent) const
{
    return mXmlLite.createDateTime(name, p, parent);
}

XMLElem XMLParser::createDate(const std::string& name,
        const std::string& uri, const DateTime& p, XMLElem parent) const
{
    return mXmlLite.createDate(name, xml::lite::Uri(uri), p, parent);
}
XMLElem XMLParser::createDate(const std::string& name, const DateTime& p,
        XMLElem parent) const
{
    return mXmlLite.createDate(name, p, parent);
}

xml::lite::Element& XMLParser::getFirstAndOnly(const xml::lite::Element& parent, const std::string& tag)
{
    return XmlLite::getFirstAndOnly(parent, tag);
}
XMLElem XMLParser::getFirstAndOnly(const xml::lite::Element* parent, const std::string& tag)
{
    assert(parent != nullptr);
    return & getFirstAndOnly(*parent, tag);
}

xml::lite::Element* XMLParser::getOptional(const xml::lite::Element& parent, const std::string& tag)
{
    return XmlLite::getOptional(parent, tag);
}
XMLElem XMLParser::getOptional(const xml::lite::Element* parent, const std::string& tag)
{
    assert(parent != nullptr);
    return getOptional(*parent, tag);
}

XMLElem XMLParser::require(XMLElem element, const std::string& name)
{
    return & XmlLite::require(element, name);
}

bool XMLParser::parseDouble(const xml::lite::Element& element, double& value) const
{
    return mXmlLite.parseDouble(element, value);
}
bool XMLParser::parseDouble(const xml::lite::Element* element, double& value) const
{
    assert(element != nullptr);
    return mXmlLite.parseDouble(*element, value);
}
void XMLParser::parseDouble(const xml::lite::Element* element, std::optional<double>& value) const
{
    assert(element != nullptr);
    mXmlLite.parseDouble(*element, value);
}

bool XMLParser::parseOptionalDouble(const xml::lite::Element* parent, const std::string& tag, double& value) const
{
    assert(parent != nullptr);
    return mXmlLite.parseOptionalDouble(*parent, tag, value);
}
bool XMLParser::parseOptionalDouble(const xml::lite::Element* parent, const std::string& tag, std::optional<double>& value) const
{
    assert(parent != nullptr);
    return mXmlLite.parseOptionalDouble(*parent, tag, value);
}

void XMLParser::parseComplex(const xml::lite::Element* element, std::complex<double>& value) const
{
    assert(element != nullptr);
    mXmlLite.parseComplex(*element, value);
}

void XMLParser::parseString(const xml::lite::Element& element, std::string& value) const
{
    value = element.getCharacterData();
}
void XMLParser::parseString(const xml::lite::Element* element, std::string& value) const
{
    assert(element != nullptr);
    parseString(*element, value);
}

bool XMLParser::parseOptionalString(const xml::lite::Element& parent, const std::string& tag, std::string& value) const
{
    return mXmlLite.parseOptionalString(parent, tag, value);
}
}
