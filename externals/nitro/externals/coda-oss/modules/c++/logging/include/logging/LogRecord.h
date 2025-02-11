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
//  LogRecord.h
///////////////////////////////////////////////////////////

#ifndef __LOGGING_LOG_RECORD_H__
#define __LOGGING_LOG_RECORD_H__

#include <string>
#include "logging/Enums.h"

namespace logging
{

/*!
 * \class LogRecord
 *
 * \brief A LogRecord instance represents an event being logged.
 * LogRecord instances are created every time something is logged. They
 * contain all the information pertinent to the event being logged. The
 * record also includes the timestamp when the record was created.
 */
class LogRecord
{

public:
    LogRecord(std::string name, std::string msg, LogLevel level = LogLevel::LOG_NOTSET);
    LogRecord(std::string name, std::string msg, LogLevel level,
              std::string file, std::string function, int lineNum, std::string timestamp) :
            mName(name), mMsg(msg), mLevel(level), mFile(file), mFunction(function),
            mLineNum(lineNum), mTimestamp(timestamp){}
    virtual ~LogRecord(){}

    LogLevel getLevel() const { return mLevel; }
    std::string getLevelName() const;

    std::string getMessage() const { return mMsg; }
    std::string getName() const { return mName; }
    std::string getTimeStamp() const { return mTimestamp; }
    std::string getFile() const { return mFile; }
    std::string getFunction() const { return mFunction; }
    int getLineNum() const { return mLineNum; }


private:
    std::string mName;
    std::string mMsg;
    LogLevel mLevel;
    std::string mFile;
    std::string mFunction;
    int mLineNum;
    std::string mTimestamp;
};

}
#endif
