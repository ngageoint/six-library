/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

#include <import/sys.h>
#include <fstream>
#include <iomanip>
#include "sys/StopWatch.h"

using namespace sys;

int main(int argc, char **argv)
{
    try
    {
        std::string cmd;

        if (argc > 1)
            cmd = std::string(argv[1]);
        else
            cmd = "echo Scream!";

        sys::Exec exe(cmd);
        exe.run();
    }
    catch (const except::Throwable& ex)
    {
        std::cerr << "Caught C++ exception" << 
            ex.getMessage() << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Caught standard exception" << 
            ex.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Caught unnamed Unwanted exception" << std::endl;
    }
    return 0;
}

