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
//  MemoryHandler.h
///////////////////////////////////////////////////////////

#ifndef __LOGGING_MEMORY_HANDLER_H__
#define __LOGGING_MEMORY_HANDLER_H__

#include <string>
#include <vector>
#include <map>
#include "logging/LogRecord.h"
#include "logging/Handler.h"

namespace logging
{
/*!
 * \class MemoryHandler
 * \brief Emits LogRecords to memory.
 */
struct MemoryHandler : public Handler
{
    MemoryHandler(LogLevel level = LogLevel::LOG_NOTSET);

    MemoryHandler& operator=(const MemoryHandler&) = delete;

    const std::vector<std::string>&
    getLogs(LogLevel level = LogLevel::LOG_NOTSET) const;

protected:
    virtual void write(const std::string& str);

    virtual void emitRecord(const LogRecord* record);

private:
    typedef std::map<LogLevel, std::vector<std::string> > LogMap;
    LogMap mLogMap;
};

}
#endif
