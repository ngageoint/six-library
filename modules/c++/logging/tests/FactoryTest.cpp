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

#include <import/logging.h>
#include <import/io.h>
#include <import/sys.h>

using namespace logging;
using namespace std;

int main(int argc, char **argv)
{
    std::cout << LoggerFactory::getInstance().getLogger("test")->getName() << std::endl;

    Logger* logger = LoggerFactory::getInstance().getLogger("tomsLogger");
    logger->error("This should get logged to stdout");

    LoggerFactory::getInstance().getLogger()->info("A message from the root logger.");
    
    //by default, the loglevel is WARN, so these won't get logged
    logger->info("Info message 1");
    logger->info("Debug message 1");
    
    //you can set the loglevel of a particular logger like this:
    //keep in mind that this will set the level for all handlers of this Logger
    logger->setLevel(LOG_DEBUG);
    logger->info("Info message 2");
    logger->info("Debug message 2");
    
    
    //we can also set the default logging level, which is set statically
    //It will be used for any future loggers that get created on the fly
    //via the factory
    DefaultLogger::setDefaultLogLevel(LOG_DEBUG);
    logger = LoggerFactory::getInstance().getLogger("anotherLogger");
    logger->info("Info message 3");
    logger->info("Debug message 3");

    return 0;
}
