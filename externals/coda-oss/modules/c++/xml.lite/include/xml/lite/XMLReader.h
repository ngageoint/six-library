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

#ifndef CODA_OSS_xml_lite_XMLReader_h_INCLUDED_
#define CODA_OSS_xml_lite_XMLReader_h_INCLUDED_
#pragma once

#include "xml/lite/xml_lite_config.h"

#if defined(USE_LIBXML)
#error LibXML is no longer supported.
#endif
#if defined(USE_EXPAT)
#error Expat is no longer supported.
#endif

#if !defined(USE_XERCES)
#error XML parser must be set at configure time
#endif

#  include "xml/lite/XMLReaderXerces.h"

namespace xml
{
namespace lite
{
using  XMLReader = XMLReaderXerces;
}
}

#endif // CODA_OSS_xml_lite_XMLReader_h_INCLUDED_
