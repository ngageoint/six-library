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

using namespace sys;
using namespace mt;

class Counter
{
public:
    Counter() : mCnt(0) { }
    ~Counter() { }
    void sayCount() { std::cout << "Count: " << ++mCnt << std::endl; }
protected:
    int mCnt;
};

typedef Singleton<Counter> AutoIncrementer;

int main (int argc, char *argv[])
{
    AutoIncrementer::getInstance().sayCount();
    AutoIncrementer::getInstance().sayCount();
    AutoIncrementer::getInstance().sayCount();
    AutoIncrementer::getInstance().sayCount();
    AutoIncrementer::getInstance().sayCount();
    AutoIncrementer::getInstance().sayCount();
    
    return 0;
}

#endif
