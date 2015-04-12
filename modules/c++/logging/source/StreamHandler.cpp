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
//  StreamHandler.cpp
///////////////////////////////////////////////////////////

#include "logging/StreamHandler.h"

logging::StreamHandler::StreamHandler(logging::LogLevel level) :
    logging::Handler(level)
{
    mStream.reset(new io::StandardOutStream());
}

logging::StreamHandler::StreamHandler(io::OutputStream* stream,
                                      logging::LogLevel level) :
    logging::Handler(level)
{
    if (!stream)
        mStream.reset(new io::StandardOutStream());
    else
        mStream.reset(stream);
}

logging::StreamHandler::~StreamHandler()
{
    close();
}

void logging::StreamHandler::close()
{
    if (mStream.get())
        mStream->close();
}

void logging::StreamHandler::emitRecord(logging::LogRecord* record)
{
    mStream->write(format(record) + "\n");
    mStream->flush();
}
