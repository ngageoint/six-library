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
//  RotatingFileHandler.h
///////////////////////////////////////////////////////////

#ifndef __LOGGING_ROTATING_FILE_HANDLER_H__
#define __LOGGING_ROTATING_FILE_HANDLER_H__

#include <string>
#include <iostream>
#include "logging/LogRecord.h"
#include "logging/StreamHandler.h"
#include <import/io.h>
#include <import/sys.h>

namespace logging
{

/*!
 * \class RotatingFileHandler
 *
 * RotatingFileHandler is a special Handler that emits logging to a file that
 * rotates once it reaches a certain maximum size. When the file rotates, it
 * will be renamed, thus creating a backup. For example, let's assume we are
 * logging to a file named 'error.log'. When the file rotates, it will be
 * renamed to 'error.log.1', and all future logs will be logged to error.log.
 * This is useful if you would like to keep backups of logs.
 */
class RotatingFileHandler : public StreamHandler
{

public:
    /*!
     * \param fname         The file to log to
     * \param maxBytes      The max file size
     * \param backupCount   The max number of backups
     * \param level         The minimum LogLevel
     */
    RotatingFileHandler(const sys::Filesystem::path& fname, long maxBytes = 0,
                        int backupCount = 0, LogLevel level = LogLevel::LOG_NOTSET);

    virtual ~RotatingFileHandler();
};
}
#endif
