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
#ifndef CODA_OSS_xml_lite_import_lite_h_INCLUDED_
#define CODA_OSS_xml_lite_import_lite_h_INCLUDED_

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
