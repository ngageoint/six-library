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
//  Handler.cpp
///////////////////////////////////////////////////////////

#include "logging/Handler.h"


logging::Handler::Handler(logging::LogLevel level)
{
    mLevel = level;

    // use standard formatter by default
    mFormatter = &mDefaultFormatter;
}

void logging::Handler::close()
{
    // delete if necessary
    if (mFormatter != &mDefaultFormatter &&
        mFormatter != NULL)
        delete mFormatter;
}

void logging::Handler::setLevel(logging::LogLevel level)
{
    mLevel = level;
}

bool logging::Handler::handle(const logging::LogRecord* record)
{
    bool rv = false;
    if (filter(record))
    {
        //acquire lock
        mt::CriticalSection<sys::Mutex> lock(&mHandlerLock);
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
    }
    return rv;
}
void logging::Handler::setFormatter(logging::Formatter* formatter)
{
    //check if current formatter
    if (mFormatter != formatter)
    {
        // delete old formatter
        Handler::close();

        mFormatter = formatter;
    }
}
