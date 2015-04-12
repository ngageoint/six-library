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
//  Handler.cpp
///////////////////////////////////////////////////////////

#include "logging/Handler.h"


logging::Handler::Handler(logging::LogLevel level)
{
    mLevel = level;
    mFormatter = NULL;
}

void logging::Handler::close()
{
    if (mFormatter) delete mFormatter;
}

std::string logging::Handler::format(logging::LogRecord* record) const
{
    //static logging::Formatter dftFormatter(mDefaultFmt);
    if (mFormatter)
        return mFormatter->format(record);
    else
        return mDefaultFormatter.format(record);
}

void logging::Handler::setLevel(logging::LogLevel level)
{
    mLevel = level;
}

bool logging::Handler::handle(logging::LogRecord* record)
{
    bool rv = false;
    if (filter(record))
    {
        //acquire lock
        mHandlerLock.lock();
        try
        {
            emitRecord(record);
            rv = true;
        }
        catch (except::Throwable & t)
        {
            //TODO do something here?
            //std::cout << t.getTrace() << std::endl;
        }
        //release lock
        mHandlerLock.unlock();
    }
    return rv;
}

void logging::Handler::setFormatter(logging::Formatter* formatter)
{
    mFormatter = formatter;
}
