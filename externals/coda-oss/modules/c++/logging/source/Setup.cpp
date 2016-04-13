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
 
#include <str/Manip.h>

#include "logging/StreamHandler.h"
#include "logging/FileHandler.h"
#include "logging/RotatingFileHandler.h"
#include "logging/StandardFormatter.h"
#include "logging/XMLFormatter.h"

#include "logging/Setup.h"

logging::LoggerPtr
logging::setupLogger(const std::string& program, 
                     const std::string& logLevel, 
                     const std::string& logFile,
                     const std::string& logFormat,
                     size_t logCount,
                     size_t logBytes)
{
    LoggerPtr log(new logging::Logger(program));

    // setup logging level
    std::string lev = logLevel;
    str::upper(lev);
    str::trim(lev);
    logging::LogLevel level = (lev.empty()) ? logging::LogLevel::LOG_WARNING :
                                              logging::LogLevel(lev);

    // setup logging formatter
    std::auto_ptr <logging::Formatter> formatter;
    std::string file = logFile;
    str::lower(file);
    if (str::endsWith(file, ".xml"))
    {
        formatter.reset(
            new logging::XMLFormatter("", "<Log image=\"" + program + "\">"));
    }
    else
    {
        formatter.reset(new logging::StandardFormatter(logFormat));
    }
    
    // setup logging handler
    std::auto_ptr < logging::Handler > logHandler;
    if (file.empty() || file == "console")
        logHandler.reset(new logging::StreamHandler());
    else
    {
        // create a rotating logger
        logCount = (logCount < 0) ? 0 : logCount;
        logBytes = (logBytes < 0) ? 0 : logBytes;
        if (logBytes > 0)
        {
            logHandler.reset(new logging::RotatingFileHandler(logFile,
                                                              logBytes,
                                                              logCount));
        }
        // create regular logging to one file
        else
        {
            logHandler.reset(new logging::FileHandler(logFile));
        }
    }
	
    logHandler->setLevel(level);
    logHandler->setFormatter(formatter.release());
    log->addHandler(logHandler.release(), true);

    return log;
}

