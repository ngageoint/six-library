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
//  StreamHandler.cpp
///////////////////////////////////////////////////////////

#include "logging/StreamHandler.h"

namespace logging
{
StreamHandler::StreamHandler(LogLevel level) :
    Handler(level),
    mClosed(false)
{
    mStream.reset(new io::StandardOutStream());

    // write prologue to stream
    write(mFormatter->getPrologue());
}

StreamHandler::StreamHandler(io::OutputStream* stream,
                             LogLevel level) :
    Handler(level),
    mClosed(false)
{
    mStream.reset(stream);

    // write prologue to stream
    write(mFormatter->getPrologue());
}

StreamHandler::~StreamHandler()
{
    try
    {
        closeImpl();
    }
    catch (...)
    {
    }
}

void StreamHandler::setFormatter(Formatter* formatter)
{
    // end log with formatter injection
    write(mFormatter->getEpilogue());

    // delete old and reset to new
    Handler::setFormatter(formatter);

    // start log with formatter injection
    write(mFormatter->getPrologue());
}

void StreamHandler::close()
{
    closeImpl();
}

void StreamHandler::closeImpl()
{
    if (!mClosed)
    {
        // end log with formatter injection
        write(mFormatter->getEpilogue());

        // delete formatter
        Handler::close();

        // kill stream
        if (mStream.get())
            mStream->close();

        mClosed = true;
    }
}

void StreamHandler::write(const std::string& str)
{
    if (!str.empty())
    {
        //acquire lock
        mt::CriticalSection<sys::Mutex> lock(&mHandlerLock);

        // write to stream
        mStream->write(str);
        mStream->flush();
    }
}
void StreamHandler::emitRecord(const LogRecord* record)
{
    mFormatter->format(record, *mStream);
    mStream->flush();
}
}
