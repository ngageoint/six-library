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
//  LogRecord.cpp
///////////////////////////////////////////////////////////

#include "logging/LogRecord.h"
#include "sys/TimeStamp.h"

static const std::string LOG_LEVEL_NAMES[] = {"NOTSET", "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"};

logging::LogLevel logging::getLogLevelAsEnum(const std::string& levelStr)
{
    logging::LogLevel level = logging::LOG_DEBUG;

    if (levelStr == LOG_LEVEL_NAMES[2])
        level = logging::LOG_INFO;
    else if (levelStr == LOG_LEVEL_NAMES[3])
        level = logging::LOG_WARNING;
    else if (levelStr == LOG_LEVEL_NAMES[4])
        level = logging::LOG_ERROR;
    else if (levelStr == LOG_LEVEL_NAMES[5])
        level = logging::LOG_CRITICAL;
    return level;
}

logging::LogRecord::LogRecord(std::string name, std::string msg, logging::LogLevel level)
        : mName(name), mMsg(msg), mLevel(level), mFile(""), mFunction(""), mLineNum(-1)
{
    mTimestamp = sys::TimeStamp(true).local();
}


std::string logging::LogRecord::getLevelName() const { return LOG_LEVEL_NAMES[mLevel]; };

