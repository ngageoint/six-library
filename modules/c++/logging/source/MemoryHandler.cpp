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

#include <io/StringStream.h>
#include <logging/MemoryHandler.h>

namespace logging
{
MemoryHandler::MemoryHandler(LogLevel level) : Handler(level)
{
    //might as well setup the map -- we could let emit take care of it,
    //but this would allow for less chance of an exception getting thrown
    for (int logLevel = 0; logLevel <= LogLevel::LOG_CRITICAL; ++logLevel)
    {
        mLogMap[logLevel];
    }
}

const std::vector<std::string>& MemoryHandler::getLogs(LogLevel level) const
{
    const LogMap::const_iterator iter = mLogMap.find(level);
    if (iter == mLogMap.end())
    {
        throw except::NoSuchKeyException(Ctxt(
                "LogLevel: " + level.toString()));
    }
    return iter->second;
}

void MemoryHandler::write(const std::string& str)
{
    for (LogMap::iterator iter = mLogMap.begin();
         iter != mLogMap.end();
         ++iter)
    {
        iter->second.push_back(str);
    }
}

void MemoryHandler::emitRecord(const LogRecord* record)
{
    io::StringStream ostr;
    mFormatter->format(record, ostr);

    const std::string recordStr(ostr.stream().str());
    mLogMap[record->getLevel()].push_back(recordStr);
    if (record->getLevel() != LogLevel::LOG_NOTSET)
    {
        mLogMap[LogLevel::LOG_NOTSET].push_back(recordStr);
    }
}
}
