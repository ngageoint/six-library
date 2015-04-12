/* =========================================================================
 * This file is part of logging-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#include "logging/Filterer.h"
#include <import/sys.h>
#include <import/except.h>

namespace logging
{

/*!
 * \class Handler
 *
 * The Handler class is abstract. Sub-classes handle LogRecord* objects,
 * emitting the information in a user-defined manner.
 */
class Handler : public Filterer
{
public:
    /*!
     * Construct a Handler at the specified LogLevel (LOG_NOTSET is default)
     */
    Handler(LogLevel level = LOG_NOTSET);
    virtual ~Handler(){ close(); }

    //! Sets the Formatter to use when formatting LogRecords
    void setFormatter(Formatter* formatter);

    //! Sets the minimum LogLevel required to emit LogRecords
    void setLevel(LogLevel level);

    //! Returns the LogLevel
    LogLevel getLevel() const { return mLevel; }

    /*!
     * Handles the LogRecord
     * If the LogRecord meets the LogLevel criteria, it is formatted
     * and emitted.
     */
    virtual bool handle(LogRecord* record);

    virtual void close();

protected:
    //! Emits the LogRecord
    virtual void emitRecord(LogRecord* record) = 0;

    //! Applies the Formatter to the LogRecord and returns a std::string
    std::string format(LogRecord* record) const;

    LogLevel mLevel;
    sys::Mutex mHandlerLock;
    Formatter* mFormatter;
    Formatter mDefaultFormatter;
};

}
#endif
