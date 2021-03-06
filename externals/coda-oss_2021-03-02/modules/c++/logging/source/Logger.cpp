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
//  Logger.cpp
///////////////////////////////////////////////////////////

#include "logging/Logger.h"
#include <deque>

logging::Logger::~Logger()
{
    reset();
}

void logging::Logger::log(logging::LogLevel level, const std::string& msg)
{
    logging::LogRecord *rec = new logging::LogRecord(mName, msg, level);
    handle(rec);
    delete rec;
}

void logging::Logger::log(LogLevel level, const except::Context& ctxt)
{
    logging::LogRecord *rec = new logging::LogRecord(mName, ctxt.getMessage(),
                                                     level, ctxt.getFile(),
                                                     ctxt.getFunction(),
                                                     ctxt.getLine(),
                                                     ctxt.getTime());
    handle(rec);
    delete rec;
}

void logging::Logger::log(LogLevel level, const except::Throwable& t)
{
    std::deque<except::Context> savedContexts;
    except::Trace trace = t.getTrace();
    const size_t size = trace.getSize();
    if (size > 0)
    {
        for (size_t ii = 0; ii < size; ++ii)
        {
            savedContexts.push_front(trace.getContext());
            trace.popContext();
        }
        // Do this so we print the original context first
        for (size_t ii = 0; ii < savedContexts.size(); ++ii)
        {
            log(level, savedContexts[ii]);
        }
    }
    else
    {
        // Just log the message
        log(level, t.getMessage());
    }
}

void logging::Logger::debug(const std::string& msg)
{
    log(LogLevel::LOG_DEBUG, msg);
}

void logging::Logger::info(const std::string& msg)
{
    log(LogLevel::LOG_INFO, msg);
}

void logging::Logger::warn(const std::string& msg)
{
    log(LogLevel::LOG_WARNING, msg);
}

void logging::Logger::error(const std::string& msg)
{
    log(LogLevel::LOG_ERROR, msg);
}

void logging::Logger::critical(const std::string& msg)
{
    log(LogLevel::LOG_CRITICAL, msg);
}


void logging::Logger::debug(const except::Context& ctxt)
{
    log(LogLevel::LOG_DEBUG, ctxt);
}

void logging::Logger::info(const except::Context& ctxt)
{
    log(LogLevel::LOG_INFO, ctxt);
}

void logging::Logger::warn(const except::Context& ctxt)
{
    log(LogLevel::LOG_WARNING, ctxt);
}

void logging::Logger::error(const except::Context& ctxt)
{
    log(LogLevel::LOG_ERROR, ctxt);
}

void logging::Logger::critical(const except::Context& ctxt)
{
    log(LogLevel::LOG_CRITICAL, ctxt);
}

void logging::Logger::debug(const except::Throwable& t)
{
    log(LogLevel::LOG_DEBUG, t);
}

void logging::Logger::info(const except::Throwable& t)
{
    log(LogLevel::LOG_INFO, t);
}

void logging::Logger::warn(const except::Throwable& t)
{
    log(LogLevel::LOG_WARNING, t);
}

void logging::Logger::error(const except::Throwable& t)
{
    log(LogLevel::LOG_ERROR, t);
}

void logging::Logger::critical(const except::Throwable& t)
{
    log(LogLevel::LOG_CRITICAL, t);
}

void logging::Logger::handle(const logging::LogRecord* record)
{
    if (filter(record))
    {
        for (Handlers_T::iterator p = mHandlers.begin(); p != mHandlers.end(); ++p)
        {
            //std::cout << (int)(*p)->getLevel() << std::endl;
            //only handle if it is above/equal to threshold
            if (p->first->getLevel() <= record->getLevel())
                p->first->handle(record);
        }
    }
}

void logging::Logger::addHandler(logging::Handler* handler, bool own)
{
    //only add the handler if it isn't added already
    bool found = false;
    for (Handlers_T::iterator p = mHandlers.begin(); p != mHandlers.end()
            && !found; ++p)
    {
        if (p->first == handler)
        {
            found = true;
            p->second = own;
        }
    }
    if (!found)
        mHandlers.push_back(Handler_T(handler, own));
}

void logging::Logger::removeHandler(logging::Handler* handler)
{
    //find and remove, if it exists
    for (Handlers_T::iterator p = mHandlers.begin(); p != mHandlers.end(); ++p)
    {
        if (p->first == handler)
        {
            mHandlers.erase(p);
            break;
        }
    }
}

void logging::Logger::setLevel(LogLevel level)
{
    for (Handlers_T::iterator p = mHandlers.begin(); p != mHandlers.end(); ++p)
    {
        //set the level
        p->first->setLevel(level);
    }
}

void logging::Logger::reset()
{
    for (Handlers_T::iterator p = mHandlers.begin(); p
            != mHandlers.end(); ++p)
    {
        if (p->second && p->first)
            delete p->first;
    }
    mHandlers.clear();
}
