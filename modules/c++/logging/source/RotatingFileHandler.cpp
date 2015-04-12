/* =========================================================================
 * This file is part of logging-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

RotatingFileHandler::RotatingFileHandler(const std::string& fname,
                                         long maxBytes, int backupCount,
                                         LogLevel level) :
    StreamHandler(level), mLogFile(fname), mMaxBytes(maxBytes),
            mBackupCount(backupCount)
{
    sys::OS os;

    if (os.exists(mLogFile))
    {
        mStream.reset(new io::FileOutputStream(mLogFile, sys::File::EXISTING));
        ((io::FileOutputStream*) mStream.get())->seek(0, io::Seekable::END);
    }
    else
    {
        //see if we need to make the parent directory
        std::string parDir = sys::Path::splitPath(mLogFile).first;
        if (!os.exists(parDir))
            os.makeDirectory(parDir);
        mStream.reset(new io::FileOutputStream(mLogFile, sys::File::CREATE));
    }

    // in case we already have a full log to begin with
    if (shouldRollover(NULL))
        doRollover();
}

RotatingFileHandler::~RotatingFileHandler()
{
    // the StreamHandler destructor closes the stream
}

void RotatingFileHandler::emitRecord(logging::LogRecord* record)
{
    if (shouldRollover(record))
        doRollover();
    StreamHandler::emitRecord(record);
}

bool RotatingFileHandler::shouldRollover(LogRecord* record)
{
    if (mMaxBytes > 0)
    {
        std::string msg = record ? format(record) : "";
        io::FileOutputStream* fos = (io::FileOutputStream*) mStream.get();
        sys::Off_T pos = fos->tell();

        // first check if we are at the beginning of the file
        // if one log message overflows our max bytes, we'll just write it
        // the other option is to not write it at all - at least this way we
        // track it
        if (pos == 0 && msg.length() > mMaxBytes)
            return false;

        // otherwise, if this message puts us over, we rollover
        if (pos + msg.length() > mMaxBytes)
            return true;
    }
    return false;
}

void RotatingFileHandler::doRollover()
{
    io::FileOutputStream* fos = (io::FileOutputStream*) mStream.get();
    fos->close();
    sys::OS os;

    if (mBackupCount > 0)
    {
        for (int i = mBackupCount - 1; i > 0; --i)
        {
            std::stringstream curName;
            curName << mLogFile << "." << i;
            std::stringstream nextName;
            nextName << mLogFile << "." << (i + 1);
            if (os.exists(curName.str()))
            {
                if (os.exists(nextName.str()))
                {
                    os.remove(nextName.str());
                }
                os.move(curName.str(), nextName.str());
            }
        }
        std::string curName = mLogFile + ".1";
        if (os.exists(curName))
            os.remove(curName);
        os.move(mLogFile, curName);
    }
    mStream.reset(new io::FileOutputStream(mLogFile, sys::File::CREATE));
}
