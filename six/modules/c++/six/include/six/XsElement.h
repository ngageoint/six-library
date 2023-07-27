/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
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
#ifndef SIX_six_XsElement_h_INCLUDED_
#define SIX_six_XsElement_h_INCLUDED_

#include <std/string>
#include <ostream>

#include <str/EncodedStringView.h>
#include <xml/lite/Element.h>
#include <xml/lite/QName.h>

#include <six/Types.h>
#include <six/Init.h>
#include <six/Utilities.h>

namespace six
{
	struct XmlLite; // forward

	// Make it easier to manipulate a Xml Element:
	// XSD of `<xs:element name="Foo" type="xs:string" />`
	// Can be represented in C++ as
	// ```
	// struct Foo final
	// { XsElement<std::string> foo {"Foo"}; };
	// ```
	template<typename T>
	class XsElement final
	{
		std::string tag_;
		T value_{};

	public:
		explicit XsElement(const std::string& tag) : tag_(tag) {}
		XsElement(const std::string& tag, const T& value) : tag_(tag), value_(value) {}
		~XsElement() = default;
		XsElement(const XsElement&) = default;
		XsElement& operator=(const XsElement&) = default;
		XsElement(XsElement&&) = default;
		XsElement& operator=(XsElement&&) = default;

		const std::string& tag() const { return tag_; }

		const T& value() const { return value_; }
		XsElement& operator=(const T& v) {
			value_ = v;
			return *this;
		}
	};

	template<typename T, typename U = T>
	inline bool operator==(const XsElement<T>& lhs, const U& rhs)
	{
		return lhs.value() == rhs;
	}
	template<typename T, typename U = T>
	inline bool operator!=(const XsElement<T>& lhs, const U& rhs)
	{
		return !(lhs == rhs);
	}

	template<typename T, typename U = T>
	inline bool operator==(const XsElement<T>& lhs, const XsElement<U>& rhs)
	{
		return (lhs.tag() == rhs.tag()) && (lhs == rhs.value());
	}
	template<typename T, typename U = T>
	inline bool operator!=(const XsElement<T>& lhs, const XsElement<U>& rhs)
	{
		return !(lhs == rhs);
	}

	template<typename T>
	inline std::ostream& operator<<(std::ostream& os, const XsElement<T>& v)
	{
		os << "\t" << v.tag() << "\t: " << v.value();
		return os;
	}
	template<>
	inline std::ostream& operator<<(std::ostream& os, const XsElement<std::u8string>& v)
	{
		os << "\t" << v.tag() << "\t: " << str::EncodedStringView(v.value()).native();
		return os;
	}

	xml::lite::Element& create(XmlLite&, const XsElement<double>&, xml::lite::Element& parent);
	void getFirstAndOnly(const XmlLite&, const xml::lite::Element&, XsElement<double>&);
}

#endif // SIX_six_XsElement_h_INCLUDED_
