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
//  Logger.h
///////////////////////////////////////////////////////////

#ifndef __LOGGING_LOGGER_H__
#define __LOGGING_LOGGER_H__

#include <string>
#include <vector>
#include "logging/Filterer.h"
#include "logging/LogRecord.h"
#include "logging/Handler.h"
#include <import/except.h>

namespace logging
{

/*!
 * \class Logger
 *
 * Instances of the Logger class represent a single logging channel.
 * A Logger instance can log to several Handlers.
 */
struct Logger : public Filterer
{
    /*!
     * Constructs a Logger with an optional name
     * \param name  (optional) Name of the logger
     */
    Logger(const std::string& name = "") :
        mName(name)
    {
    }

    virtual ~Logger();

    //! Logs a message at the specified LogLevel
    void log(LogLevel level, const std::string& msg);

    //! Logs an Exception Context at the specified LogLevel
    void log(LogLevel level, const except::Context& ctxt);

    //! Logs a Throwable at the specified LogLevel
    void log(LogLevel level, const except::Throwable& t);

    //! Logs a message at the DEBUG LogLevel
    void debug(const std::string& msg);
    //! Logs a message at the INFO LogLevel
    void info(const std::string& msg);
    //! Logs a message at the WARNING LogLevel
    void warn(const std::string& msg);
    //! Logs a message at the ERROR LogLevel
    void error(const std::string& msg);
    //! Logs a message at the CRITICAL LogLevel
    void critical(const std::string& msg);

    //! Logs an Exception Context at the DEBUG LogLevel
    void debug(const except::Context& ctxt);
    //! Logs an Exception Context at the INFO LogLevel
    void info(const except::Context& ctxt);
    //! Logs an Exception Context at the WARNING LogLevel
    void warn(const except::Context& ctxt);
    //! Logs an Exception Context at the ERROR LogLevel
    void error(const except::Context& ctxt);
    //! Logs an Exception Context at the CRITICAL LogLevel
    void critical(const except::Context& ctxt);

    //! Logs a Throwable at the DEBUG LogLevel
    void debug(const except::Throwable& t);
    //! Logs a Throwable at the INFO LogLevel
    void info(const except::Throwable& t);
    //! Logs a Throwable at the WARNING LogLevel
    void warn(const except::Throwable& t);
    //! Logs a Throwable at the ERROR LogLevel
    void error(const except::Throwable& t);
    //! Logs a Throwable at the CRITICAL LogLevel
    void critical(const except::Throwable& t);

    /*!
     * Adds a Hander to the list of Handlers this Logger delegates to.
     * This Logger does not own the passed-in Handler.
     */
    void addHandler(Handler* handler, bool own = false);

    /*!
     * Removes the specified Handler from the list of Handlers.
     */
    void removeHandler(Handler* handler);

    /*!
     * This sets the LogLevel for all of the Handlers for this Logger
     */
    void setLevel(LogLevel level);

    //! Sets the name of the Logger
    void setName(const std::string& name)
    {
        mName = name;
    }
    //! Returns the name of the Logger
    std::string getName() const
    {
        return mName;
    }

    //! Removes all handlers
    void reset();

    // NOTE: It isn't currently safe to copy a logger because mHandlers isn't
    //       a deep copy and you end up with a double delete (it's not using
    //       smart pointers :o( ).  If we really wanted to support a copy,
    //       would need to decide if mHandlers should be deeply or shallowly
    //       copied.
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

protected:
    void handle(const LogRecord* record);

    typedef std::pair<Handler*, bool> Handler_T;
    typedef std::vector<Handler_T> Handlers_T;

    std::string mName;
    Handlers_T mHandlers;

};
typedef mem::SharedPtr<Logger> LoggerPtr;
}
#endif
