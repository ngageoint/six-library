/* =========================================================================
 * This file is part of logging 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * logging-c++ is free software; you can redistribute it and/or modify
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
#ifndef __LOGGING_XML_FORMATTER_H__
#define __LOGGING_XML_FORMATTER_H__

#include <string>
#include <str/Manip.h>
#include "logging/Formatter.h"
#include "logging/LogRecord.h"

namespace logging
{

/*!
 *  \class XMLFormatter
 *  \brief  This class converts the standard log format to XML.  The syntax
 *  for the logger remains the same as the default Formatter, but separates
 *  the LogRecord into separate elements.
 *
 *  c = Log Name
 *  p = Log Level
 *  d = Date/Time
 *  F = File name
 *  L = Line number
 *  M = Function
 *  m = Log message
 *  t = Thread id
 *
 *  The default XML format looks like this:
 *  <Record name="%c" level="%p" date="%d">
 *      <File>%F</FileName>
 *      <Message>%m</Message>
 *  </Record>
 *  
 */
class XMLFormatter : public logging::Formatter
{
public:

    static const char DEFAULT_FORMAT[];

    XMLFormatter();
    XMLFormatter(const std::string& fmt, 
                 const std::string& prologue = "<Log>",
                 const std::string& epilogue = "</Log>");

    virtual void format(const logging::LogRecord* record, io::OutputStream& os) const;

};

}
#endif
