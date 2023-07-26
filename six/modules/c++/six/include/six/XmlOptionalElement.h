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
#ifndef SIX_six_XmlOptionalElement_h_INCLUDED_
#define SIX_six_XmlOptionalElement_h_INCLUDED_

#include <std/string>
#include <std/optional>
#include <ostream>

#include <six/XmlValueElement.h>

namespace six
{
	// Make it easier to manipulate an optional Xml Element
	// ```
	// struct Foo final
	// { XmlOptionalElement<int> myInt {"MyInt"}; };
	// ```
	template<typename T>
	class XmlOptionalElement final
	{
		std::string tag_;
		std::optional<T> value_;

	public:
		explicit XmlOptionalElement(const std::string& tag) : tag_(tag) {}
		XmlOptionalElement(const std::string& tag, const T& value) : tag_(tag), value_(value) {}

		const std::string& tag() const { return tag_; }

		const std::optional<T>& value() const { return value_; }
		std::optional<T>& value() { return value_; }

		bool has_value() const { return value_.has_value(); }
		const T& operator*() const { return *value_; }
		T& operator*() { return *value_; }
		void value(const T& value) { value_ = value; }

		void reset() { value_.reset(); }
	};

	template<typename T, typename U = T>
	inline bool operator==(const XmlOptionalElement<T>& lhs, const U& rhs)
	{
		return lhs.value() == rhs;
	}
	template<typename T, typename U = T>
	inline bool operator!=(const XmlOptionalElement<T>& lhs, const U& rhs)
	{
		return !(lhs == rhs);
	}

	template<typename T, typename U = T>
	inline bool operator==(const XmlOptionalElement<T>& lhs, const XmlOptionalElement<U>& rhs)
	{
		return (lhs.tag() == rhs.tag()) && (lhs.value() == rhs.value());
	}
	template<typename T, typename U = T>
	inline bool operator!=(const XmlOptionalElement<T>& lhs, const XmlOptionalElement<U>& rhs)
	{
		return !(lhs == rhs);
	}

	template<typename T>
	inline std::ostream& operator<<(std::ostream& os, const XmlOptionalElement<T>& o)
	{
		if (o.has_value())
		{
			os << XmlValueElement<T>(o.tag(), *o); // use operator<<() for XmlValueElement
		}
		else
		{
			os << "\t[ " << o.tag() << "\t: --- ]\n";
		}
		return os;
	}

	xml::lite::Element* create(XmlLite&, const XmlOptionalElement<bool>&, xml::lite::Element& parent);
	xml::lite::Element* create(XmlLite&, const XmlOptionalElement<double>&, xml::lite::Element& parent);
	xml::lite::Element* create(XmlLite&, const XmlOptionalElement<std::u8string>&, xml::lite::Element& parent);

	bool parseOptional(const XmlLite&, const xml::lite::Element&, XmlOptionalElement<bool>&);
	bool parseOptional(const XmlLite&, const xml::lite::Element&, XmlOptionalElement<double>&);
	bool parseOptional(const XmlLite&, const xml::lite::Element&, XmlOptionalElement<std::u8string>&);
}

#endif // SIX_six_XmlOptionalElement_h_INCLUDED_
