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
//  MemoryHandler.h
///////////////////////////////////////////////////////////

#ifndef __LOGGING_MEMORY_HANDLER_H__
#define __LOGGING_MEMORY_HANDLER_H__

#include <string>
#include <list>
#include <map>
#include "logging/LogRecord.h"
#include "logging/Handler.h"

namespace logging
{
/*!
 * \class MemoryHandler
 * \brief Emits LogRecords to memory.
 */
class MemoryHandler : public Handler
{

public:
    MemoryHandler(LogLevel level = LOG_NOTSET);
    virtual ~MemoryHandler();

    std::list<std::string>& getLogs(LogLevel level = LOG_NOTSET);

protected:
    std::map<LogLevel, std::list<std::string> > mLogMap;

    //! Emits the LogRecord
    virtual void emitRecord(LogRecord* record);
};

}
#endif
