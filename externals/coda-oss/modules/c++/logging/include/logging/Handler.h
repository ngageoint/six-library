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
//  Handler.h
///////////////////////////////////////////////////////////

#ifndef __LOGGING_HANDLER_H__
#define __LOGGING_HANDLER_H__

#include <string>
#include "logging/LogRecord.h"
#include "logging/Formatter.h"
#include "logging/StandardFormatter.h"
#include "logging/Filterer.h"
#include <import/sys.h>
#include <import/except.h>
#include <import/mt.h>

namespace logging
{

/*!
 * \class Handler
 *
 * The Handler class is abstract. Sub-classes handle LogRecord* objects,
 * emitting the information in a user-defined manner.
 */
struct Handler : public Filterer
{
    /*!
     * Construct a Handler at the specified LogLevel (LogLevel::LOG_NOTSET is default)
     */
    Handler(LogLevel level = LogLevel::LOG_NOTSET);
    virtual ~Handler()
    {
    }
    Handler& operator=(const Handler&) = delete;

    /*! 
     * Sets the Formatter to use when formatting LogRecords
     * Not Threads Safe!
     */ 
    virtual void setFormatter(Formatter* formatter);

    //! Sets the minimum LogLevel required to emit LogRecords
    void setLevel(LogLevel level);

    //! Returns the LogLevel
    LogLevel getLevel() const { return mLevel; }

    /*!
     * Handles the LogRecord
     * If the LogRecord meets the LogLevel criteria, it is formatted
     * and emitted.
     */
    virtual bool handle(const LogRecord* record);

    virtual void close();

protected:

    // for general string write
    virtual void write(const std::string&) = 0;

    // for writing directly to stream, 
    // used for the bulk of the logging for speed
    virtual void emitRecord(const LogRecord* record) = 0;


    LogLevel mLevel = LogLevel::LOG_NOTSET;
    sys::Mutex mHandlerLock;
    Formatter* mFormatter = nullptr;
    StandardFormatter mDefaultFormatter; 
};

}
#endif
