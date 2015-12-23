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

#include <import/logging.h>
#include <import/io.h>
#include <import/sys.h>

using namespace logging;
using namespace std;

int main(int, char**)
{
    //create a logger, named 'test'
    Logger logger;

    //log everything to the console
    StreamHandler handler(LogLevel::LOG_DEBUG);
    std::string
            format =
                    "Thread = %t, Name = %c, Level = %p, File = %F, Method = %M, Line = %L, TimeStamp = %d, Message = %m";
    handler.setFormatter(new StandardFormatter(format));
    logger.addHandler(&handler, true);

    //log only WARNING or worse to the file
    //FileHandler fileHandler("./test.log", LogLevel::LOG_WARNING);
    //logger.addHandler(&fileHandler);

    logger.warn(Ctxt("WARNING Test!"));
    logger.debug(Ctxt("DEBUG TEST!"));
    logger.info(Ctxt("INFO TEST!"));
    logger.critical(Ctxt("CRITICAL TEST!"));

    //use the global logging methods
    error("global error");
    warn("global warning");

    logger.removeHandler(&handler);
    logger.warn(Ctxt("No Handlers"));

    logger.addHandler(new NullHandler, true);
    logger.warn(Ctxt("Null Handler - should not log!"));
    logger.addHandler(new StreamHandler(LogLevel::LOG_DEBUG), true);
    logger.warn(Ctxt("WARNING Test!"));

    return 0;
}
