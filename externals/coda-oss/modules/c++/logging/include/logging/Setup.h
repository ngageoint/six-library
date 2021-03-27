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
 
#ifndef CODA_OSS_logging_Setup_h_INCLUDED_
#define CODA_OSS_logging_Setup_h_INCLUDED_

#include <memory>
#include <string>

#include "coda_oss/filesystem.h"
#include "mem/SharedPtr.h"
#include "logging/Logger.h"

namespace logging
{

/*!
 *  \fn setupLogger
 *
 *  This is a simple utility for logging. Creation of the logger is
 *  non-trivial in nature, so this helpful utility reduces the setup
 *  to a few parameters.
 *
 *  \param program  - name of the program doing the logging
 *  \param logLevel - level of logging (debug|warning|info|error|critical)
 *  \param logFile  - location where to log (default: console logs to std::cout)
 *  \param logFormat- format of the log (default: [%p] (%d) %m)
 *  \param logCount - number of rotating logs to keep (default: 0 no rotation)
 *  \param logBytes - number of bytes per rotating log (default: 0 no rotation)
 */
mem::auto_ptr<logging::Logger> setupLogger(
    const coda_oss::filesystem::path& program, 
    const std::string& logLevel = "warning", 
    const coda_oss::filesystem::path& logFile = "console",
    const std::string& logFormat = "[%p] (%d) %m",
    size_t logCount = 0,
    size_t logBytes = 0);
}

#endif // CODA_OSS_logging_Setup_h_INCLUDED_
