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
//  FileHandler.h
///////////////////////////////////////////////////////////

#ifndef __LOGGING_FILE_HANDLER_H__
#define __LOGGING_FILE_HANDLER_H__

#include <string>
#include <iostream>
#include "logging/LogRecord.h"
#include "logging/StreamHandler.h"
#include <import/io.h>
#include <import/sys.h>
#include <import/mem.h>

namespace logging
{
/*!
 * \class FileHandler
 * \brief Emits LogRecords to a file on disk.
 */
struct FileHandler : public StreamHandler
{
    FileHandler(const coda_oss::filesystem::path& fname, LogLevel level = LogLevel::LOG_NOTSET,
                int creationFlags = sys::File::CREATE | sys::File::TRUNCATE) :
        StreamHandler(coda_oss::make_unique<io::FileOutputStream>(fname.string(), creationFlags), level)
    {
        // In case we are in append mode
        if (auto pStream = dynamic_cast<io::FileOutputStream*>(mStream.get()))
        {
            pStream->seek(0, io::Seekable::END);
        }
    }
    virtual ~FileHandler() = default;

    FileHandler(const FileHandler&) = delete;
    FileHandler& operator=(const FileHandler&) = delete;

};
}
#endif
