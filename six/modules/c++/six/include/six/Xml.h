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
#ifndef SIX_six_Xml_h_INCLUDED_
#define SIX_six_Xml_h_INCLUDED_
#pragma once

#include <xml/lite/Element.h>
#include <xml/lite/Document.h>
#include <xml/lite/MinidomParser.h>
#include <xml/lite/Validator.h>

namespace six
{
	// Making this "xml" instead of "xml_lite" causes an ambiguous symbol inside of the "six"
	// namesapce if "xml" isn't fully-qualfied (which is usually the case).  Nobody writes
	// "::xml::lite" as "::" is usually redundant.
	//
	// So, "six::xml_lite" instead of "six:xml".
	namespace xml_lite = ::xml::lite;
}

#endif // SIX_six_Xml_h_INCLUDED_
