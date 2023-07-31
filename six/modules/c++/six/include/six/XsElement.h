/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
 *
 *  six-c++ is free software; you can redistribute it and/or modify
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
#ifndef SIX_six_XsElement_h_INCLUDED_
#define SIX_six_XsElement_h_INCLUDED_

#include <std/string>
#include <ostream>
#include <utility>
#include <std/optional>

#include <str/EncodedStringView.h>
#include <xml/lite/Element.h>
#include <xml/lite/QName.h>

namespace six
{

// Make it easier to manipulate a Xml Element: XSD of `<xs:element name="Foo"
// type="xs:string" />` Can be represented in C++ as
// ```
// struct Foo final
// { XsElement<std::string> foo {"Foo"}; };
// ```
// This is `xs::element` as opposed to `xs:comlexType`, `xs::sequence`, `xs:enumeration`, etc.
template <typename T>
class XsElement final
{
    xml::lite::QName name_;
    T value_{};

public:
    using value_type = T;

    explicit XsElement(const xml::lite::QName& name) : name_(name) { }
    explicit XsElement(const std::string& lName) : XsElement(xml::lite::QName(lName)) { }
    XsElement(const xml::lite::QName& name, const value_type& value) : name_(name), value_(value) { }
    XsElement(const std::string& lName, const value_type& value) : XsElement(xml::lite::QName(lName), value) { }
    ~XsElement() = default;
    XsElement(const XsElement&) = default;
    XsElement& operator=(const XsElement&) = default;
    XsElement(XsElement&&) = default;
    XsElement& operator=(XsElement&&) = default;

    const auto& name() const
    {
        return name_;
    }
    XsElement& operator=(xml::lite::QName name)
    {
        name_ = std::move(name);
        return *this;
    }
    void setAssociatedUri(const xml::lite::Uri& uri)
    {
        name_.setAssociatedUri(uri);
    }
    std::string tag() const
    {
        return name_.getName();
    }

    const value_type& value() const
    {
        return value_;
    }
    value_type& value()
    {
        return value_;
    }
    operator const value_type& () const
    {
        return value();
    }
    operator value_type& ()
    {
        return value();
    }
    XsElement& operator=(const value_type& v)
    {
        value_ = v;
        return *this;
    }
    XsElement& operator=(value_type&& v)
    {
        value_ = std::move(v);
        return *this;
    }
};

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
inline const auto& value(const XsElement<T>& o)
{
    return o.value();
}
template <typename T>
inline auto& value(XsElement<T>& o)
{
    return o.value();
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const XsElement<T>& v)
{
    os << "\t" << v.tag() << "\t: " << v.value();
    return os;
}
template <>
inline std::ostream& operator<<(std::ostream& os, const XsElement<std::u8string>& v)
{
    os << "\t" << v.tag() << "\t: " << str::EncodedStringView(v.value()).native();
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
using XsElement_minOccurs0 = XsElement<std::optional<T>>;

template <typename T, typename U = T>
inline bool operator==(const XsElement_minOccurs0<T>& lhs, const std::optional<U>& rhs)
{
    return lhs.value() == rhs;
}
template <typename T, typename U = T>
inline bool operator!=(const XsElement_minOccurs0<T>& lhs, const std::optional<U>& rhs)
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
        os << XsElement<T>(o.name(), value(o));
    }
    else
    {
        os << "\t[ " << o.tag() << "\t: --- ]";
    }
    return os;
}

// Existing code uses mem::ScopedCopyablePtr rather than std::optional
template<typename T>
inline auto get(const XsElement_minOccurs0<T>& o)
{
    return has_value(o) ? &(value(o)) : nullptr;
}
template<typename T>
inline auto get(XsElement_minOccurs0<T>& o)
{
    return has_value(o) ? &(value(o)) : nullptr;
}

}
#endif // SIX_six_XsElement_h_INCLUDED_
