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
#include <stdexcept>

#include <nitf/coda-oss.hpp>
#include <except/Exception.h>
#include <str/Convert.h>
#include <logging/NullLogger.h>
#include <six/Utilities.h>
#include <six/Init.h>
#include <six/XmlValueElement.h>
#include <six/XmlOptionalElement.h>

namespace six
{
    struct MinidomParser::Impl final
    {
        xml::lite::MinidomParser parser;
        Impl() : parser() {}
        Impl(const Impl&) = delete;
        Impl& operator=(const Impl&) = delete;
        Impl(Impl&&) = delete;
        Impl& operator=(Impl&&) = delete;
    };

    MinidomParser::MinidomParser()
        : pImpl(std::make_unique<Impl>())
    {
    }
    MinidomParser::~MinidomParser() = default;
    MinidomParser::MinidomParser(MinidomParser&&) = default;
    MinidomParser& MinidomParser::operator=(MinidomParser&&) = default;

    void MinidomParser::parse(io::InputStream& is, int size)
    {
        pImpl->parser.parse(is, size);
    }
    void MinidomParser::clear()
    {
        pImpl->parser.clear();
    }

    const xml::lite::Document& MinidomParser::getDocument() const
    {
        return *(pImpl->parser.getDocument());
    }
    void MinidomParser::getDocument(std::unique_ptr<xml::lite::Document>& pDoc)
    {
        pImpl->parser.getDocument(pDoc);
    }

    void MinidomParser::preserveCharacterData(bool preserve)
    {
        pImpl->parser.preserveCharacterData(preserve);
    }

    XmlLite::XmlLite(const xml::lite::Uri& defaultURI, bool addClassAttributes,
        logging::Logger* log, bool ownLog) :
        mDefaultURI(defaultURI), mAddClassAttributes(addClassAttributes)
    {
        setLogger(log, ownLog);
    }
    XmlLite::XmlLite(const xml::lite::Uri& defaultURI, bool addClassAttributes, std::unique_ptr<logging::Logger>&& log) :
        mDefaultURI(defaultURI), mAddClassAttributes(addClassAttributes),
        mLogger(std::move(log)) { }
    XmlLite::XmlLite(const xml::lite::Uri& defaultURI, bool addClassAttributes, logging::Logger& log) :
        mDefaultURI(defaultURI), mAddClassAttributes(addClassAttributes),
        mLogger(log) { }

xml::lite::Element* XmlLite::newElement(const std::string& name, xml::lite::Element* parent) const
{
    const auto qname = makeQName(name);
    // prefer reference version as that minimizes the opportunity for memory leaks
    return parent != nullptr ? &newElement(qname, *parent) : newElement(qname, parent);
}
xml::lite::Element& XmlLite::newElement(const std::string& name, xml::lite::Element& parent) const
{
    return newElement(makeQName(name), parent);
}
xml::lite::Element* XmlLite::newElement(const xml::lite::QName& name, xml::lite::Element* parent)
{
    // prefer reference version as that minimizes the opportunity for memory leaks
    return parent != nullptr ? &newElement(name, *parent) : newElement(name, "", parent);
}
xml::lite::Element& XmlLite::newElement(const xml::lite::QName& name, xml::lite::Element& parent)
{
    return newElement(name, "", parent);
}

static void addClassAttributes(xml::lite::Element& elem, const std::string& type, const xml::lite::Uri& uri)
{
    XmlLite::setAttribute(elem, xml::lite::QName(uri, "class"), type);
}

xml::lite::Element* XmlLite::newElement(const xml::lite::QName& name, const std::string& characterData, xml::lite::Element* parent)
{
    if (parent != nullptr)
    {
        // prefer reference version as that minimizes the opportunity for memory leaks
        return &newElement(name, characterData, *parent);
    }
    return xml::lite::Element::create(name, characterData).release(); // caller must delete!
}
xml::lite::Element& XmlLite::newElement(const xml::lite::QName& name, const std::string& characterData, xml::lite::Element& parent)
{
    auto elem = xml::lite::Element::create(name, characterData);
    return parent.addChild(std::move(elem));
}
xml::lite::Element* XmlLite::newElement(const xml::lite::QName& name, const std::u8string& characterData, xml::lite::Element* parent)
{
    if (parent != nullptr)
    {
        // prefer reference version as that minimizes the opportunity for memory leaks
        return &newElement(name, characterData, *parent);
    }

    return xml::lite::Element::create(name, characterData).release();  // caller must delete!
}
xml::lite::Element& XmlLite::newElement(const xml::lite::QName& name, const std::u8string& characterData, xml::lite::Element& parent)
{
    auto elem = xml::lite::Element::create(name, characterData);
    return parent.addChild(std::move(elem));
}

xml::lite::Element& XmlLite::createString(const xml::lite::QName& name, const std::string& p, xml::lite::Element& parent) const
{
    auto& elem = newElement(name, p, parent);
    if (mAddClassAttributes)
    {
        addClassAttributes(elem, "xs:string", getDefaultURI());
    }
    return elem;
}
xml::lite::Element& XmlLite::createString(const std::string& name,
    const std::string& p, xml::lite::Element& parent) const
{
    return createString(makeQName(name), p, parent);
}
xml::lite::Element& XmlLite::createString(const xml::lite::QName& name, const std::u8string& p, xml::lite::Element& parent) const
{
    auto& elem = newElement(name, p, parent);
    if (mAddClassAttributes)
    {
        addClassAttributes(elem, "xs:string", getDefaultURI());
    }
    return elem;
}

xml::lite::Element& XmlLite::createString_(const std::string& name,
        const std::string& p, xml::lite::Element& parent) const
{
    return createString(makeQName(name), p, parent);
}

template<typename T>
static std::string toString(const xml::lite::QName& name, const T& p, const xml::lite::Element& parent)
{
    try
    {
        return str::toString(p);
    }
    catch (const except::Exception& ex)
    {
        std::string message("Unable to create " + name.getName() + " in element "
                + parent.getLocalName() + ": " + ex.getMessage());
        throw except::Exception(Ctxt(message));
    }
}
template<typename T>
inline std::string toString_(const xml::lite::QName& name, const T& v, const xml::lite::Element& parent)
{
    return toString(name, v, parent);
}

template<typename T, typename ToString>
static xml::lite::Element& createValue(const xml::lite::QName& name,
    const T& v, xml::lite::Element& parent,
    bool bAddClassAttributes, const std::string& type, const xml::lite::Uri& attributeUri,
    ToString toString)
{
    auto& elem = xml::lite::addNewElement(name, v, parent, toString);
    if (bAddClassAttributes)
    {
        addClassAttributes(elem, type, attributeUri);
    }

    return elem;
}
template<typename T>
inline xml::lite::Element& createValue(const xml::lite::QName& name,
    const T& v, xml::lite::Element& parent,
    bool addClassAttributes, const std::string& type, const xml::lite::Uri& attributeUri)
{
    const auto toString = [&](const T& v) { return toString_(name, v, parent); };
    return createValue(name, v, parent, addClassAttributes, type, attributeUri, toString);
}

template<typename T>
static xml::lite::Element* createOptionalValue(const xml::lite::QName& name,
    const std::optional<T>& v, xml::lite::Element& parent,
    bool addClassAttributes, const std::string& type, const xml::lite::Uri& attributeUri)
{
    if (v.has_value())
    {
        return &createValue(name, v.value(), parent, addClassAttributes, type, attributeUri);
    }
    return nullptr;
}

xml::lite::Element& XmlLite::createInt(const xml::lite::QName& name,int p, xml::lite::Element& parent) const
{
    return createValue(name, p, parent, mAddClassAttributes, "xs:int", getDefaultURI());
}
xml::lite::Element& XmlLite::createInt(const xml::lite::QName& name, const std::string& p, xml::lite::Element& parent) const
{
    auto& elem = newElement(name, p, parent);
    if (mAddClassAttributes)
    {
        addClassAttributes(elem, "xs:int", getDefaultURI());
    }
    return elem;
}
xml::lite::Element& XmlLite::createInt_(const std::string& name, int p, xml::lite::Element& parent) const
{
    return createInt(makeQName(name), p, parent);
}

xml::lite::Element& XmlLite::createDouble(const xml::lite::QName& name, double p, xml::lite::Element& parent) const
{
    p = value(p); // be sure this is initialized; throws if not
    return createValue(name, p, parent, mAddClassAttributes, "xs:double", getDefaultURI());
}
xml::lite::Element& XmlLite::createDouble(const xml::lite::QName& name, const std::optional<double>& p, xml::lite::Element& parent) const
{
    return createDouble(name, p.value(), parent);
}
xml::lite::Element& XmlLite::createDouble(const std::string& name, double p, xml::lite::Element& parent) const
{
    return createDouble(makeQName(name), p, parent);
}
xml::lite::Element& XmlLite::createDouble(const std::string& name, const std::optional<double>& p, xml::lite::Element& parent) const
{
    return createDouble(name, p.value(), parent);
}

xml::lite::Element* XmlLite::createOptionalDouble(const xml::lite::QName& name, double p, xml::lite::Element& parent) const
{
    return Init::isDefined(p) ? &createDouble(name, p, parent) : nullptr;
}
xml::lite::Element* XmlLite::createOptionalDouble(const std::string& name, double p, xml::lite::Element& parent) const
{
    return createOptionalDouble(makeQName(name), p, parent);
}
xml::lite::Element* XmlLite::createOptionalDouble(const xml::lite::QName& name, const std::optional<double>& p, xml::lite::Element& parent) const
{
    return createOptionalValue(name, p, parent, mAddClassAttributes, "xs:double", getDefaultURI());
}
xml::lite::Element* XmlLite::createOptionalDouble(const std::string& name, const std::optional<double>& p, xml::lite::Element& parent) const
{
    return createOptionalDouble(makeQName(name), p, parent);
}

xml::lite::Element* XmlLite::createBooleanType(const xml::lite::QName& name, BooleanType p, xml::lite::Element& parent) const
{
    if (p == six::BooleanType::NOT_SET)
    {
        return nullptr;
    }

    const auto toString = [&](const BooleanType& v) { return six::toString(v); };
    return &createValue(name, p, parent,
        mAddClassAttributes, "xs:boolean", getDefaultURI(),
        toString);
}
xml::lite::Element* XmlLite::createBooleanType(const std::string& name, BooleanType p, xml::lite::Element& parent) const
{
    return createBooleanType(makeQName(name), p, parent);
}
xml::lite::Element* XmlLite::createOptional(const std::string& name, const std::optional<bool>& v, xml::lite::Element& parent) const
{
    if (!v.has_value())
    {
        return nullptr;
    }
    const auto p = *v ? BooleanType::IS_TRUE : BooleanType::IS_FALSE;
    return createBooleanType(name, p, parent);
}

xml::lite::Element* XmlLite::createOptional(const std::string& name, const std::optional<std::u8string>& v, xml::lite::Element& parent) const
{
    return createOptionalValue(makeQName(name), v, parent, mAddClassAttributes, "xs:string", getDefaultURI());
}

xml::lite::Element& XmlLite::createDateTime(const xml::lite::QName& name, const DateTime& p, xml::lite::Element& parent) const
{
    return createValue(name, six::toString(p), parent,
        mAddClassAttributes, "xs:dateTime", getDefaultURI());
}
xml::lite::Element& XmlLite::createDateTime(const std::string& name, const DateTime& p, xml::lite::Element& parent) const
{
    return createDateTime(makeQName(name), p, parent);
}

xml::lite::Element& XmlLite::createDate(const xml::lite::QName& name, const DateTime& p, xml::lite::Element& parent) const
{
    const auto toString = [&](const DateTime& p) { return p.format("%Y-%m-%d"); };
    return createValue(name, p, parent,
        mAddClassAttributes, "xs:date", getDefaultURI(),
        toString);
}
xml::lite::Element& XmlLite::createDate(const std::string& name, const DateTime& p,
        xml::lite::Element& parent) const
{
    return createDate(makeQName(name), p, parent);
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

void XmlLite::setAttribute(xml::lite::Element& e, const xml::lite::QName& name, const std::string& v)
{
    xml::lite::AttributeNode node;
    node.setQName(name);
    node.setValue(v);
    e.getAttributes().add(node);
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

void XmlLite::parseComplex(const xml::lite::Element& element, six::zdouble& value) const
{
    double r, i;

    parseDouble(getFirstAndOnly(element, "Real"), r);
    parseDouble(getFirstAndOnly(element, "Imag"), i);

    value = six::zdouble(r, i);
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
bool  XmlLite::parseOptional(const xml::lite::Element& parent, const std::string& tag, std::optional<std::u8string>& value) const
{
    if (const xml::lite::Element* const element = getOptional(parent, tag))
    {
        value = getCharacterData(*element);
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
BooleanType XmlLite::parseBooleanType(const xml::lite::Element& element) const
{
    BooleanType retval;
    parseBooleanType(element, retval);
    return retval;
}
bool XmlLite::parseBoolean(const xml::lite::Element& element) const
{
    switch (parseBooleanType(element))
    {
        case BooleanType::IS_TRUE: return true;
        case BooleanType::IS_FALSE: return false;
        default: break;
    }
    throw std::logic_error("Unknown 'BooleanType' value.");
}

bool XmlLite::parseOptional(const xml::lite::Element& parent, const std::string& tag, std::optional<bool>& value) const
{
    if (const xml::lite::Element* const element = getOptional(parent, tag))
    {
        value = parseBoolean(*element);
        return true;
    }
    return false;
}

void XmlLite::parseDateTime(const xml::lite::Element& element, DateTime& value) const
{
    value = castValue(element, six::toType<DateTime>);
}

xml::lite::QName XmlLite::makeQName(const std::string& name) const
{
    return xml::lite::QName(getDefaultURI(), name);
}

xml::lite::Element& create(XmlLite& parser, const XmlValueElement<double>& v, xml::lite::Element& parent)
{
    return parser.createDouble(v.name(), v.value(), parent);
}
void getFirstAndOnly(const XmlLite& parser, const xml::lite::Element& parent, XmlValueElement<double>& v)
{
    auto& element = parser.getFirstAndOnly(parent, v.name());
    v.value(xml::lite::getValue<double>(element)); // throws except::BadCastException on failure, see parseDouble()
}

xml::lite::Element* create(XmlLite& parser, const XmlOptionalElement<bool>& v, xml::lite::Element& parent)
{
    return parser.createOptional(v.name(), v.value(), parent);
}
xml::lite::Element* create(XmlLite& parser, const XmlOptionalElement<double>& v, xml::lite::Element& parent)
{
    return parser.createOptionalDouble(v.name(), v.value(), parent);
}
xml::lite::Element* create(XmlLite& parser, const XmlOptionalElement<std::u8string>& v, xml::lite::Element& parent)
{
    return parser.createOptional(v.name(), v.value(), parent);
}

bool parseOptional(const XmlLite& parser, const xml::lite::Element& parent, XmlOptionalElement<bool>& v)
{
    return parser.parseOptional(parent, v.name(), v.value());
}
bool parseOptional(const XmlLite& parser, const xml::lite::Element& parent, XmlOptionalElement<double>& v)
{
    return parser.parseOptionalDouble(parent, v.name(), v.value());
}
bool parseOptional(const XmlLite& parser, const xml::lite::Element& parent, XmlOptionalElement<std::u8string>& v)
{
    return parser.parseOptional(parent, v.name(), v.value());
}

}
