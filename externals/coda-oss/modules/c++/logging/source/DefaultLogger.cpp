/* =========================================================================
 * This file is part of logging-c++
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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
//  DefaultLogger.cpp
///////////////////////////////////////////////////////////

#include "logging/DefaultLogger.h"

    
logging::LogLevel logging::DefaultLogger::defaultLogLevel = logging::LogLevel::LOG_WARNING;


logging::DefaultLogger::DefaultLogger(std::string name)
    : logging::Logger(name)
{
    //TODO might be able to share just one amongst all DefaultLoggers -- just a thought
    mDefaultHandler = new logging::StreamHandler(defaultLogLevel);
    this->addHandler(mDefaultHandler);
}

logging::DefaultLogger::~DefaultLogger()
{
    if (mDefaultHandler)
    {
        this->removeHandler(mDefaultHandler);
        delete mDefaultHandler;
    }
}


void logging::DefaultLogger::setDefaultLogLevel(logging::LogLevel logLevel)
{
    logging::DefaultLogger::defaultLogLevel = logLevel;
}


