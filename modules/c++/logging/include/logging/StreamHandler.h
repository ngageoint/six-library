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
//  StreamHandler.h
///////////////////////////////////////////////////////////

#ifndef __LOGGING_STREAM_HANDLER_H__
#define __LOGGING_STREAM_HANDLER_H__

#include <memory>
#include "logging/LogRecord.h"
#include "logging/Handler.h"
#include <import/io.h>

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
    StreamHandler(LogLevel level = LOG_NOTSET);

    //! Constructs a StreamHandler using the specified OutputStream
    StreamHandler(io::OutputStream* stream, LogLevel level = LOG_NOTSET);

    virtual ~StreamHandler();
    virtual void close();

protected:
    virtual void emitRecord(LogRecord* record);
    std::auto_ptr<io::OutputStream> mStream;

};

}
#endif
