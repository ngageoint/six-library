/* =========================================================================
 * This file is part of mt-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * mt-c++ is free software; you can redistribute it and/or modify
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

#if defined(__APPLE_CC__)
#include <iostream>
int main (int argc, char *argv[])
{
    std::cout << "Sorry no semaphores" << std::endl;
    return 0;
}
#else

#include <iostream>
#include <import/sys.h>
#include <import/mt.h>
#include <sstream>

using namespace sys;
using namespace mt;

class Logger
{
private:
    int num;
    sys::Mutex mutex;
public:
    Logger():num(0){ std::cout << "In Logger Constructor." << std::endl; }
    ~Logger(){ std::cout << "In Logger Destructor." << std::endl; }
    void log(std::string msg)
    {
        mt::CriticalSection<sys::Mutex> obtainLock(&mutex);
        std::cout << "INFO[" << num++ << "]: " << msg << std::endl;
    }
};
typedef Singleton<Logger, true> LoggerFactory; //this gets deleted at exit


class Counter
{
public:
    Counter() : mCnt(0) { std::cout << "In Counter Constructor." << std::endl; }
    ~Counter()
    {
        LoggerFactory::getInstance().log("In Counter Destructor.");
    }
    void sayCount()
    {
        std::ostringstream os;
        os << "Count: " << ++mCnt;
        LoggerFactory::getInstance().log(os.str());
    }
protected:
    int mCnt;
};
typedef Singleton<Counter> AutoIncrementer; //this does NOT get deleted at exit


int main (int argc, char *argv[])
{
    AutoIncrementer::getInstance().sayCount();
    AutoIncrementer::getInstance().sayCount();
    AutoIncrementer::getInstance().sayCount();
    AutoIncrementer::getInstance().sayCount();
    AutoIncrementer::getInstance().sayCount();
    AutoIncrementer::getInstance().sayCount();
    LoggerFactory::getInstance().log("Main end");
    AutoIncrementer::destroy();
    return 0;
}

#endif

