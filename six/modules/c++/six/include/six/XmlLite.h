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
#ifndef SIX_six_XmlLite_h_INCLUDED_
#define SIX_six_XmlLite_h_INCLUDED_
#pragma once

#include <assert.h>

#include <std/string>
#include <type_traits>
#include <std/optional>
#include <memory>

#include <import/gsl.h>
#include <xml/lite/Element.h>
#include <xml/lite/QName.h>

#include <six/Types.h>
#include <six/Init.h>
#include <six/Utilities.h>
#include <six/Logger.h>
#include <six/Xml.h>

namespace six
{
// A simple wrapper around xml::lite::MinidomParser
struct MinidomParser final
{
    MinidomParser();
    ~MinidomParser();
    MinidomParser(const MinidomParser&) = delete;
    MinidomParser& operator=(const MinidomParser&) = delete;
    MinidomParser(MinidomParser&&) = default;
    MinidomParser& operator=(MinidomParser&&) = default;

     /*!
     *  Present our parsing interface.  Similar to DOM, the input
     *  is an InputStream (DOM's is called input source), but it
     *  is even more flexible, and works within the XPC io paradigm.
     *  \param is  This is the input stream to feed the parser
     *  \param size  This is the size of the stream to feed the parser
     */
    void parse(io::InputStream& is, int size = io::InputStream::IS_END);

    /*!
     *  This clears the MinidomHandler, killing its underlying Document
     *  tree.  The Document node is preserved, however -- it must
     *  be explicitly reset to another document to change element type.
     */
    void clear();

    /*!
        *  Return a pointer to the document.  Note that its a reference
        *  so you dont get to keep it.
        *  \return Pointer to document.
        */
    xml::lite::Document* getDocument() const;
    xml::lite::Document* getDocument(bool steal = false);
    void getDocument(std::unique_ptr<xml::lite::Document>&); // steal = true

    /*!
        *  This is the public interface for resetting the
        *  XML document.  This will call the handler version of this
        *  function, which will delete the old document.
        *
        *  \param newDocument The new document.
        */
    void setDocument(xml::lite::Document* newDocument, bool own = true);
    void setDocument(std::unique_ptr< xml::lite::Document>&&);  // own = true

    /*!
        * @see MinidomHandler::preserveCharacterData
        */
    void preserveCharacterData(bool preserve);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
inline xml::lite::Document& getDocument(MinidomParser& xmlParser)
{
    auto retval = xmlParser.getDocument(false /*steal*/);
    assert(retval != nullptr);
    return *retval;
}


struct XmlLite final
{
    XmlLite(const xml_lite::Uri& defaultURI, bool addClassAttributes,
        logging::Logger* log = nullptr, bool ownLog = false);
    XmlLite(const xml_lite::Uri& defaultURI, bool addClassAttributes, std::unique_ptr<logging::Logger>&&);
    XmlLite(const xml_lite::Uri& defaultURI, bool addClassAttributes, logging::Logger&);
    XmlLite(const XmlLite&) = delete;
    XmlLite& operator=(const XmlLite&) = delete;
    XmlLite(XmlLite&&) = delete;
    XmlLite& operator=(XmlLite&&) = delete;
    ~XmlLite() = default;

    template<typename TLogger>
    void setLogger(TLogger&& logger)
    {
        mLogger.setLogger(std::forward<TLogger>(logger));
    }
    void setLogger(logging::Logger* logger, bool ownLog)
    {
        mLogger.setLogger(logger, ownLog);
    }

    logging::Logger* log() const
    {
        return mLogger.get(std::nothrow);
    }

    //! Returns the default URI
    const xml_lite::Uri& getDefaultURI() const
    {
        return mDefaultURI;
    }

    static xml_lite::Element* newElement(const xml_lite::QName&, xml_lite::Element* prnt);
    static xml_lite::Element& newElement(const xml_lite::QName&, xml_lite::Element& prnt);
    xml_lite::Element* newElement(const std::string& name, xml_lite::Element* prnt) const;
    xml_lite::Element& newElement(const std::string& name, xml_lite::Element& prnt) const;
    static xml_lite::Element* newElement(const xml_lite::QName&, const std::string& characterData, xml_lite::Element* parent);
    static xml_lite::Element& newElement(const xml_lite::QName&, const std::string& characterData, xml_lite::Element& parent);
    static xml_lite::Element* newElement(const xml_lite::QName&, const std::u8string& characterData, xml_lite::Element* parent);
    static xml_lite::Element& newElement(const xml_lite::QName&, const std::u8string& characterData, xml_lite::Element& parent);
    template<typename T>
    static xml_lite::Element* newElement(const T* pElement, const xml_lite::QName& name, xml_lite::Element* parent)
    {
        return pElement == nullptr ? nullptr : newElement(name, parent);
    }

    template<typename T>
    static std::string six_toString(const T& t)
    {
        return ::six::toString(t);
    }

    // generic element creation methods, w/URI
    xml_lite::Element& createString(const xml_lite::QName&, const std::string& p, xml_lite::Element& parent) const;
    xml_lite::Element& createString(const xml_lite::QName&, const std::u8string& p, xml_lite::Element& parent) const;
    template<typename T>
    xml_lite::Element& createSixString(const xml_lite::QName& name, const T& t, xml_lite::Element& parent) const // six::toString(t) isntead of t.toString()
    {
        return createString(name, six_toString(t), parent);
    }
    xml_lite::Element& createInt(const xml_lite::QName& name, const std::string& p, xml_lite::Element& parent) const;
    xml_lite::Element& createInt(const xml_lite::QName& name, int p, xml_lite::Element& parent) const;
    xml_lite::Element& createDouble(const xml_lite::QName&, double p, xml_lite::Element& parent) const;
    xml_lite::Element& createDouble(const xml_lite::QName&, const std::optional<double>& p, xml_lite::Element& parent) const;
    xml_lite::Element* createOptionalDouble(const xml_lite::QName&, double p, xml_lite::Element& parent) const;
    xml_lite::Element* createOptionalDouble(const xml_lite::QName&, const std::optional<double>& p, xml_lite::Element& parent) const;
    xml_lite::Element* createBooleanType(const xml_lite::QName&, BooleanType b, xml_lite::Element& parent) const;
    xml_lite::Element& createDateTime(const xml_lite::QName&, const DateTime& p, xml_lite::Element& parent) const;
    xml_lite::Element& createDate(const xml_lite::QName&, const DateTime& p, xml_lite::Element& parent) const;

    // generic element creation methods, using default URI
    xml_lite::Element& createString(const std::string& name, const std::string&, xml_lite::Element& parent) const;
    xml_lite::Element& createString(const std::string& name, const char* p, xml_lite::Element& parent) const
    {
        return createString_(name, p, parent);
    }
    template<typename T>
    xml_lite::Element& createString(const std::string& name, const T& t, xml_lite::Element& parent) const
    {
        return createString_(name, t.toString(), parent);
    }
    template<typename T>
    xml_lite::Element& createSixString(const std::string& name, const T& t, // six::toString(t) isntead of t.toString()
        xml_lite::Element& parent) const
    {
        return createString_(name, six_toString(t), parent);
    }
    template <typename T>
    xml_lite::Element& createStringFromEnum(const std::string& name, const T& enumVal, xml_lite::Element& parent) const
    {
        if (six::Init::isUndefined(enumVal.value))
        {
            throw six::UninitializedValueException(Ctxt("Attempted use of uninitialized value"));
        }

        return createString(name, enumVal.toString(), parent);
    }
    template<typename T>
    xml_lite::Element& createInt(const std::string& name, T p,
            xml_lite::Element& parent) const
    {
        return createInt_(name, gsl::narrow_cast<int>(p), parent);
    }
    xml_lite::Element& createDouble(const std::string& name, double p, xml_lite::Element& parent ) const;
    xml_lite::Element& createDouble(const std::string& name, const std::optional<double>& p, xml_lite::Element& parent) const;
    xml_lite::Element* createOptionalDouble(const std::string& name, double p, xml_lite::Element& parent) const;
    xml_lite::Element* createOptionalDouble(const std::string& name, const std::optional<double>& p, xml_lite::Element& parent) const;
    xml_lite::Element* createBooleanType(const std::string& name, BooleanType b, xml_lite::Element&parent ) const;
    xml_lite::Element& createDateTime(const std::string& name, const DateTime& p, xml_lite::Element& parent) const;
    xml_lite::Element& createDate(const std::string& name, const DateTime& p, xml_lite::Element& parent) const;

    template <typename T>
    void parseInt(const xml_lite::Element& element, T& value) const
    {
        try
        {
            value = str::toType<T>(element.getCharacterData());
        }
        catch (const except::BadCastException& ex)
        {
            log()->warn(Ctxt("Unable to parse: " + ex.toString()));
        }
    }

    template <typename T>
    void parseEnum(const xml_lite::Element& element, T& enumVal) const
    {
        std::string name;
        parseString(element, name);
        enumVal = T(name);
    }

    bool parseDouble(const xml_lite::Element&, double&) const;
    void parseDouble(const xml_lite::Element& element, std::optional<double>& value) const;
    bool parseOptionalDouble(const xml_lite::Element& parent, const std::string& tag, double& value) const;
    bool parseOptionalDouble(const xml_lite::Element& parent, const std::string& tag, std::optional<double>& value) const;
    void parseComplex(const xml_lite::Element& element, std::complex<double>& value) const;
    void parseString(const xml_lite::Element* element, std::string& value) const;
    void parseString(const xml_lite::Element&, std::string&) const;
    void parseBooleanType(const xml_lite::Element& element, BooleanType& value) const;

    bool parseOptionalString(const xml_lite::Element& parent, const std::string& tag, std::string& value) const;

    template <typename T>
    bool parseOptionalInt(const xml_lite::Element& parent, const std::string& tag, T& value) const
    {
        if (const xml_lite::Element* const element = getOptional(parent, tag))
        {
            parseInt(*element, value);
            return true;
        }
        return false;
    }

    void parseDateTime(const xml_lite::Element& element, DateTime& value) const;

    static void setAttribute(xml_lite::Element& e, const xml_lite::QName& name, size_t i)
    {
        setAttribute(e, name, std::to_string(i));
    }

    static xml_lite::Element* getOptional(const xml_lite::Element& parent, const std::string& tag);

    template<typename T>
    static xml_lite::Element* getOptional_reset(const xml_lite::Element& parent, const std::string& tag, mem::ScopedCopyablePtr<T>& obj)
    {
        auto retval = getOptional(parent, tag);
        if (retval != nullptr)
        {
            //optional
            obj.reset(std::make_unique<T>());
        }
        return retval;
    }

    static xml_lite::Element& getFirstAndOnly(const xml_lite::Element& parent, const std::string& tag);

    /*!
     * Require an element to be not nullptr
     * @throw throws an Exception if the element is nullptr
     * @return returns the input Element
     */
    static xml_lite::Element& require(xml_lite::Element* element, const std::string& name);

    static void setAttribute(xml_lite::Element&, const xml_lite::QName&, const std::string& v);

private:
    xml_lite::Element& createInt_(const std::string& name, int p, xml_lite::Element& parent) const;
    xml_lite::Element& createString_(const std::string& name, const std::string& p, xml_lite::Element& parent) const;
    xml_lite::QName makeQName(const std::string& name) const;

    const xml_lite::Uri mDefaultURI;
    const bool mAddClassAttributes;

    Logger mLogger;
};

 template<> inline xml_lite::Element& XmlLite::createString(const std::string& name,
						      const std::string& p, xml_lite::Element& parent) const
  {
    return createString_(name, p, parent);
  }
}

#endif // SIX_six_XmlLite_h_INCLUDED_
