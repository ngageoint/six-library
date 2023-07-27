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
#include <utility>

#include <xml/lite/XsElement.h>

namespace six
{
	struct XmlLite; // forward

	// Make it easier to manipulate a Xml Element: XSD of `<xs:element name="Foo" type="xs:string" />`
	// Can be represented in C++ as
	// ```
	// struct Foo final
	// { XsElement<std::string> foo {"Foo"}; };
	// ```
	// This is `xs::element` as opposed to `xs:comlexType`, `xs::sequence`, `xs:enumeration`, etc.
	template<typename T>
	using XsElement = xml::lite::XsElement<T>;

	xml::lite::Element& create(const XmlLite&, const XsElement<double>&, xml::lite::Element& parent);
	void getFirstAndOnly(const XmlLite&, const xml::lite::Element&, XsElement<double>&);


	// Make it easier to manipulate an optional Xml Element:
	// XSD of `<xs:element name="Foo" type="xs:string" minOccurs="0" />`
	// Can be represented in C++ as
	// ```
	// struct Foo final
	// { XsElement_minOccurs0<std::string> foo {"Foo"}; };
	// ```
	template<typename T>
	using XsElement_minOccurs0 = xml::lite::XsElement_minOccurs0<T>;

	xml::lite::Element* create(const XmlLite&, const XsElement_minOccurs0<bool>&, xml::lite::Element& parent);
	xml::lite::Element* create(const XmlLite&, const XsElement_minOccurs0<double>&, xml::lite::Element& parent);
	xml::lite::Element* create(const XmlLite&, const XsElement_minOccurs0<std::u8string>&, xml::lite::Element& parent);

	bool parseOptional(const XmlLite&, const xml::lite::Element&, XsElement_minOccurs0<bool>&);
	bool parseOptional(const XmlLite&, const xml::lite::Element&, XsElement_minOccurs0<double>&);
	bool parseOptional(const XmlLite&, const xml::lite::Element&, XsElement_minOccurs0<std::u8string>&);

}


#endif // SIX_six_XsElement_h_INCLUDED_
