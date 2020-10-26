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

#include <sys/Conf.h>
#include <except/Exception.h>
#include <str/Convert.h>
#include <logging/NullLogger.h>
#include <six/XMLParser.h>
#include <six/Utilities.h>

namespace
{
typedef xml::lite::Element* XMLElem;
}

namespace six
{
XMLParser::XMLParser(const std::string& defaultURI,
                     bool addClassAttributes,
                     logging::Logger* log,
                     bool ownLog) :
    mDefaultURI(defaultURI),
    mAddClassAttributes(addClassAttributes),
    mLog(NULL),
    mOwnLog(false)
{
    setLogger(log, ownLog);
}

XMLParser::~XMLParser()
{
    if (mOwnLog)
    {
        delete mLog;
    }
}

void XMLParser::setLogger(logging::Logger* log, bool own)
{
    if (mLog && mOwnLog && log != mLog)
    {
        delete mLog;
        mLog = NULL;
    }

    if (log)
    {
        mLog = log;
        mOwnLog = own;
    }
    else
    {
        mLog = new logging::NullLogger;
        mOwnLog = true;
    }
}

XMLElem XMLParser::newElement(const std::string& name, XMLElem parent) const
{
    return newElement(name, mDefaultURI, parent);
}

XMLElem XMLParser::newElement(const std::string& name,
        const std::string& uri, XMLElem parent)
{
    return newElement(name, uri, "", parent);
}

XMLElem XMLParser::newElement(const std::string& name,
        const std::string& uri, const std::string& characterData,
        XMLElem parent)
{
#ifdef _WIN32
    static const auto encoding = xml::lite::string_encoding::windows_1252;
#else
    static const auto encoding = xml::lite::string_encoding::utf_8;
#endif
    XMLElem elem = new xml::lite::Element(name, uri, characterData, &encoding);
    if (parent)
        parent->addChild(elem);
    return elem;
}

XMLElem XMLParser::createString(const std::string& name,
        const std::string& uri, const std::string& p, XMLElem parent) const
{
    XMLElem const elem = newElement(name, uri, p, parent);
    if (mAddClassAttributes)
    {
        xml::lite::AttributeNode node;
        node.setQName("class");
        node.setUri(getDefaultURI());
        node.setValue("xs:string");
        elem->getAttributes().add(node);
    }

    return elem;
}

XMLElem XMLParser::createString(const std::string& name,
        const std::string& p, XMLElem parent) const
{
    return createString(name, mDefaultURI, p, parent);
}

XMLElem XMLParser::createInt(const std::string& name, const std::string& uri,
        int p, XMLElem parent) const
{
    std::string elementValue;
    try
    {
        elementValue = six::toString<int>(p);
    }
    catch (const except::Exception& ex)
    {
        std::string message("Unable to create " + name + " in element "
                + parent->getLocalName() + ": " + ex.getMessage());
        throw except::Exception(Ctxt(message));
    }
    XMLElem const elem = newElement(name, uri, elementValue, parent);
    if (mAddClassAttributes)
    {
        xml::lite::AttributeNode node;
        node.setQName("class");
        node.setUri(getDefaultURI());
        node.setValue("xs:int");
        elem->getAttributes().add(node);
    }

    return elem;
}

XMLElem XMLParser::createInt(const std::string& name, const std::string& uri,
        const std::string& p, XMLElem parent) const
{
    XMLElem const elem = newElement(name, uri, p, parent);
    if (mAddClassAttributes)
    {
        xml::lite::AttributeNode node;
        node.setQName("class");
        node.setUri(getDefaultURI());
        node.setValue("xs:int");
        elem->getAttributes().add(node);
    }

    return elem;
}

XMLElem XMLParser::createInt(const std::string& name, int p, XMLElem parent) const
{
    return createInt(name, mDefaultURI, p, parent);
}

XMLElem XMLParser::createDouble(const std::string& name,
        const std::string& uri, double p, XMLElem parent) const
{
    std::string elementValue;
    try
    {
        elementValue = six::toString<double>(p);
    }
    catch (const except::Exception& ex)
    {
        std::string message("Unable to create " + name + " in element "
                + parent->getLocalName() + ": " + ex.getMessage());
        throw except::Exception(Ctxt(message));
    }
    XMLElem elem = newElement(name, uri, elementValue, parent);
    if (mAddClassAttributes)
    {
        xml::lite::AttributeNode node;
        node.setQName("class");
        node.setUri(getDefaultURI());
        node.setValue("xs:double");
        elem->getAttributes().add(node);
    }

    return elem;
}

XMLElem XMLParser::createDouble(const std::string& name, double p,
        XMLElem parent) const
{
    return createDouble(name, mDefaultURI, p, parent);
}

XMLElem XMLParser::createBooleanType(const std::string& name,
        const std::string& uri, BooleanType p, XMLElem parent) const
{
    if (p == six::BooleanType::NOT_SET)
    {
        return NULL;
    }

    XMLElem const elem =
            newElement(name, uri, six::toString<BooleanType>(p), parent);
    if (mAddClassAttributes)
    {
        xml::lite::AttributeNode node;
        node.setQName("class");
        node.setUri(getDefaultURI());
        node.setValue("xs:boolean");
        elem->getAttributes().add(node);
    }

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
    if (mAddClassAttributes)
    {
        xml::lite::AttributeNode node;
        node.setQName("class");
        node.setUri(getDefaultURI());
        node.setValue("xs:dateTime");
        elem->getAttributes().add(node);
    }

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
    return createDateTime(name, uri, six::toString<DateTime>(p), parent);
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
    if (mAddClassAttributes)
    {
        xml::lite::AttributeNode node;
        node.setQName("class");
        node.setUri(getDefaultURI());
        node.setValue("xs:date");
        elem->getAttributes().add(node);
    }

    return elem;
}

XMLElem XMLParser::createDate(const std::string& name, const DateTime& p,
        XMLElem parent) const
{
    return createDate(name, mDefaultURI, p, parent);
}

XMLElem XMLParser::getFirstAndOnly(XMLElem parent, const std::string& tag)
{
    std::vector < XMLElem > children;
    parent->getElementsByTagName(tag, children);
    if (children.size() != 1)
    {
        throw except::Exception(Ctxt(
                 "Expected exactly one " + tag + " but got " +
                    str::toString(children.size())));
    }
    return children[0];
}
XMLElem XMLParser::getOptional(XMLElem parent, const std::string& tag)
{
    std::vector < XMLElem > children;
    parent->getElementsByTagName(tag, children);
    if (children.size() != 1)
        return NULL;
    return children[0];
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

void XMLParser::setAttribute(XMLElem e, const std::string& name,
                             const std::string& v, const std::string& uri)
{
    xml::lite::AttributeNode node;
    node.setUri(uri);
    node.setQName(name);
    node.setValue(v);
    e->getAttributes().add(node);
}

void XMLParser::parseDouble(XMLElem element, double& value) const
{
    try
    {
        value = str::toType<double>(element->getCharacterData());
    }
    catch (const except::BadCastException& ex)
    {
        mLog->warn(Ctxt("Unable to parse: " + ex.toString()));
    }
}

void XMLParser::parseComplex(XMLElem element, std::complex<double>& value) const
{
    double r, i;

    parseDouble(getFirstAndOnly(element, "Real"), r);
    parseDouble(getFirstAndOnly(element, "Imag"), i);

    value = std::complex<double>(r, i);
}

void XMLParser::parseString(XMLElem element, std::string& value) const
{
    value = element->getCharacterData();
}

void XMLParser::parseBooleanType(XMLElem element, BooleanType& value) const
{
    try
    {
        value = six::toType<BooleanType>(element->getCharacterData());
    }
    catch (const except::BadCastException& ex)
    {
        mLog->warn(Ctxt("Unable to parse: " + ex.toString()));
    }
}

void XMLParser::parseDateTime(XMLElem element, DateTime& value) const
{
    value = six::toType<DateTime>(element->getCharacterData());
}
}
