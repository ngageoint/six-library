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
//  NullHandler.h
///////////////////////////////////////////////////////////

#ifndef __LOGGING_NULL_LOGGER_H__
#define __LOGGING_NULL_LOGGER_H__

#include "logging/Logger.h"
#include "logging/Handler.h"

namespace logging
{

/*!
 * \class NullHandler
 * \brief Ignores all records
 */
struct NullHandler : public Handler
{
    NullHandler(LogLevel level = LogLevel::LOG_NOTSET) :
        Handler(level)
    {
    }
    ~NullHandler() = default;

    NullHandler(const NullHandler&) = delete;
    NullHandler& operator=(const NullHandler&) = delete;

protected:
        
    void write(const std::string&) override
    {
        // does nothing...
    }
    void emitRecord(const LogRecord*) override
    {
        // does nothing...
    }
};

/**
 * A Logger that doesn't actually log anything. It can be used in cases where
 * a Logger is required but you don't want anything to log.
 */
class NullLogger final : public Logger
{
    NullHandler mHandler;

public:
    NullLogger(const std::string& name = "") :
        Logger(name)
    {
        addHandler(&mHandler, false /*own*/);
    }
    ~NullLogger() = default;

    NullLogger(const NullLogger&) = delete;
    NullLogger& operator=(const NullLogger&) = delete;
};

}
#endif
