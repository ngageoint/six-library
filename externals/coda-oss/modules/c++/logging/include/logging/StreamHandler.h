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
//  StreamHandler.h
///////////////////////////////////////////////////////////

#ifndef __LOGGING_STREAM_HANDLER_H__
#define __LOGGING_STREAM_HANDLER_H__

#include <memory>
#include "logging/LogRecord.h"
#include "logging/Handler.h"
#include <import/io.h>
#include <mem/SharedPtr.h>

namespace logging
{

/*!
 * \class StreamHandler
 * \brief Emits LogRecords to an io::OutputStream
 */
class StreamHandler : public Handler
{
public:
    //! Constructs a StreamHandler that uses an io::StandardOutStream
    StreamHandler(LogLevel level = LogLevel::LOG_NOTSET);

    //! Constructs a StreamHandler using the specified OutputStream
    StreamHandler(io::OutputStream* stream, LogLevel level = LogLevel::LOG_NOTSET);

    virtual ~StreamHandler();

    StreamHandler& operator=(const StreamHandler&) = delete;

    //! adds the need to write epilogue before deleting formatter
    //  and then writing the prologue with the new formatter
    virtual void setFormatter(Formatter* formatter);

    virtual void close();

protected:
    // This is necessary so this class and an inherited class can call a
    // non-virtual version of close in its destructor.
    void closeImpl();

    //! for general string write
    virtual void write(const std::string&);

    //! for writing directly to stream,
    // used for the bulk of the logging for speed
    virtual void emitRecord(const LogRecord* record);

    mem::auto_ptr<io::OutputStream> mStream;

private:
    bool mClosed;
};

}
#endif
