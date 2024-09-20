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
//  DefaultLogger.h
///////////////////////////////////////////////////////////

#ifndef __LOGGING_DEFAULT_LOGGER_H__
#define __LOGGING_DEFAULT_LOGGER_H__

#include "logging/Logger.h"
#include "logging/StreamHandler.h"

namespace logging
{

/*!
 * \class DefaultLogger
 *
 * \brief DefaultLogger extends the Logger class by providing a default Handler
 * which logs all messages to a StandardStream. This can of course be overridden.
 */
class DefaultLogger : public Logger
{
protected:
    Handler* mDefaultHandler;

    static LogLevel defaultLogLevel;

public:
    DefaultLogger(std::string name = "");

    virtual ~DefaultLogger();

    DefaultLogger(const DefaultLogger&) = delete;
    DefaultLogger& operator=(const DefaultLogger&) = delete;

    //! Returns the default Handler. You can modify, but do not destroy.
    virtual Handler* getDefaultHandler()
    {
        return mDefaultHandler;
    }

    static void setDefaultLogLevel(LogLevel logLevel);
};

}
#endif
