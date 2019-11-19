/* =========================================================================
 * This file is part of logging
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
#include <sstream>
#include <iostream>
#include <import/sys.h>
#include <import/str.h>
#include "logging/XMLFormatter.h"

// default log for xml --
// Writing without a dom keeps the format function const, 
// and saves us from creating doms for every log
const char logging::XMLFormatter::DEFAULT_FORMAT[] = 
"\t<Record name=\"%c\" level=\"%p\" date=\"%d\">\n\
\t\t<FileName>%F</FileName>\n\
\t\t<Message>%m</Message>\n\
\t</Record>";

logging::XMLFormatter::XMLFormatter() : logging::Formatter(DEFAULT_FORMAT, "<Log>", "</Log>")
{
    // TODO: Generate a better prologue that contains
    //       date, jobID, and other information that
    //       can distinguish the logs from one another
}
logging::XMLFormatter::XMLFormatter(const std::string& fmt, 
                               const std::string& prologue,
                               const std::string& epilogue) :
    logging::Formatter((fmt.empty()) ? DEFAULT_FORMAT : fmt,
                       prologue,
                       epilogue)
{
}

void logging::XMLFormatter::format(const logging::LogRecord* record, io::OutputStream& os) const
{
    // conver record
    std::string name = (record->getName().empty()) ? 
        ("DEFAULT") : record->getName();
    std::string line = str::toString<int>(record->getLineNum());
    std::string threadID =  str::toString(sys::getThreadID());

    // populate vector with record
    std::vector<std::string> logRecord;
    logRecord.push_back(threadID);
    logRecord.push_back(name);    
    logRecord.push_back(record->getLevelName());    
    logRecord.push_back(record->getTimeStamp());    
    logRecord.push_back(record->getFile());    
    logRecord.push_back(line);    
    logRecord.push_back(record->getFunction());    
    logRecord.push_back(record->getMessage());    


    // update record with SGML escape characters
    for (size_t chr = 4; chr < logRecord.size(); chr++)
    { 
        str::escapeForXML(logRecord[chr]);
    }


    // populate log
    std::string format = mFmt;
    str::replace(format, THREAD_ID, logRecord[0]);
    str::replace(format, LOG_NAME,  logRecord[1]);
    str::replace(format, LOG_LEVEL, logRecord[2]);
    str::replace(format, TIMESTAMP, logRecord[3]);
    if (record->getLineNum() >= 0)
    {
        str::replace(format, FILE_NAME, logRecord[4]);
        str::replace(format, LINE_NUM,  logRecord[5]);
    }
    else
    {
        str::replace(format, FILE_NAME, "");
        str::replace(format, LINE_NUM,  "");
    }
    str::replace(format, FUNCTION, logRecord[6]);
    str::replace(format, MESSAGE,  logRecord[7]);

    // write to stream
    os.write(format + "\n");
}

