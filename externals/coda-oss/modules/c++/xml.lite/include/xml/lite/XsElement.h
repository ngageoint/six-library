/* =========================================================================
 * This file is part of xml.lite-c++ 
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
 *
 * xml.lite-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_xml_lite_XsElement_h_INCLUDED_
#define CODA_OSS_xml_lite_XsElement_h_INCLUDED_

#include "coda_oss/string.h"
#include <ostream>
#include <utility>
#include "coda_oss/optional.h"

#include "str/EncodedStringView.h"

#include "Element.h"
#include "QName.h"

namespace xml
{
namespace lite
{
// Make it easier to manipulate a Xml Element: XSD of `<xs:element name="Foo"
// type="xs:string" />` Can be represented in C++ as
// ```
// struct Foo final
// { XsElement<std::string> foo {"Foo"}; };
// ```
// This is `xs::element` as opposed to `xs:comlexType`, `xs::sequence`,
// `xs:enumeration`, etc.
template <typename T>
class XsElement final
{
    QName name_;
    T value_{};

public:
    using value_type = T;

    explicit XsElement(const QName& name) : name_(name) { }
    explicit XsElement(const std::string& lName) : XsElement(QName(lName)) { }
    XsElement(const QName& name, const value_type& value) : name_(name), value_(value) { }
    XsElement(const std::string& lName, const value_type& value) : XsElement(QName(lName), value) { }
    ~XsElement() = default;
    XsElement(const XsElement&) = default;
    XsElement& operator=(const XsElement&) = default;
    XsElement(XsElement&&) = default;
    XsElement& operator=(XsElement&&) = default;

     const QName& name() const
    {
        return name_;
    }
    const xml::lite::Uri& uri() const
    {
        return name().getUri();
    }
    void setAssociatedUri(const xml::lite::Uri& uri)
    {
        name_.setAssociatedUri(uri);
    }
    const std::string& prefix() const
    {
        return name().getPrefix();
    }
    const std::string& tag() const
    {
        return name().getName();
    }

    const value_type& value() const
    {
        return value_;
    }
    value_type& value()
    {
        return value_;
    }
    // No cast to `T`: this class is intended for XML serialization and
    // shouldn't be widely used elsewhere.

    XsElement& operator=(const value_type& v)
    {
        value_ = v;
        return *this;
    }
};

template <typename T>
inline auto make_XsElement(const std::string& tag)
{
    return XsElement<T>(tag);
}
template <typename T>
inline auto make_XsElement(const std::string& tag, const T& value)
{
    return XsElement<T>(tag, value);
}

template <typename T, typename U = T>
inline bool operator==(const XsElement<T>& lhs, const U& rhs)
{
    return lhs.value() == rhs;
}
template <typename T, typename U = T>
inline bool operator!=(const XsElement<T>& lhs, const U& rhs)
{
    return !(lhs == rhs);
}

template <typename T, typename U = T>
inline bool operator==(const XsElement<T>& lhs, const XsElement<U>& rhs)
{
    return (lhs.tag() == rhs.tag()) && (lhs == rhs.value());
}
template <typename T, typename U = T>
inline bool operator!=(const XsElement<T>& lhs, const XsElement<U>& rhs)
{
    return !(lhs == rhs);
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const XsElement<T>& v)
{
    os << "\t" << v.tag() << "\t: " << v.value();
    return os;
}
template <>
inline std::ostream& operator<<(std::ostream& os, const XsElement<coda_oss::u8string>& v)
{
    os << "\t" << v.tag()
       << "\t: " << str::EncodedStringView(v.value()).native();
    return os;
}

// Make it easier to manipulate an optional Xml Element:
// XSD of `<xs:element name="Foo" type="xs:string" minOccurs="0" />`
// Can be represented in C++ as
// ```
// struct Foo final
// { XsElement_minOccurs0<std::string> foo {"Foo"}; };
// ```
template <typename T>
using XsElement_minOccurs0 = XsElement<coda_oss::optional<T>>;

template <typename T, typename U = T>
inline bool operator==(const XsElement_minOccurs0<T>& lhs, const coda_oss::optional<U>& rhs)
{
    return lhs.value() == rhs;
}
template <typename T, typename U = T>
inline bool operator!=(const XsElement_minOccurs0<T>& lhs, const coda_oss::optional<U>& rhs)
{
    return !(lhs == rhs);
}

template <typename T, typename U = T>
inline bool operator==(const XsElement_minOccurs0<T>& lhs, const XsElement_minOccurs0<U>& rhs)
{
    return (lhs.tag() == rhs.tag()) && (lhs == rhs.value());
}
template <typename T, typename U = T>
inline bool operator!=(const XsElement_minOccurs0<T>& lhs, const XsElement_minOccurs0<U>& rhs)
{
    return !(lhs == rhs);
}

// A few convenience routines; this avoids the need for XsElement_minOccurs0
// to be a seperate class.
template <typename T>
inline bool has_value(const XsElement_minOccurs0<T>& o)
{
    return o.value().has_value();
}
template <typename T>
inline const auto& value(const XsElement_minOccurs0<T>& o)
{
    return o.value().value();
}
template <typename T>
inline auto& value(XsElement_minOccurs0<T>& o)
{
    return o.value().value();
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const XsElement_minOccurs0<T>& o)
{
    if (o.value().has_value())
    {
        os << make_XsElement(o.tag(), *(o.value()));
    }
    else
    {
        os << "\t[ " << o.tag() << "\t: --- ]";
    }
    return os;
}

}
}
#endif // CODA_OSS_xml_lite_XsElement_h_INCLUDED_
