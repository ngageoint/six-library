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

#include <string>
#include <type_traits>
#include <std/optional>
#include <memory>

#include <import/gsl.h>

#include <six/Types.h>
#include <six/Init.h>
#include <six/Utilities.h>
#include <xml/lite/Element.h>
#include <xml/lite/QName.h>
#include <six/Logger.h>

namespace six
{
struct XmlLite final
{
    XmlLite(const xml::lite::Uri& defaultURI, bool addClassAttributes,
        logging::Logger* log = nullptr, bool ownLog = false);
    XmlLite(const xml::lite::Uri& defaultURI, bool addClassAttributes, std::unique_ptr<logging::Logger>&&);
    XmlLite(const xml::lite::Uri& defaultURI, bool addClassAttributes, logging::Logger&);
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
    const xml::lite::Uri& getDefaultURI() const
    {
        return mDefaultURI;
    }

    static xml::lite::Element* newElement(const xml::lite::QName&, xml::lite::Element* prnt);
    static xml::lite::Element& newElement(const xml::lite::QName&, xml::lite::Element& prnt);
    xml::lite::Element* newElement(const std::string& name, xml::lite::Element* prnt) const;
    xml::lite::Element& newElement(const std::string& name, xml::lite::Element& prnt) const;
    static xml::lite::Element* newElement(const xml::lite::QName&, const std::string& characterData, xml::lite::Element* parent);
    static xml::lite::Element& newElement(const xml::lite::QName&, const std::string& characterData, xml::lite::Element& parent);
    static xml::lite::Element* newElement(const xml::lite::QName&, const std::u8string& characterData, xml::lite::Element* parent);
    static xml::lite::Element& newElement(const xml::lite::QName&, const std::u8string& characterData, xml::lite::Element& parent);
    template<typename T>
    static xml::lite::Element* newElement(const T* pElement, const xml::lite::QName& name, xml::lite::Element* parent)
    {
        return pElement == nullptr ? nullptr : newElement(name, parent);
    }

    // generic element creation methods, w/URI
    xml::lite::Element& createString(const xml::lite::QName&, const std::string& p, xml::lite::Element& parent) const;
    #if CODA_OSS_lib_char8_t
    xml::lite::Element& createString(const xml::lite::QName&, const std::u8string& p, xml::lite::Element& parent) const;
    #endif
    template<typename T>
    xml::lite::Element& createSixString(const xml::lite::QName& name, const T& t, xml::lite::Element& parent) const // six::toString(t) isntead of t.toString()
    {
        return createString(name, toString(t), parent);
    }
    xml::lite::Element& createInt(const xml::lite::QName& name, const std::string& p, xml::lite::Element& parent) const;
    xml::lite::Element& createInt(const xml::lite::QName& name, int p, xml::lite::Element& parent) const;
    xml::lite::Element& createDouble(const xml::lite::QName&, double p, xml::lite::Element& parent) const;
    xml::lite::Element& createDouble(const xml::lite::QName&, const std::optional<double>& p, xml::lite::Element& parent) const;
    xml::lite::Element* createOptionalDouble(const xml::lite::QName&, double p, xml::lite::Element& parent) const;
    xml::lite::Element* createOptionalDouble(const xml::lite::QName&, const std::optional<double>& p, xml::lite::Element& parent) const;
    xml::lite::Element* createBooleanType(const xml::lite::QName&, BooleanType b, xml::lite::Element& parent) const;
    xml::lite::Element& createDateTime(const xml::lite::QName&, const DateTime& p, xml::lite::Element& parent) const;
    xml::lite::Element& createDate(const xml::lite::QName&, const DateTime& p, xml::lite::Element& parent) const;

    // generic element creation methods, using default URI
    xml::lite::Element& createString(const std::string& name, const std::string&, xml::lite::Element& parent) const;
    xml::lite::Element& createString(const std::string& name, const char* p, xml::lite::Element& parent) const
    {
        return createString_(name, p, parent);
    }
    template<typename T>
    xml::lite::Element& createString(const std::string& name, const T& t, xml::lite::Element& parent) const
    {
        return createString_(name, t.toString(), parent);
    }
    template<typename T>
    xml::lite::Element& createSixString(const std::string& name, const T& t, // six::toString(t) isntead of t.toString()
        xml::lite::Element& parent) const
    {
        return createString_(name, toString(t), parent);
    }

    template <typename T>
    xml::lite::Element& createStringFromEnum(const std::string& name, const T& enumVal, xml::lite::Element& parent) const
    {
        if (six::Init::isUndefined(enumVal.value))
        {
            throw six::UninitializedValueException(Ctxt("Attempted use of uninitialized value"));
        }

        return createString(name, enumVal.toString(), parent);
    }

    template<typename T>
    xml::lite::Element& createInt(const std::string& name, T p,
            xml::lite::Element& parent) const
    {
        return createInt_(name, gsl::narrow_cast<int>(p), parent);
    }
    xml::lite::Element& createDouble(const std::string& name, double p, xml::lite::Element& parent ) const;
    xml::lite::Element& createDouble(const std::string& name, const std::optional<double>& p, xml::lite::Element& parent) const;
    xml::lite::Element* createOptionalDouble(const std::string& name, double p, xml::lite::Element& parent) const;
    xml::lite::Element* createOptionalDouble(const std::string& name, const std::optional<double>& p, xml::lite::Element& parent) const;
    xml::lite::Element* createBooleanType(const std::string& name, BooleanType b, xml::lite::Element&parent ) const;
    xml::lite::Element& createDateTime(const std::string& name, const DateTime& p, xml::lite::Element& parent) const;
    xml::lite::Element& createDate(const std::string& name, const DateTime& p, xml::lite::Element& parent) const;

    template <typename T>
    void parseInt(const xml::lite::Element& element, T& value) const
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
    void parseEnum(const xml::lite::Element& element, T& enumVal) const
    {
        std::string name;
        parseString(element, name);
        enumVal = T(name);
    }

    bool parseDouble(const xml::lite::Element&, double&) const;
    void parseDouble(const xml::lite::Element& element, std::optional<double>& value) const;
    bool parseOptionalDouble(const xml::lite::Element& parent, const std::string& tag, double& value) const;
    bool parseOptionalDouble(const xml::lite::Element& parent, const std::string& tag, std::optional<double>& value) const;
    void parseComplex(const xml::lite::Element& element, std::complex<double>& value) const;
    void parseString(const xml::lite::Element* element, std::string& value) const;
    void parseString(const xml::lite::Element&, std::string&) const;
    void parseBooleanType(const xml::lite::Element& element, BooleanType& value) const;

    bool parseOptionalString(const xml::lite::Element& parent, const std::string& tag, std::string& value) const;

    template <typename T>
    bool parseOptionalInt(const xml::lite::Element& parent, const std::string& tag, T& value) const
    {
        if (const xml::lite::Element* const element = getOptional(parent, tag))
        {
            parseInt(*element, value);
            return true;
        }
        return false;
    }

    void parseDateTime(const xml::lite::Element& element, DateTime& value) const;

    static void setAttribute(xml::lite::Element& e, const xml::lite::QName& name, size_t i)
    {
        setAttribute(e, name, std::to_string(i));
    }

    static xml::lite::Element* getOptional(const xml::lite::Element& parent, const std::string& tag);

    template<typename T>
    static xml::lite::Element* getOptional_reset(const xml::lite::Element& parent, const std::string& tag, mem::ScopedCopyablePtr<T>& obj)
    {
        auto retval = getOptional(parent, tag);
        if (retval != nullptr)
        {
            //optional
            obj.reset(std::make_unique<T>());
        }
        return retval;
    }

    static xml::lite::Element& getFirstAndOnly(const xml::lite::Element& parent, const std::string& tag);

    /*!
     * Require an element to be not nullptr
     * @throw throws an Exception if the element is nullptr
     * @return returns the input Element
     */
    static xml::lite::Element& require(xml::lite::Element* element, const std::string& name);

    static void setAttribute(xml::lite::Element&, const xml::lite::QName&, const std::string& v);

private:
    xml::lite::Element& createInt_(const std::string& name, int p, xml::lite::Element& parent) const;
    xml::lite::Element& createString_(const std::string& name, const std::string& p, xml::lite::Element& parent) const;

    const xml::lite::Uri mDefaultURI;
    const bool mAddClassAttributes;

    Logger mLogger;
};

 template<> inline xml::lite::Element& XmlLite::createString(const std::string& name,
						      const std::string& p, xml::lite::Element& parent) const
  {
    return createString_(name, p, parent);
  }
}

#endif // SIX_six_XmlLite_h_INCLUDED_
