/* =========================================================================
 * This file is part of xml.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifndef CODA_OSS_xml_lite_import_lite_h_INCLUDED_
#define CODA_OSS_xml_lite_import_lite_h_INCLUDED_
#pragma once

#if _MSC_VER
#pragma warning(push)
// these are from Xerces
#pragma warning(disable: 5219) // implicit conversion from '...' to '...', possible loss of data
#pragma warning(disable: 4365) // '...': conversion from '...' to '...', signed / unsigned mismatch
#pragma warning(disable: 26477) // Use 'nullptr' rather than 0 or NULL (es.47).
#pragma warning(disable: 26493) // Don't use C-style casts (type.4).
#pragma warning(disable: 26814) // The const variable '...' can be computed at compile-time. Consider using constexpr (con.5).
#pragma warning(disable: 26496) // The variable '...' does not change after construction, mark it as const (con.4).
#pragma warning(disable: 26497) // The function '...' could be marked constexpr if compile-time evaluation is desired (f.4).
#pragma warning(disable: 26475) // Do not use function style casts (es.49). Prefer '...' over '...'.
#pragma warning(disable: 26495) // Variable '...' is uninitialized. Always initialize a member variable (type.6).
#pragma warning(disable: 26461) // The pointer argument '...' for function '...' can be marked as a pointer to const (con.3).
#pragma warning(disable: 26462) // The value pointed to by '...' is assigned only once, mark it as a pointer to const (con.4).
#pragma warning(disable: 26494) // Variable '...' is uninitialized. Always initialize an object (type.5).
#pragma warning(disable: 26489) // Don't dereference a pointer that may be invalid: '...'. '...' may have been invalidated at line ... (lifetime.1).
#pragma warning(disable: 26488) // Do not dereference a potentially null pointer : ...'. '...' was null at line ... (lifetime.1).
#pragma warning(disable: 26447) // The function is declared '...' but calls function '..' which may throw exceptions (f.6).
#pragma warning(disable: 26485) // Expression '...': No array to pointer decay (bounds.3).
#pragma warning(disable: 26457) // (void) should not be used to ignore return values, use '...' instead (es.48).
#pragma warning(disable: 26487) // Don't return a pointer '...' that may be invalid (lifetime.4).
#pragma warning(disable: 26451) // Arithmetic overflow : Using operator '...' on a 4 byte value and then casting the result to a 8 byte value. Cast the value to the wider type before calling operator '...' to avoid overflow (io.2).
#pragma warning(disable: 26455) //	 Default constructor may not throw.Declare it 'noexcept' (f.6).
#pragma warning(disable: 26409) // Avoid calling new and delete explicitly, use std::make_unique<T> instead (r.11).
#pragma warning(disable: 26429) // Symbol '...' is never tested for nullness, it can be marked as not_null(f.23).
#pragma warning(disable: 26400) // Do not assign the result of an allocation or a function call with an owner<T> return value to a raw pointer, use owner<T> instead(i.11).
#pragma warning(disable: 26467) // Converting from floating point to unsigned integral types results in non-portable code if the double/float has a negative value. Use gsl::narrow_cast or gsl::narrow instead to guard against undefined behavior and potential data loss (es.46).
#pragma warning(disable: 26823) // Dereferencing a possibly null pointer '...' (lifetime.1).
#endif // _MSC_VER

#include "xml/lite/ContentHandler.h"
#include "xml/lite/Attributes.h"
#include "xml/lite/QName.h"
#include "xml/lite/NamespaceStack.h"
#include "xml/lite/Document.h"
#include "xml/lite/Element.h"
#include "xml/lite/XMLException.h"
#include "xml/lite/XMLReader.h"
#include "xml/lite/MinidomHandler.h"
#include "xml/lite/MinidomParser.h"
#include "xml/lite/Serializable.h"
#include "xml/lite/Validator.h"

#if _MSC_VER
#pragma warning(pop)

#pragma comment(lib, "xml.lite-c++.lib")

#if defined(USE_XERCES)
#pragma comment(lib, "xerces-c")
#endif
#endif

/*!
 *  \file lite.h
 *
 *  The xml.lite is a simple lightweight API for XML programming.
 *  It is based on the PHP, Perl and Python implementations.  The
 *  idea behind it is that it avoids much of the bloat of W3C specifications,
 *  particularly the notorious DOM specifications, leaving the practical
 *  API's and discarding the overly-complex or confusing ones.
 *
 *  Getting started with xml.lite is fairly straight-forward, with the
 *  assistance of the <a href="../GettingStarted.html">quickstart article</a>
 *
 */

#endif  // CODA_OSS_xml_lite_import_lite_h_INCLUDED_
