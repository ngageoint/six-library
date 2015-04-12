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
//  LoggerFactory.cpp
///////////////////////////////////////////////////////////

#include "logging/LoggerFactory.h"


logging::LoggerManager::~LoggerManager()
{
    for (std::map<std::string, logging::Logger*>::iterator it =  mLoggerMap.begin();
            it != mLoggerMap.end(); ++it)
    {
        if (it->second)
        {
            delete it->second;
        }
    }
}

logging::Logger* logging::LoggerManager::getLogger(std::string name)
{
    mt::CriticalSection<sys::Mutex> obtainLock(&mMutex);
    if (mLoggerMap.find(name) == mLoggerMap.end())
    {
        logging::Logger* logger = new logging::DefaultLogger(name);
        mLoggerMap[name] = logger;
    }
    return mLoggerMap[name];
}





void logging::debug(const std::string& msg)
{ logging::LoggerFactory::getInstance().getLogger()->debug(msg); }
void logging::info(const std::string& msg)
{ logging::LoggerFactory::getInstance().getLogger()->info(msg); }
void logging::warn(const std::string& msg)
{ logging::LoggerFactory::getInstance().getLogger()->warn(msg); }
void logging::error(const std::string& msg)
{ logging::LoggerFactory::getInstance().getLogger()->error(msg); }
void logging::critical(const std::string& msg)
{ logging::LoggerFactory::getInstance().getLogger()->critical(msg); }
void logging::debug(const except::Context& ctxt)
{ logging::LoggerFactory::getInstance().getLogger()->debug(ctxt); }
void logging::info(const except::Context& ctxt)
{ logging::LoggerFactory::getInstance().getLogger()->info(ctxt); }
void logging::warn(const except::Context& ctxt)
{ logging::LoggerFactory::getInstance().getLogger()->warn(ctxt); }
void logging::error(const except::Context& ctxt)
{ logging::LoggerFactory::getInstance().getLogger()->error(ctxt); }
void logging::critical(const except::Context& ctxt)
{ logging::LoggerFactory::getInstance().getLogger()->critical(ctxt); }
void logging::debug(except::Throwable& t)
{ logging::LoggerFactory::getInstance().getLogger()->debug(t); }
void logging::info(except::Throwable& t)
{ logging::LoggerFactory::getInstance().getLogger()->info(t); }
void logging::warn(except::Throwable& t)
{ logging::LoggerFactory::getInstance().getLogger()->warn(t); }
void logging::error(except::Throwable& t)
{ logging::LoggerFactory::getInstance().getLogger()->error(t); }
void logging::critical(except::Throwable& t)
{ logging::LoggerFactory::getInstance().getLogger()->critical(t); }

void logging::setLogLevel(logging::LogLevel level)
{
    //set the level of the root logger
    logging::LoggerFactory::getInstance().getLogger()->setLevel(level);
}

logging::Logger* logging::getLogger(std::string name)
{
    return logging::LoggerFactory::getInstance().getLogger(name);
}
