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

#pragma once
#ifndef CODA_OSS_xml_lite_xerces_h_INCLUDED_
#define CODA_OSS_xml_lite_xerces_h_INCLUDED_

#include "config/compiler_extensions.h"
#include <xml/lite/xml_lite_config.h>

#if defined(USE_XERCES)

CODA_OSS_disable_warning_system_header_push
#if _MSC_VER
#pragma warning(disable: 26493) // Don't use C-style casts (type.4).
#pragma warning(disable: 26494) // Variable '...' is uninitialized. Always initialize an object (type.5).
#pragma warning(disable: 26451) // Arithmetic overflow: Using operator '...' on a 4 byte value and then casting the result to a 8 byte value. Cast the value to the wider type before calling operator '...' to avoid overflow (io.2).
#pragma warning(disable: 26814) // The const variable '...' can be computed at compile-time. Consider using constexpr (con.5).
#pragma warning(disable: 26447) // The function is declared '...' but calls function '...' which may throw exceptions (f.6).
#pragma warning(disable: 26455) // Default constructor should not throw. Declare it '...' (f.6).
#pragma warning(disable: 26440)  // Function '...' can be declared '...' (f.6).
#pragma warning(disable: 26496) // The variable '...' does not change after construction, mark it as const (con.4).
#pragma warning(disable: 26462) // The value pointed to by '...' is assigned only once, mark it as a pointer to const (con.4).
#pragma warning(disable: 26435) // Function '...' should specify exactly one of '...', '...', or '...' (c.128).
#pragma warning(disable: 26433) // Function '...' should be marked with '...' (c.128).
#pragma warning(disable: 26497) // You can attempt to make '...' constexpr unless it contains any undefined behavior (f.4).
#pragma warning(disable: 26433) // Function '...' should be marked with '...' (c.128).
#pragma warning(disable: 26456) // Operator '...' hides a non-virtual operator '...' (c.128).
#pragma warning(disable: 26446) // Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4).
#pragma warning(disable: 26488) // Do not dereference a potentially null pointer: '...'. '...' was null at line 133 (lifetime.1).
#pragma warning(disable : 26477)  // Use '...' rather than 0 or NULL(es .47).
#pragma warning(disable: 26457) // (void) should not be used to ignore return values, use '...' instead (es.48).
#pragma warning(disable: 26475) // Do not use function style casts (es.49). Prefer '...' over '...'..
#else
CODA_OSS_disable_warning(-Wshadow)
CODA_OSS_disable_warning(-Wsuggest-override)
CODA_OSS_disable_warning(-Wzero-as-null-pointer-constant)
CODA_OSS_disable_warning(-Wmisleading-indentation)
CODA_OSS_disable_warning(-Wsign-compare)
CODA_OSS_disable_warning(-Wformat-overflow=)
CODA_OSS_disable_warning(-Wunused-value)
CODA_OSS_disable_warning(-Walloc-size-larger-than=)
#endif

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/impl/DOMLSInputImpl.hpp>

#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/ContentHandler.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>

#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/regx/Token.hpp>

#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/framework/XMLValidator.hpp>
#include <xercesc/framework/XMLGrammarPool.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/parsers/DOMLSParserImpl.hpp>
#include <xercesc/validators/schema/SchemaValidator.hpp>
#include <xercesc/validators/common/ContentSpecNode.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
CODA_OSS_disable_warning_pop

#endif

#endif  // CODA_OSS_xml_lite_xerces_h_INCLUDED_

