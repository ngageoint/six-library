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
//  Formatter.h
///////////////////////////////////////////////////////////

#ifndef __LOGGING_FORMATTER_H__
#define __LOGGING_FORMATTER_H__

#include <string>
#include "logging/LogRecord.h"
#include "import/io.h"


namespace logging
{

/*!
 *  \class Formatter
 *  \brief  This class is the interface for deriving formatters.
 */
class Formatter
{
public:


    Formatter() : mFmt(""),
                  mPrologue(""),
                  mEpilogue("") 
    {
    }

    Formatter(const std::string& fmt, 
              const std::string& prologue = "",
              const std::string& epilogue = "") :
        mFmt(fmt),
        mPrologue(prologue),
        mEpilogue(epilogue) 
    {
    }
    
    virtual ~Formatter() {}

    Formatter& operator=(const Formatter&) = delete;

    // returns string
    virtual void format(const LogRecord* record, io::OutputStream& os) const = 0;

    virtual std::string getPrologue() const;
    virtual std::string getEpilogue() const; 

protected:

    static const char THREAD_ID[];
    static const char LOG_NAME[];
    static const char LOG_LEVEL[];
    static const char TIMESTAMP[];
    static const char FILE_NAME[];
    static const char LINE_NUM[];
    static const char MESSAGE[];
    static const char FUNCTION[];

    
    const std::string mFmt;
    const std::string mPrologue;
    const std::string mEpilogue;
};

}
#endif
