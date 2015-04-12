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
        sys::DateTime now;
        std::cout << "Today is: " << now.getMonth() << "/"
                << now.getDayOfMonth() << "/" << now.getYear() << std::endl;
        std::cout << "The Time is: " << now.getHour() << ":" << now.getMinute()
                << ":" << std::setprecision(50) << now.getSecond() << std::endl;
        if (now.getDST())
            std::cout << "Daylight Savings Time is in effect" << std::endl;

        sys::RealTimeStopWatch sw;
        sys::CPUStopWatch csw;
        //std::cout << "CPS: " << CLOCKS_PER_SEC << std::endl;
        std::cout << "clock(): " << clock() / CLOCKS_PER_SEC << std::endl;
        std::cout << "time(): " << time(NULL) << std::endl;
        std::cout << "RTStart: " << sw.start() << std::endl;
        std::cout << "CStart: " << csw.start() << std::endl;
        for (int i = 0; i < 1000000000; ++i)
        {
            int x = 1 * 2 * 3;
        }
        std::cout << "Finish Loop 1" << std::endl;
        sw.pause();
        csw.pause();
        for (int i = 0; i < 1000000000; ++i)
        {
            int x = 1 * 2 * 3;
        }
        std::cout << "Finish Loop 2" << std::endl;
        //sw.clear();
        //csw.clear();
        sw.start();
        csw.start();

        for (int i = 0; i < 1000000000; ++i)
        {
            int x = 1 * 2 * 3;
        }
        std::cout << "Finish Loop 3" << std::endl;
        std::cout << "clock(): " << clock() / CLOCKS_PER_SEC << std::endl;
        std::cout << "time(): " << time(NULL) << std::endl;
        std::cout << "RTStop: " << std::setprecision(50) << sw.stop()
                << std::endl;
        std::cout << "CStop: " << std::setprecision(50) << csw.stop()
                << std::endl;

    }
    catch (except::Throwable& t)
    {
        std::cerr << "Caught throwable: " << t.toString() << std::endl;
        exit( EXIT_FAILURE);
    }
    catch (...)
    {
        std::cerr << "Caught unnamed exception" << std::endl;
    }
    return 0;
}
