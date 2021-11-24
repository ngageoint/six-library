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

namespace
{
typedef xml::lite::Element* XMLElem;
}

namespace six
{
    XMLParser::XMLParser(const std::string& defaultURI, bool addClassAttributes,
        logging::Logger* log, bool ownLog) :
        mDefaultURI(defaultURI), mAddClassAttributes(addClassAttributes)
    {
        setLogger(log, ownLog);
    }
    XMLParser::XMLParser(const std::string& defaultURI, bool addClassAttributes, std::unique_ptr<logging::Logger>&& log) :
        mDefaultURI(defaultURI), mAddClassAttributes(addClassAttributes),
        mLogger(std::move(log)) { }
    XMLParser::XMLParser(const std::string& defaultURI, bool addClassAttributes, logging::Logger& log) :
        mDefaultURI(defaultURI), mAddClassAttributes(addClassAttributes),
        mLogger(log) { }

XMLElem XMLParser::newElement(const std::string& name, XMLElem parent) const
{
    return newElement(name, mDefaultURI, parent);
}
xml::lite::Element& XMLParser::newElement(const std::string& name, xml::lite::Element& parent) const
{
    return *newElement(name, mDefaultURI, &parent);
}

XMLElem XMLParser::newElement(const std::string& name,
        const std::string& uri, XMLElem parent)
{
    return newElement(name, uri, "", parent);
}
xml::lite::Element& XMLParser::newElement(const std::string& name, const std::string& uri, xml::lite::Element& parent)
{
    return *newElement(name, uri, "", &parent);
}

void XMLParser::addClassAttributes(xml::lite::Element& elem, const std::string& type) const
{
    if (mAddClassAttributes)
    {
        setAttribute_(&elem, "class", type, getDefaultURI());
    }
}

XMLElem XMLParser::newElement(const std::string& name,
        const std::string& uri, const std::string& characterData,
        XMLElem parent)
{
    XMLElem elem = xml::lite::Element::create(name, uri, characterData).release();
    if (parent)
        parent->addChild(elem);
    return elem;
}
#if CODA_OSS_lib_char8_t
XMLElem XMLParser::newElement(const std::string& name,
    const std::string& uri, const std::u8string& characterData,
    XMLElem parent)
{
    XMLElem elem = new xml::lite::Element(name, uri, characterData);
    if (parent)
        parent->addChild(elem);
    return elem;
}
#endif

XMLElem XMLParser::createString(const std::string& name,
        const std::string& uri, const std::string& p, XMLElem parent) const
{
    XMLElem const elem = newElement(name, uri, p, parent);
    addClassAttributes(*elem, "xs:string");

    return elem;
}

#if CODA_OSS_lib_char8_t
XMLElem XMLParser::createString(const std::string& name,
    const std::string& uri, const std::u8string& p, XMLElem parent) const
{
    XMLElem const elem = newElement(name, uri, p, parent);
    addClassAttributes(*elem, "xs:string");

    return elem;
}
#endif

XMLElem XMLParser::createString_(const std::string& name,
        const std::string& p, XMLElem parent) const
{
    return createString(name, mDefaultURI, p, parent);
}

template<typename T>
static std::string toString(const std::string& name, T p, const xml::lite::Element* parent)
{
    assert(parent != nullptr);
    try
    {
        return str::toString(p);
    }
    catch (const except::Exception& ex)
    {
        std::string message("Unable to create " + name + " in element "
                + parent->getLocalName() + ": " + ex.getMessage());
        throw except::Exception(Ctxt(message));
    }
}

XMLElem XMLParser::createInt_(const std::string& name, const std::string& uri,
        int p, XMLElem parent) const
{
    const auto elementValue = toString(name, p, parent);
    return createInt(name, uri, elementValue, parent);
}

XMLElem XMLParser::createInt_(const std::string& name, const std::string& uri,
        const std::string& p, XMLElem parent) const
{
    XMLElem const elem = newElement(name, uri, p, parent);
    addClassAttributes(*elem, "xs:int");
    return elem;
}

XMLElem XMLParser::createInt_(const std::string& name, int p, XMLElem parent) const
{
    return createInt(name, mDefaultURI, p, parent);
}

XMLElem XMLParser::createDouble(const std::string& name,
        const std::string& uri, double p, XMLElem parent) const
{
    p = value(p); // be sure this is initialized; throws if not 

    const auto elementValue = toString(name, p, parent);
    XMLElem elem = newElement(name, uri, elementValue, parent);
    addClassAttributes(*elem, "xs:double");

    return elem;
}
XMLElem XMLParser::createDouble(const std::string& name,
    const std::string& uri, const std::optional<double>& p, XMLElem parent) const
{
    return createDouble(name, uri, p.value(), parent);
}
XMLElem XMLParser::createDouble(const std::string& name, double p,
        XMLElem parent) const
{
    return createDouble(name, mDefaultURI, p, parent);
}
XMLElem XMLParser::createDouble(const std::string& name, const std::optional<double>& p,
    XMLElem parent) const
{
    return createDouble(name, p.value(), parent);
}

XMLElem XMLParser::createOptionalDouble(const std::string& name,
    const std::string& uri, const double& p, XMLElem parent) const
{
    return Init::isDefined(p) ? createDouble(name, uri, p, parent) : nullptr;
}
XMLElem XMLParser::createOptionalDouble(const std::string& name,
    const std::string& uri, const std::optional<double>& p, XMLElem parent) const
{
    return p.has_value() ? createDouble(name, uri, *p, parent) : nullptr;
}
XMLElem XMLParser::createOptionalDouble(const std::string& name, const double& p,
        XMLElem parent) const
{
    return Init::isDefined(p) ? createDouble(name, p, parent) : nullptr;
}
XMLElem XMLParser::createOptionalDouble(const std::string& name, const std::optional<double>& p,
    XMLElem parent) const
{
    return p.has_value() ? createOptionalDouble(name, *p, parent) : nullptr;
}

XMLElem XMLParser::createBooleanType(const std::string& name,
        const std::string& uri, BooleanType p, XMLElem parent) const
{
    if (p == six::BooleanType::NOT_SET)
    {
        return nullptr;
    }

    XMLElem const elem =
            newElement(name, uri, six::toString(p), parent);
    addClassAttributes(*elem, "xs:boolean");

    return elem;
}

XMLElem XMLParser::createBooleanType(const std::string& name, BooleanType p,
        XMLElem parent) const
{
    return createBooleanType(name, mDefaultURI, p, parent);
}

XMLElem XMLParser::createDateTime(const std::string& name,
        const std::string& uri, const std::string& s, XMLElem parent) const
{
    XMLElem elem = newElement(name, uri, s, parent);
    addClassAttributes(*elem, "xs:dateTime");

    return elem;
}

XMLElem XMLParser::createDateTime(const std::string& name,
        const std::string& s, XMLElem parent) const
{
    return createDateTime(name, mDefaultURI, s, parent);
}

XMLElem XMLParser::createDateTime(const std::string& name,
        const std::string& uri, const DateTime& p, XMLElem parent) const
{
    return createDateTime(name, uri, six::toString(p), parent);
}

XMLElem XMLParser::createDateTime(const std::string& name, const DateTime& p,
        XMLElem parent) const
{
    return createDateTime(name, mDefaultURI, p, parent);
}

XMLElem XMLParser::createDate(const std::string& name,
        const std::string& uri, const DateTime& p, XMLElem parent) const
{
    XMLElem const elem = newElement(name, uri, p.format("%Y-%m-%d"), parent);
    addClassAttributes(*elem, "xs:date");
    return elem;
}

XMLElem XMLParser::createDate(const std::string& name, const DateTime& p,
        XMLElem parent) const
{
    return createDate(name, mDefaultURI, p, parent);
}

xml::lite::Element& XMLParser::getFirstAndOnly(const xml::lite::Element& parent, const std::string& tag)
{
    return parent.getElementByTagName(tag);
}
XMLElem XMLParser::getFirstAndOnly(const xml::lite::Element* parent, const std::string& tag)
{
    assert(parent != nullptr);
    auto& element = getFirstAndOnly(*parent, tag);
    return &element; // OK, element is a reference
}

xml::lite::Element* XMLParser::getOptional(const xml::lite::Element& parent, const std::string& tag)
{
    return parent.getElementByTagName(std::nothrow, tag);
}
XMLElem XMLParser::getOptional(const xml::lite::Element* parent, const std::string& tag)
{
    assert(parent != nullptr);
    return getOptional(*parent, tag);
}

XMLElem XMLParser::require(XMLElem element, const std::string& name)
{
    if (!element)
    {
        throw except::Exception(Ctxt(
            "Required field [" + name + "] is undefined or null"));
    }
    return element;
}

void XMLParser::setAttribute_(XMLElem e, const std::string& name,
                             const std::string& v, const std::string& uri)
{
    assert(e != nullptr);
    xml::lite::AttributeNode node;
    node.setUri(uri);
    node.setQName(name);
    node.setValue(v);
    e->getAttributes().add(node);
}

template<typename TGetValue>
static bool parseValue(logging::Logger& log, TGetValue getValue)
{
    try
    {
        getValue();
        return true;
    }
    catch (const except::BadCastException& ex)
    {
        log.warn(Ctxt("Unable to parse: " + ex.toString()));
    }
    return false;
}

bool XMLParser::parseDouble(const xml::lite::Element& element, double& value) const
{
    value = Init::undefined<double>();
    const auto getValue = [&]() {
        value = xml::lite::getValue<double>(element);
        assert(Init::isDefined(value)); };
    return parseValue(mLogger.get(), getValue);
}
bool XMLParser::parseDouble(const xml::lite::Element* element, double& value) const
{
    return parseDouble(*element, value);
}
void XMLParser::parseDouble(const xml::lite::Element* element, std::optional<double>& value) const
{
    value.reset(); // be sure callers can determine success/failure

    double result;
    if (parseDouble(element, result))
    {
        value = result;
    }
}

bool XMLParser::parseOptionalDouble(const xml::lite::Element* parent, const std::string& tag, double& value) const
{
    if (const xml::lite::Element* const element = getOptional(parent, tag))
    {
        parseDouble(element, value);
        return true;
    }
    value = six::Init::undefined<double>();
    return false;
}
bool XMLParser::parseOptionalDouble(const xml::lite::Element* parent, const std::string& tag, std::optional<double>& value) const
{
    if (const xml::lite::Element* const element = getOptional(parent, tag))
    {
        parseDouble(element, value);
        return true;
    }
    return false;
}


void XMLParser::parseComplex(const xml::lite::Element* element, std::complex<double>& value) const
{
    double r, i;

    parseDouble(getFirstAndOnly(element, "Real"), r);
    parseDouble(getFirstAndOnly(element, "Imag"), i);

    value = std::complex<double>(r, i);
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

bool  XMLParser::parseOptionalString(const xml::lite::Element& parent, const std::string& tag, std::string& value) const
{
    if (const xml::lite::Element* const element = getOptional(parent, tag))
    {
        parseString(element, value);
        return true;
    }
    return false;
}

void XMLParser::parseBooleanType(const xml::lite::Element* element, BooleanType& value) const
{
    assert(element != nullptr);
    parseValue(mLogger.get(), [&]() {
        value = castValue(*element, six::toType<BooleanType>);
        });
}

void XMLParser::parseDateTime(const xml::lite::Element* element, DateTime& value) const
{
    assert(element != nullptr);
    value = castValue(*element, six::toType<DateTime>);
}
}
