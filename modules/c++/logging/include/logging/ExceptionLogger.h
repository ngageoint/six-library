/* =========================================================================
 * This file is part of logging-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2013 - 2014, MDA Information Systems LLC
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
//  ExceptionLogger.h
///////////////////////////////////////////////////////////

#ifndef __LOGGING_EXCEPTION_LOGGER_H__
#define __LOGGING_EXCEPTION_LOGGER_H__

#include "logging/Logger.h"
#include "logging/StreamHandler.h"
#include <sys/Mutex.h>
#include <mt/CriticalSection.h>

namespace logging
{

/*!
 * \class ExceptionLogger
 *
 * \brief ExceptionLogger owns a Logger and logs exceptions passed to it. 
 */
class ExceptionLogger
{
protected:
    sys::Mutex mLock;

    Logger* mLogger;

    bool mHasLogged;

public:
    ExceptionLogger(Logger* logger) : mLogger(logger), mHasLogged(false) 
    {}

    virtual ~ExceptionLogger() {}

    //! Tells whether it has logged at least one exception
    bool hasLogged()
    {
        mt::CriticalSection<sys::Mutex> crit(&mLock);
        return mHasLogged;
    }

    //! Log the exception/throwable
    void log(const except::Throwable& t, LogLevel logLevel)
    {
        mt::CriticalSection<sys::Mutex> crit(&mLock);
        mLogger->log(logLevel, t);
        mHasLogged = true;
    }
};

}
#endif
