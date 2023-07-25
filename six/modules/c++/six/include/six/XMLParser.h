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
#ifndef SIX_six_XMLParser_h_INCLUDED_
#define SIX_six_XMLParser_h_INCLUDED_
#pragma once

#include <assert.h>

#include <string>
#include <type_traits>
#include <std/optional>
#include <memory>

#include <import/gsl.h>

#include <six/Types.h>
#include <six/Init.h>
#include <six/Utilities.h>
#include <xml/lite/Element.h>
#include <six/XmlLite.h>
#include <six/Logger.h>

namespace six
{
struct XMLParser
{
    XMLParser(const std::string& defaultURI, bool addClassAttributes,
        logging::Logger* log = nullptr, bool ownLog = false);
    XMLParser(const std::string& defaultURI, bool addClassAttributes, std::unique_ptr<logging::Logger>&&);
    XMLParser(const std::string& defaultURI, bool addClassAttributes, logging::Logger&);
    XMLParser(const XMLParser&) = delete;
    XMLParser& operator=(const XMLParser&) = delete;
    XMLParser(XMLParser&&) = delete;
    XMLParser& operator=(XMLParser&&) = delete;
    virtual ~XMLParser() = default;

    template<typename TLogger>
    void setLogger(TLogger&& logger)
    {
        mXmlLite.setLogger(std::forward<TLogger>(logger));
    }
    void setLogger(logging::Logger* logger, bool ownLog)
    {
        mXmlLite.setLogger(logger, ownLog);
    }

    typedef xml::lite::Element* XMLElem;

    xml::lite::Element& newElement(const std::string& name, xml::lite::Element& prnt) const;

    template<typename T>
    xml::lite::Element& createString(const std::string& name, const T& t, xml::lite::Element& parent) const
    {
        return mXmlLite.createString(name, t, parent);
    }

    template <typename T>
    xml::lite::Element& createStringFromEnum(const std::string& name, const T& enumVal, xml::lite::Element& parent) const
    {
        return mXmlLite.createStringFromEnum(name, enumVal, parent);
    }

    template<typename T>
    xml::lite::Element& createInt(const std::string& name, T p, xml::lite::Element& parent) const
    {
        return mXmlLite.createInt(name, p, parent);
    }

    xml::lite::Element& createDouble(const std::string& name, double p, xml::lite::Element& parent) const;
    XMLElem createDouble(const std::string& name,
        const std::string& uri, double p = 0, XMLElem parent = nullptr) const;

    static void setAttribute(xml::lite::Element& e, const std::string& name,
        size_t i, const std::string& uri = "")
    {
        return XmlLite::setAttribute(e, xml::lite::QName(xml::lite::Uri(uri), name), i);
    }

    // generic element creation methods, w/URI
    XMLElem createString(const std::string& name,
        const std::string& uri, const std::string& p = "",
        XMLElem parent = nullptr) const;
    XMLElem createString(const std::string& name,
        const std::string& uri, const std::u8string& p,
        XMLElem parent) const;
    XMLElem createString(const std::string& name, const char* p,
        XMLElem parent) const {
        assert(parent != nullptr);
        return &mXmlLite.createString(name, p, *parent);
    }
    template<typename T>
    XMLElem createSixString(const std::string& name,
        const std::string& uri, const T& t,
        XMLElem parent) const
    {
        assert(parent != nullptr);
        return &mXmlLite.createSixString(xml::lite::QName(xml::lite::Uri(uri), name), t, *parent);
    }

    XMLElem createDateTime(const std::string& name, const DateTime& p,
        XMLElem parent) const;

protected:
    logging::Logger* log() const
    {
        return mXmlLite.log();
    }

    //! Returns the default URI
    std::string getDefaultURI() const
    {
        return mXmlLite.getDefaultURI().value;
    }

    XMLElem newElement(const std::string& name, XMLElem prnt = nullptr) const;

    static
    XMLElem newElement(const std::string& name, const std::string& uri,
            XMLElem prnt = nullptr);
    static xml::lite::Element& newElement(const std::string& name, const std::string& uri, xml::lite::Element& prnt);

    static
    XMLElem newElement(const std::string& name, const std::string& uri,
            const std::string& characterData, XMLElem parent = nullptr);
    static XMLElem newElement(const std::string& name, const std::string& uri,
            const std::u8string& characterData, XMLElem parent);

    template<typename T>
    static XMLElem newElement(const T* pElement, const std::string& name, const std::string& uri,
        XMLElem parent = nullptr)
    {
        return XmlLite::newElement(pElement, xml::lite::QName(xml::lite::Uri(uri), name), parent);
    }

    template <typename T>
    XMLElem createStringFromEnum(const std::string& name,
                                 const T& enumVal,
                                 XMLElem parent) const
    {
        return & createStringFromEnum(name, enumVal, *parent);
    }

    XMLElem createInt(const std::string& name, const std::string& uri, const std::string& p, XMLElem parent) const
    {
        assert(parent != nullptr);
        return &mXmlLite.createInt(xml::lite::QName(xml::lite::Uri(uri), name), p, *parent);
    }
    XMLElem createInt(const std::string& name, const std::string& uri, int p, XMLElem parent) const
    {
        assert(parent != nullptr);
        return &mXmlLite.createInt(xml::lite::QName(xml::lite::Uri(uri), name), p, *parent);
    }
    XMLElem createInt(const std::string& name, const std::string& uri, size_t p, XMLElem parent) const
    {
        assert(parent != nullptr);
        return createInt(name, uri, gsl::narrow<int>(p), parent);
    }
    XMLElem createInt(const std::string& name, const std::string& uri, ptrdiff_t p, XMLElem parent) const
    {
        assert(parent != nullptr);
        return createInt(name, uri, gsl::narrow<int>(p), parent);
    }

    XMLElem createDouble(const std::string& name, const std::string& uri, const std::optional<double>& p, XMLElem parent) const;
    XMLElem createOptionalDouble(const std::string& name, const std::string& uri, double p, XMLElem parent) const;
    XMLElem createOptionalDouble(const std::string& name, const std::string& uri, const std::optional<double>& p, XMLElem parent) const;
    XMLElem createBooleanType(const std::string& name, const std::string& uri, BooleanType b, XMLElem parent) const;
    XMLElem createDateTime(const std::string& name, const std::string& uri, const DateTime& p, XMLElem parent) const;
    XMLElem createDate(const std::string& name, const std::string& uri, const DateTime& p, XMLElem parent) const;

    // generic element creation methods, using default URI
    template<typename T>
    XMLElem createString(const std::string& name, const T& t,
        XMLElem parent) const {
        assert(parent != nullptr);
        return & createString(name, t, *parent);
    }
    template<typename T>
    XMLElem createSixString(const std::string& name, const T& t, // six::toString(t) isntead of t.toString()
        XMLElem parent) const {
        assert(parent != nullptr);
        return &mXmlLite.createSixString(name, t, *parent);
    }
    template<typename T>
    XMLElem createInt(const std::string& name, T p = 0,
            XMLElem parent = nullptr) const
    {
        return & createInt(name, p, *parent);
    }

    XMLElem createDouble(const std::string& name, double p = 0, XMLElem parent = nullptr) const;
    XMLElem createDouble(const std::string& name, const std::optional<double>& p, XMLElem parent) const;
    XMLElem createOptionalDouble(const std::string& name, double p, XMLElem parent) const;
    XMLElem createOptionalDouble(const std::string& name, const std::optional<double>& p, XMLElem parent) const;
    XMLElem createBooleanType(const std::string& name, BooleanType b, XMLElem parent) const;
    XMLElem createDate(const std::string& name, const DateTime& p, XMLElem parent) const;

    XMLElem createOptional(const std::string& name, const std::optional<bool>&, xml::lite::Element&) const;

    template <typename T>
    void parseInt(const xml::lite::Element& element, T& value) const
    {
        mXmlLite.parseInt(element, value);
    }
    template <typename T>
    void parseInt(const xml::lite::Element* element, T& value) const
    {
        assert(element != nullptr);
        parseInt(*element, value);
    }

    template <typename T, typename TElement>
    void parseUInt(const TElement& element, T& value) const
    {
        parseInt<T>(element, value);
    }

    template <typename T>
    void parseEnum(const xml::lite::Element& element, T& enumVal) const
    {
        mXmlLite.parseEnum(element, enumVal);
    }
    template <typename T>
    void parseEnum(const xml::lite::Element* element, T& enumVal) const
    {
        assert(element != nullptr);
        parseEnum(*element, enumVal);
    }

    bool parseDouble(const xml::lite::Element* element, double& value) const;
    bool parseDouble(const xml::lite::Element&, double&) const;
    void parseDouble(const xml::lite::Element* element, std::optional<double>& value) const;
    bool parseOptionalDouble(const xml::lite::Element* parent, const std::string& tag, double& value) const;
    bool parseOptionalDouble(const xml::lite::Element* parent, const std::string& tag, std::optional<double>& value) const;
    void parseComplex(const xml::lite::Element* element, six::zdouble& value) const;
    void parseString(const xml::lite::Element* element, std::string& value) const;
    void parseString(const xml::lite::Element&, std::string&) const;
    bool parseString(const xml::lite::Element&, std::u8string&) const;

    void parseBooleanType(const xml::lite::Element* element, BooleanType& value) const
    {
        assert(element != nullptr);
        mXmlLite.parseBooleanType(*element, value);
    }
    bool parseOptional(const xml::lite::Element& parent, const std::string& tag, std::optional<bool>& value) const;

    bool parseOptionalString(const xml::lite::Element& parent, const std::string& tag, std::string& value) const;
    bool parseOptionalString(const xml::lite::Element* parent, const std::string& tag, std::string& value) const
    {
        assert(parent != nullptr);
        return parseOptionalString(*parent, tag, value);
    }
    template <typename T>
    bool parseOptionalInt(const xml::lite::Element* parent, const std::string& tag, T& value) const
    {
        assert(parent != nullptr);
        return mXmlLite.parseOptionalInt(*parent, tag, value);
    }

    void parseDateTime(const xml::lite::Element* element, DateTime& value) const
    {
        assert(element != nullptr);
        mXmlLite.parseDateTime(*element, value);
    }

    static void setAttribute(xml::lite::Element& e, const std::string& name,
        const std::string& s, const xml::lite::Uri& uri)
    {
        return XmlLite::setAttribute(e, xml::lite::QName(uri, name), s);
    }
    static void setAttribute(XMLElem e, const std::string& name,
        const std::string& s, const std::string& uri = "")
    {
        assert(e != nullptr);
        return setAttribute(*e, name, s, xml::lite::Uri(uri));
    }
    static void setAttribute(XMLElem e, const std::string& name,
        size_t i, const std::string& uri = "")
    {
        assert(e != nullptr);
        return setAttribute(*e, name, i, uri);
    }

    static XMLElem getOptional(const xml::lite::Element* parent, const std::string& tag);
    static xml::lite::Element* getOptional(const xml::lite::Element& parent, const std::string& tag);

    template<typename T>
    static XMLElem getOptional_reset(const xml::lite::Element& parent, const std::string& tag, mem::ScopedCopyablePtr<T>& obj)
    {
        return XmlLite::getOptional_reset(parent, tag, obj);
    }
    template<typename T>
    static XMLElem getOptional_reset(const xml::lite::Element* parent, const std::string& tag, mem::ScopedCopyablePtr<T>& obj)
    {
        return getOptional_reset(*parent, tag, obj);
    }

    static XMLElem getFirstAndOnly(const xml::lite::Element* parent, const std::string& tag);
    static xml::lite::Element& getFirstAndOnly(const xml::lite::Element& parent, const std::string& tag);

    void getFirstAndOnly(const xml::lite::Element& parent, const std::string& tag, double&) const;


    /*!
     * Require an element to be not nullptr
     * @throw throws an Exception if the element is nullptr
     * @return returns the input Element
     */
    static XMLElem require(XMLElem element, const std::string& name);

private:
    XmlLite mXmlLite;
};
}

#endif // SIX_six_XMLParser_h_INCLUDED_
