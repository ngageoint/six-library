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
//  LogRecord.cpp
///////////////////////////////////////////////////////////

#include "logging/LogRecord.h"
#include "sys/TimeStamp.h"

logging::LogRecord::LogRecord(std::string name, std::string msg, logging::LogLevel level)
        : mName(name), mMsg(msg), mLevel(level), mFile(""), mFunction(""), mLineNum(-1)
{
    mTimestamp = sys::TimeStamp(true).local();
}


std::string logging::LogRecord::getLevelName() const { return mLevel.toString(); }
