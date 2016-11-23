/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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

#include <sys/Backtrace.h>

#include <sys/sys_config.h>
#include <sstream>

static const size_t MAX_STACK_ENTRIES = 62;

#ifdef HAVE_EXECINFO_H

#include <execinfo.h>
#include <cstdlib>

namespace
{

//! RAII wrapper for stack symbols
class BacktraceHelper
{
public:
    BacktraceHelper(char** stackSymbols)
        : mStackSymbols(stackSymbols)
    {}

    ~BacktraceHelper()
    {
        std::free(mStackSymbols);
    }

    std::string operator[](size_t idx)
    {
        return mStackSymbols[idx];
    }
private:
    char** mStackSymbols;
};

}

std::string sys::getBacktrace()
{
    void* stackBuffer[MAX_STACK_ENTRIES];
    int currentStackSize = backtrace(stackBuffer, MAX_STACK_ENTRIES);
    BacktraceHelper stackSymbols(backtrace_symbols(stackBuffer, 
                                                   currentStackSize));

    std::stringstream ss;
    for (int ii = 0; ii < currentStackSize; ++ii)
    {
        ss << stackSymbols[ii] << std::endl;
    }

    return ss.str();
}

#else

std::string sys::getBacktrace()
{
    return "sys::getBacktrace() is not supported "
        "on the current platform and/or libc";
}

#endif

