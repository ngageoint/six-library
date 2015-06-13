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

int main(int argc, char **argv)
{
    //create a logger, named 'test'
    Logger *logger = new Logger("test");

    //rotate every 1024 bytes, max of 5 backups
    RotatingFileHandler handler("./rotate.log", 1024, 5);

    logger->addHandler(&handler);

    int loops = 10;
    for (int i = 0; i < loops; ++i)
    {
        logger->warn(Ctxt("WARNING Test! "));
        logger->debug(Ctxt("DEBUG TEST! "));
        logger->info(Ctxt("INFO TEST! "));
        logger->critical(Ctxt("CRITICAL TEST! "));
    }

    delete logger;
    return 0;
}
