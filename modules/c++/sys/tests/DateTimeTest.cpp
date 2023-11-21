/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include "sys/LocalDateTime.h"
#include "sys/UTCDateTime.h"

using namespace sys;

int main(int, char**)
{
    try
    {
        sys::LocalDateTime now;
        std::cout << "Today is: " << now.getMonth() << "/"
                << now.getDayOfMonth() << "/" << now.getYear() << std::endl;
        std::cout << "Time is: " << now.getHour() << ":" << now.getMinute()
                << ":" << std::setprecision(50) << now.getSecond() << std::endl;
        if (now.getDST())
            std::cout << "Daylight Savings Time is in effect" << std::endl;
        std::cout << "formatted: " << now.format() << std::endl;

        sys::UTCDateTime gmnow;
        std::cout << "Today (UTC) is: " << gmnow.getMonth() << "/"
                << gmnow.getDayOfMonth() << "/" << gmnow.getYear() << std::endl;
        std::cout << "Time (UTC) is: " << gmnow.getHour() << ":"
            << gmnow.getMinute() << ":" << std::setprecision(50)
            << gmnow.getSecond() << std::endl;
        std::cout << "formatted: " << gmnow.format() << std::endl;

        sys::RealTimeStopWatch sw;
        sys::CPUStopWatch csw;
        //std::cout << "CPS: " << CLOCKS_PER_SEC << std::endl;
        std::cout << "clock(): " << clock() / CLOCKS_PER_SEC << std::endl;
        std::cout << "time(): " << time(nullptr) << std::endl;
        std::cout << "RTStart: " << sw.start() << std::endl;
        std::cout << "CStart: " << csw.start() << std::endl;
        int x = 0;
        for (int i = 0; i < 1000000000; ++i)
        {
            x = 1 * 2 * 3;
        }
        if (x > 0) { /*remove compiler warning*/ }
        std::cout << "Finish Loop 1" << std::endl;
        sw.pause();
        csw.pause();
        for (int i = 0; i < 1000000000; ++i)
        {
            x = 1 * 2 * 3;
        }
        std::cout << "Finish Loop 2" << std::endl;
        //sw.clear();
        //csw.clear();
        sw.start();
        csw.start();

        for (int i = 0; i < 1000000000; ++i)
        {
            x = 1 * 2 * 3;
        }
        std::cout << "Finish Loop 3" << std::endl;
        std::cout << "clock(): " << clock() / CLOCKS_PER_SEC << std::endl;
        std::cout << "time(): " << time(nullptr) << std::endl;
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
