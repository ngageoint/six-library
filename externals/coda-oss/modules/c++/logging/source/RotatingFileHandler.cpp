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
//  RotatingFileHandler.cpp
///////////////////////////////////////////////////////////

#include "logging/RotatingFileHandler.h"

using namespace logging;

RotatingFileHandler::RotatingFileHandler(const sys::Filesystem::path& fname_,
                                         long maxBytes, int backupCount,
                                         LogLevel level) :
    StreamHandler(level)
{
    const auto fname = fname_.string();

    sys::OS os;
    int creationFlags;

    // create directory if one doesn't exist
    if (!os.exists(fname))
    {
        //see if we need to make the parent directory
        std::string parDir = sys::Path::splitPath(fname).first;
        if (!os.exists(parDir))
            os.makeDirectory(parDir);
    }
    // do rollover, so we start fresh
    if (backupCount > 0)
    {
        for (int i = backupCount - 1; i > 0; --i)
        {
            std::stringstream curName;
            curName << fname << "." << i;
            std::stringstream nextName;
            nextName << fname << "." << (i + 1);
            if (os.exists(curName.str()))
            {
                if (os.exists(nextName.str()))
                {
                    os.remove(nextName.str());
                }
                os.move(curName.str(), nextName.str());
            }
        }
        std::string curName = fname + ".1";
        if (os.exists(curName))
            os.remove(curName);
        os.move(fname, curName);
    }
    
    // create log file
    creationFlags = sys::File::CREATE | sys::File::TRUNCATE;
    mStream.reset(new io::RotatingFileOutputStream(fname, maxBytes,
                                                   backupCount, creationFlags));
}

RotatingFileHandler::~RotatingFileHandler()
{
    // the StreamHandler destructor closes the stream
}
