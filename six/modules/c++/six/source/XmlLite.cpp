/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
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
#include <six/XmlLite.h>

#include <assert.h>

#include <string>

#include <nitf/coda-oss.hpp>
#include <except/Exception.h>
#include <str/Convert.h>
#include <logging/NullLogger.h>
#include <six/Utilities.h>
#include <six/Init.h>

namespace six
{
    XmlLite::XmlLite(const std::string& defaultURI, bool addClassAttributes,
        logging::Logger* log, bool ownLog) :
        mDefaultURI(defaultURI), mAddClassAttributes(addClassAttributes)
    {
        setLogger(log, ownLog);
    }
    XmlLite::XmlLite(const std::string& defaultURI, bool addClassAttributes, std::unique_ptr<logging::Logger>&& log) :
        mDefaultURI(defaultURI), mAddClassAttributes(addClassAttributes),
        mLogger(std::move(log)) { }
    XmlLite::XmlLite(const std::string& defaultURI, bool addClassAttributes, logging::Logger& log) :
        mDefaultURI(defaultURI), mAddClassAttributes(addClassAttributes),
        mLogger(log) { }

xml::lite::Element* XmlLite::newElement(const std::string& name, xml::lite::Element* parent) const
{
    return newElement(name, mDefaultURI, parent);
}
xml::lite::Element& XmlLite::newElement(const std::string& name, xml::lite::Element& parent) const
{
    return *newElement(name, mDefaultURI, &parent);
}

xml::lite::Element* XmlLite::newElement(const std::string& name,
        const std::string& uri, xml::lite::Element* parent)
{
    return newElement(name, uri, "", parent);
}
xml::lite::Element& XmlLite::newElement(const std::string& name, const std::string& uri, xml::lite::Element& parent)
{
    return *newElement(name, uri, "", &parent);
}

static void addClassAttributes_(xml::lite::Element& elem, const std::string& type, const std::string& uri)
{
    XmlLite::setAttribute_(elem, "class", type, uri);
}
void XmlLite::addClassAttributes(xml::lite::Element& elem, const std::string& type) const
{
    if (mAddClassAttributes)
    {
        addClassAttributes_(elem, type, getDefaultURI());
    }
}

xml::lite::Element* XmlLite::newElement(const std::string& name,
        const std::string& uri, const std::string& characterData,
        xml::lite::Element* parent)
{
    xml::lite::Element* elem = xml::lite::Element::create(name, uri, characterData).release();
    if (parent)
        parent->addChild(elem);
    return elem;
}
#if CODA_OSS_lib_char8_t
xml::lite::Element* XmlLite::newElement(const std::string& name,
    const std::string& uri, const std::u8string& characterData,
    xml::lite::Element* parent)
{
    xml::lite::Element* elem = new xml::lite::Element(name, uri, characterData);
    if (parent)
        parent->addChild(elem);
    return elem;
}
#endif

xml::lite::Element& XmlLite::createString(const std::string& name,
        const std::string& uri, const std::string& p, xml::lite::Element& parent) const
{
    auto pElem = newElement(name, uri, p, &parent);
    auto& elem = *pElem;
    addClassAttributes(elem, "xs:string");

    return elem;
}
xml::lite::Element& XmlLite::createString(const std::string& name,
    const std::string& p, xml::lite::Element& parent) const
{
    return createString(name, mDefaultURI, p, parent);
}

#if CODA_OSS_lib_char8_t
xml::lite::Element* XmlLite::createString(const std::string& name,
    const std::string& uri, const std::u8string& p, xml::lite::Element* parent) const
{
    xml::lite::Element* const elem = newElement(name, uri, p, parent);
    addClassAttributes(*elem, "xs:string");

    return elem;
}
#endif

xml::lite::Element* XmlLite::createString_(const std::string& name,
        const std::string& p, xml::lite::Element* parent) const
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
template<typename T>
inline std::string toString_(const std::string& name, const T& v, const xml::lite::Element& parent)
{
    return toString(name, v, &parent);
}

template<typename T, typename ToString>
static xml::lite::Element& createValue(const std::string& name, const std::string& uri,
    const T& v, xml::lite::Element& parent,
    bool addClassAttributes, const std::string& type, const std::string& attributeUri,
    ToString toString)
{
    auto& elem = xml::lite::createElement(name, uri, v, parent, toString);
    if (addClassAttributes)
    {
        addClassAttributes_(elem, type, attributeUri);
    }

    return elem;
}
template<typename T>
inline xml::lite::Element& createValue(const std::string& name, const std::string& uri,
    const T& v, xml::lite::Element& parent,
    bool addClassAttributes, const std::string& type, const std::string& attributeUri)
{
    const auto toString = [&](const T& v) { return toString_(name, v, parent); };
    return createValue(name, uri, v, parent, addClassAttributes, type, attributeUri, toString);
}

template<typename T>
static xml::lite::Element* createOptionalValue(const std::string& name, const std::string& uri,
    const std::optional<T>& v, xml::lite::Element& parent,
    bool addClassAttributes, const std::string& type, const std::string& attributeUri)
{
    if (v.has_value())
    {
        return &createValue(name, uri, v.value(), parent, addClassAttributes, type, attributeUri);
    }
    return nullptr;
}


xml::lite::Element* XmlLite::createInt_(const std::string& name, const std::string& uri,
        int p, xml::lite::Element* parent) const
{
    assert(parent != nullptr);
    return &createValue(name, uri, p, *parent, mAddClassAttributes, "xs:int", getDefaultURI());
}
xml::lite::Element* XmlLite::createInt_(const std::string& name, const std::string& uri,
    const std::string& p, xml::lite::Element* parent) const
{
    assert(parent != nullptr);
    xml::lite::Element* const elem = newElement(name, uri, p, parent);
    addClassAttributes(*elem, "xs:int");
    return elem;
}
xml::lite::Element* XmlLite::createInt_(const std::string& name, int p, xml::lite::Element* parent) const
{
    return createInt(name, mDefaultURI, p, parent);
}

xml::lite::Element* XmlLite::createDouble(const std::string& name,
        const std::string& uri, double p, xml::lite::Element* parent) const
{
    assert(parent != nullptr);

    p = value(p); // be sure this is initialized; throws if not
    return &createValue(name, uri, p, *parent, mAddClassAttributes, "xs::double", getDefaultURI());
}
xml::lite::Element& XmlLite::createDouble(const std::string& name, double p,
        xml::lite::Element& parent) const
{
    return *createDouble(name, mDefaultURI, p, &parent);
}
xml::lite::Element* XmlLite::createDouble(const std::string& name, const std::optional<double>& p,
    xml::lite::Element* parent) const
{
    return createDouble(name, mDefaultURI, p.value(), parent);
}

xml::lite::Element* XmlLite::createOptionalDouble(const std::string& name,
    const std::string& uri, const double& p, xml::lite::Element* parent) const
{
    return Init::isDefined(p) ? createDouble(name, uri, p, parent) : nullptr;
}
xml::lite::Element* XmlLite::createOptionalDouble(const std::string& name,
    const std::string& uri, const std::optional<double>& p, xml::lite::Element* parent) const
{
    assert(parent != nullptr);
    return createOptionalValue(name, uri, p, *parent, mAddClassAttributes, "xs::double", getDefaultURI());
}
xml::lite::Element* XmlLite::createOptionalDouble(const std::string& name, const double& p,
        xml::lite::Element* parent) const
{
    return createOptionalDouble(name, mDefaultURI, p, parent);
}
xml::lite::Element* XmlLite::createOptionalDouble(const std::string& name, const std::optional<double>& p,
    xml::lite::Element* parent) const
{
    return createOptionalDouble(name, mDefaultURI, p, parent);
}

xml::lite::Element* XmlLite::createBooleanType(const std::string& name,
        const std::string& uri, BooleanType p, xml::lite::Element* parent) const
{
    if (p == six::BooleanType::NOT_SET)
    {
        return nullptr;
    }

    assert(parent != nullptr);
    const auto toString = [&](const BooleanType& v) { return six::toString(v); };
    return &createValue(name, uri, p, *parent,
        mAddClassAttributes, "xs:boolean", getDefaultURI(),
        toString);
}
xml::lite::Element* XmlLite::createBooleanType(const std::string& name, BooleanType p,
        xml::lite::Element* parent) const
{
    return createBooleanType(name, mDefaultURI, p, parent);
}

xml::lite::Element* XmlLite::createDateTime(const std::string& name,
        const std::string& uri, const std::string& s, xml::lite::Element* parent) const
{
    assert(parent != nullptr);
    return &createValue(name, uri, s, *parent,
        mAddClassAttributes, "xs:dateTime", getDefaultURI());
}
xml::lite::Element* XmlLite::createDateTime(const std::string& name,
        const std::string& s, xml::lite::Element* parent) const
{
    return createDateTime(name, mDefaultURI, s, parent);
}
xml::lite::Element* XmlLite::createDateTime(const std::string& name,
        const std::string& uri, const DateTime& p, xml::lite::Element* parent) const
{
    assert(parent != nullptr);

    const auto toString = [&](const DateTime& v) { return six::toString(v); };
    return &createValue(name, uri, p, *parent,
        mAddClassAttributes, "xs:dateTime", getDefaultURI(),
        toString);
}
xml::lite::Element* XmlLite::createDateTime(const std::string& name, const DateTime& p,
        xml::lite::Element* parent) const
{
    return createDateTime(name, mDefaultURI, p, parent);
}

xml::lite::Element* XmlLite::createDate(const std::string& name,
        const std::string& uri, const DateTime& p, xml::lite::Element* parent) const
{
    assert(parent != nullptr);

    const auto toString = [&](const DateTime& p) { return p.format("%Y-%m-%d"); };
    return &createValue(name, uri, p, *parent,
        mAddClassAttributes, "xs:date", getDefaultURI(),
        toString);
}
xml::lite::Element* XmlLite::createDate(const std::string& name, const DateTime& p,
        xml::lite::Element* parent) const
{
    return createDate(name, mDefaultURI, p, parent);
}

xml::lite::Element& XmlLite::getFirstAndOnly(const xml::lite::Element& parent, const std::string& tag)
{
    return parent.getElementByTagName(tag);
}

xml::lite::Element* XmlLite::getOptional(const xml::lite::Element& parent, const std::string& tag)
{
    return parent.getElementByTagName(std::nothrow, tag);
}

xml::lite::Element& XmlLite::require(xml::lite::Element* element, const std::string& name)
{
    if (element == nullptr)
    {
        throw except::Exception(Ctxt("Required field [" + name + "] is undefined or null"));
    }
    return *element;
}

void XmlLite::setAttribute_(xml::lite::Element& e, const std::string& name,
                             const std::string& v, const std::string& uri)
{
    xml::lite::AttributeNode node;
    node.setUri(uri);
    node.setQName(name);
    node.setValue(v);
    e.getAttributes().add(node);
}
void XmlLite::setAttribute_(xml::lite::Element* e, const std::string& name,
    const std::string& v, const std::string& uri)
{
    assert(e != nullptr);
    setAttribute_(*e, name, v, uri);
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

bool XmlLite::parseDouble(const xml::lite::Element& element, double& value) const
{
    value = Init::undefined<double>();
    const auto getValue = [&]() {
        value = xml::lite::getValue<double>(element);
        assert(Init::isDefined(value)); };
    return parseValue(mLogger.get(), getValue);
}
void XmlLite::parseDouble(const xml::lite::Element& element, std::optional<double>& value) const
{
    value.reset(); // be sure callers can determine success/failure

    double result;
    if (parseDouble(element, result))
    {
        value = result;
    }
}

bool XmlLite::parseOptionalDouble(const xml::lite::Element& parent, const std::string& tag, double& value) const
{
    if (const xml::lite::Element* const element = getOptional(parent, tag))
    {
        parseDouble(*element, value);
        return true;
    }
    value = six::Init::undefined<double>();
    return false;
}
bool XmlLite::parseOptionalDouble(const xml::lite::Element& parent, const std::string& tag, std::optional<double>& value) const
{
    if (const xml::lite::Element* const element = getOptional(parent, tag))
    {
        parseDouble(*element, value);
        return true;
    }
    return false;
}

void XmlLite::parseComplex(const xml::lite::Element& element, std::complex<double>& value) const
{
    double r, i;

    parseDouble(getFirstAndOnly(element, "Real"), r);
    parseDouble(getFirstAndOnly(element, "Imag"), i);

    value = std::complex<double>(r, i);
}

void XmlLite::parseString(const xml::lite::Element& element, std::string& value) const
{
    value = element.getCharacterData();
}
void XmlLite::parseString(const xml::lite::Element* element, std::string& value) const
{
    assert(element != nullptr);
    parseString(*element, value);
}

bool  XmlLite::parseOptionalString(const xml::lite::Element& parent, const std::string& tag, std::string& value) const
{
    if (const xml::lite::Element* const element = getOptional(parent, tag))
    {
        parseString(element, value);
        return true;
    }
    return false;
}

void XmlLite::parseBooleanType(const xml::lite::Element& element, BooleanType& value) const
{
    parseValue(mLogger.get(), [&]() {
        value = castValue(element, six::toType<BooleanType>);
        });
}

void XmlLite::parseDateTime(const xml::lite::Element& element, DateTime& value) const
{
    value = castValue(element, six::toType<DateTime>);
}
}
