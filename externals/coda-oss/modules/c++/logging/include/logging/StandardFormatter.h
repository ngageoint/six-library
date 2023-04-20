/* =========================================================================
 * This file is part of logging-c++ 
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

///////////////////////////////////////////////////////////
//  StandardFormatter.h
///////////////////////////////////////////////////////////

#ifndef __LOGGING_STANDARD_FORMATTER_H__
#define __LOGGING_STANDARD_FORMATTER_H__

#include <string>
#include <str/Manip.h>
#include "config/Exports.h"
#include "logging/Formatter.h"
#include "logging/LogRecord.h"

namespace logging
{

/*!
 *  \class StandardFormatter
 *  \brief  This class provides default formatting capabilities.  The syntax
 *  for the format string maps to that which is used in log4j.
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
 *  The default format looks like this:
 *  [%c] %p %d ==> %m
 */
class CODA_OSS_API StandardFormatter : public Formatter
{
public:
    static const char DEFAULT_FORMAT[];

    StandardFormatter() : Formatter(DEFAULT_FORMAT) {}
    StandardFormatter(const std::string& fmt, 
                      const std::string& prologue = "",
                      const std::string& epilogue = "");

    virtual ~StandardFormatter() {}

    StandardFormatter& operator=(const StandardFormatter&) = delete;

    virtual void format(const LogRecord* record, io::OutputStream& os) const override;

};

}
#endif
