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

#pragma once
#ifndef __LOGGING_ENUMS_H__
#define __LOGGING_ENUMS_H__

#include <import/except.h>
#include <import/str.h>
#include <import/sys.h>

// -----------------------------------------------------------------------------
// This file is auto-generated - please do NOT edit directly
// -----------------------------------------------------------------------------

namespace logging
{


/*!
 *  \struct LogLevel 
 *
 *  Enumeration used to represent LogLevels
 */
struct LogLevel final
{
    //! The enumerations allowed
    enum
    {
        LOG_NOTSET = 0,
        LOG_DEBUG = 1,
        LOG_INFO = 2,
        LOG_WARNING = 3,
        LOG_ERROR = 4,
        LOG_CRITICAL = 5
    };

    //! Default constructor
    LogLevel(){ value = LOG_NOTSET; }

    //! string constructor
    LogLevel(std::string s)
    {
        if (s == "LOG_NOTSET")
            value = LOG_NOTSET;
        else if (s == "NOTSET")
            value = LOG_NOTSET;
        else if (s == "LOG_DEBUG")
            value = LOG_DEBUG;
        else if (s == "DEBUG")
            value = LOG_DEBUG;
        else if (s == "LOG_INFO")
            value = LOG_INFO;
        else if (s == "INFO")
            value = LOG_INFO;
        else if (s == "LOG_WARNING")
            value = LOG_WARNING;
        else if (s == "WARNING")
            value = LOG_WARNING;
        else if (s == "LOG_WARN")
            value = LOG_WARNING;
        else if (s == "WARN")
            value = LOG_WARNING;
        else if (s == "LOG_ERROR")
            value = LOG_ERROR;
        else if (s == "ERROR")
            value = LOG_ERROR;
        else if (s == "LOG_CRITICAL")
            value = LOG_CRITICAL;
        else if (s == "CRITICAL")
            value = LOG_CRITICAL;
        else if (s == "LOG_SEVERE")
            value = LOG_CRITICAL;
        else if (s == "SEVERE")
            value = LOG_CRITICAL;
        else
            throw except::InvalidFormatException(Ctxt(str::Format("Invalid enum value: %s", s)));
    }

    //! int constructor
    LogLevel(int i)
    {
        switch(i)
        {
        case 0:
            value = LOG_NOTSET;
            break;
        case 1:
            value = LOG_DEBUG;
            break;
        case 2:
            value = LOG_INFO;
            break;
        case 3:
            value = LOG_WARNING;
            break;
        case 4:
            value = LOG_ERROR;
            break;
        case 5:
            value = LOG_CRITICAL;
            break;
        default:
            throw except::InvalidFormatException(Ctxt(str::Format("Invalid enum value: %d", i)));
        }
    }

    //! destructor
    ~LogLevel() = default;

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return std::string("NOTSET");
        case 1:
            return std::string("DEBUG");
        case 2:
            return std::string("INFO");
        case 3:
            return std::string("WARNING");
        case 4:
            return std::string("ERROR");
        case 5:
            return std::string("CRITICAL");
        default:
            throw except::InvalidFormatException(Ctxt(str::Format("Invalid enum value: %d", value)));
        }
    }

    //! assignment operator
    LogLevel& operator=(const LogLevel&) = default;
    LogLevel(const LogLevel&) = default;
    LogLevel& operator=(LogLevel&&) = default;
    LogLevel(LogLevel&&) = default;

    bool operator==(const LogLevel& o) const { return value == o.value; }
    bool operator!=(const LogLevel& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    LogLevel& operator=(const int& o) { value = o; return *this; }
    bool operator<(const LogLevel& o) const { return value < o.value; }
    bool operator>(const LogLevel& o) const { return value > o.value; }
    bool operator<=(const LogLevel& o) const { return value <= o.value; }
    bool operator>=(const LogLevel& o) const { return value >= o.value; }
    operator int() const { return value; }
    operator std::string() const { return toString(); }

    int value;

};

// code auto-generated 2011-03-15 13:25:08.574563

}

#endif

