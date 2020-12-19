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
#pragma once

#include <string>

#include <xml/lite/Element.h>
#include <logging/Logger.h>
#include <six/Types.h>
#include <six/Init.h>
#include <six/Utilities.h>

namespace six
{
class XMLParser
{
public:
    //!  Constructor
    XMLParser(const std::string& defaultURI,
              bool addClassAttributes,
              logging::Logger* log = nullptr,
              bool ownLog = false);

    //!  Destructor
    virtual ~XMLParser();

    void setLogger(logging::Logger* log, bool ownLog = false);

    typedef xml::lite::Element* XMLElem;

protected:
    logging::Logger* log() const
    {
        return mLog;
    }

    //! Returns the default URI
    std::string getDefaultURI() const
    {
        return mDefaultURI;
    }

    XMLElem newElement(const std::string& name, XMLElem prnt = nullptr) const;

    static
    XMLElem newElement(const std::string& name, const std::string& uri,
            XMLElem prnt = nullptr);

    static
    XMLElem newElement(const std::string& name, const std::string& uri,
            const std::string& characterData, XMLElem parent = nullptr);

    // generic element creation methods, w/URI
    XMLElem createString(const std::string& name,
            const std::string& uri, const std::string& p = "",
            XMLElem parent = nullptr) const;

    template <typename T>
    XMLElem createStringFromEnum(const std::string& name,
                                 const T& enumVal,
                                 XMLElem parent) const
    {
        if (six::Init::isUndefined(enumVal.value))
        {
            throw six::UninitializedValueException(
                Ctxt("Attempted use of uninitialized value"));
        }

        return createString(name,
                            enumVal.toString(),
                            parent);
    }

    XMLElem createInt(const std::string& name, const std::string& uri,
           int p = 0, XMLElem parent = nullptr) const;

    XMLElem createInt(const std::string& name,
            const std::string& uri, const std::string& p = "",
            XMLElem parent = nullptr) const;

    XMLElem createDouble(const std::string& name,
            const std::string& uri, double p = 0, XMLElem parent = nullptr) const;

    XMLElem createBooleanType(const std::string& name,
           const std::string& uri, BooleanType b, XMLElem parent = nullptr) const;

    XMLElem createDateTime(const std::string& name,
            const std::string& uri, const DateTime& p, XMLElem parent = nullptr) const;

    XMLElem createDateTime(const std::string& name,
            const std::string& uri, const std::string& s,
            XMLElem parent = nullptr) const;

    XMLElem createDate(const std::string& name,
            const std::string& uri, const DateTime& p, XMLElem parent = nullptr) const;

    // generic element creation methods, using default URI
    template<typename T>
    XMLElem createString(const std::string& name, const T& t,
            XMLElem parent = nullptr) const {
        return createString_(name, t.toString(), parent);
    }
    XMLElem createString(const std::string& name, const char* p="",
        XMLElem parent = nullptr) const {
        return createString_(name, p, parent);
    }
    XMLElem createInt(const std::string& name, int p = 0,
            XMLElem parent = nullptr) const;
    XMLElem createDouble(const std::string& name, double p = 0,
            XMLElem parent = nullptr) const;
    XMLElem createBooleanType(const std::string& name, BooleanType b,
            XMLElem parent = nullptr) const;
    XMLElem createDateTime(const std::string& name, const DateTime& p,
            XMLElem parent = nullptr) const;
    XMLElem createDateTime(const std::string& name,
            const std::string& s, XMLElem parent = nullptr) const;
    XMLElem createDate(const std::string& name, const DateTime& p,
            XMLElem parent = nullptr) const;

    template <typename T>
    void parseInt(XMLElem element, T& value) const
    {
        try
        {
            value = str::toType<T>(element->getCharacterData());
        }
        catch (const except::BadCastException& ex)
        {
            mLog->warn(Ctxt("Unable to parse: " + ex.toString()));
        }
    }

    template <typename T>
    void parseUInt(XMLElem element, T& value) const
    {
        parseInt<T>(element, value);
    }

    template <typename T>
    void parseEnum(XMLElem element, T& enumVal) const
    {
        std::string name;
        parseString(element, name);
        enumVal = T(name);
    }

    void parseDouble(XMLElem element, double& value) const;
    void parseComplex(XMLElem element, std::complex<double>& value) const;
    void parseString(XMLElem element, std::string& value) const;
    void parseBooleanType(XMLElem element, BooleanType& value) const;

    void parseDateTime(XMLElem element, DateTime& value) const;

    template<typename T>
    static void setAttribute(XMLElem e, const std::string& name,
        const T& t, const std::string& uri = "")
    {
        setAttribute_(e, name, toString(t), uri);
    }

    static XMLElem getOptional(XMLElem parent, const std::string& tag);
    static XMLElem getFirstAndOnly(XMLElem parent, const std::string& tag);

    /*!
     * Require an element to be not nullptr
     * @throw throws an Exception if the element is nullptr
     * @return returns the input Element
     */
    static XMLElem require(XMLElem element, const std::string& name);

private:
    XMLElem createString_(const std::string& name,
        const std::string& p, XMLElem parent) const;
    static void setAttribute_(XMLElem e, const std::string& name,
            const std::string& v, const std::string& uri);

    const std::string mDefaultURI;
    const bool mAddClassAttributes;

    logging::Logger* mLog;
    bool mOwnLog;
};

 template<> inline XMLParser::XMLElem XMLParser::createString(const std::string& name,
						      const std::string& p, XMLElem parent) const
  {
    return createString_(name, p, parent);
  }

 template<> inline void XMLParser::setAttribute(XMLElem e, const std::string& name,
     const std::string& p, const std::string& uri)
 {
     setAttribute_(e, name, p, uri);
 }
}


