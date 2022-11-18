/* =========================================================================
 * This file is part of except-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
  * (C) Copyright 2021, Maxar Technologies, Inc.
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

#include <except/Backtrace.h>

#include <assert.h>

#include <cstdlib>
#include <sstream>

#if !CODA_OSS_except_Backtrace

static std::string getBacktrace_(bool& supported, std::vector<std::string>&)
{
    supported = true;
    return "except::getBacktrace() is not supported "
           "on the current platform and/or libc";
}

#else

#if defined(__GNUC__)
// https://man7.org/linux/man-pages/man3/backtrace.3.html
// "These functions are GNU extensions."

#include <execinfo.h>

constexpr size_t MAX_STACK_ENTRIES = 62;

namespace
{
//! RAII wrapper for stack symbols
struct BacktraceHelper final
{
    BacktraceHelper(char** stackSymbols)
        : mStackSymbols(stackSymbols)
    {}

    ~BacktraceHelper()
    {
        std::free(mStackSymbols);
    }

    std::string operator[](size_t idx) const
    {
        return mStackSymbols[idx];
    }
private:
    char** mStackSymbols;
};
}

static std::string getBacktrace_(bool& supported, std::vector<std::string>& symbolNames)
{
    supported = true;

    void* stackBuffer[MAX_STACK_ENTRIES];
    int currentStackSize = backtrace(stackBuffer, MAX_STACK_ENTRIES);
    BacktraceHelper stackSymbols(backtrace_symbols(stackBuffer,
                                                   currentStackSize));

    std::stringstream ss;
    for (int ii = 0; ii < currentStackSize; ++ii)
    {
        auto symbolName = stackSymbols[ii] + "\n"; 
        ss << symbolName;
        symbolNames.push_back(std::move(symbolName));
    }

    return ss.str();
}

#elif _WIN32
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp")

// https://docs.microsoft.com/en-us/windows/win32/api/dbghelp/nf-dbghelp-syminitialize
// "A process that calls SymInitialize should not call it again unless it calls SymCleanup first."
class SymInitialize_RAII final
{
    HANDLE process_;

public:
    bool result;
    SymInitialize_RAII(HANDLE process) : process_(process)
    {
        result = SymInitialize(process_, NULL, TRUE) == TRUE ? true : false;  // https://docs.microsoft.com/en-us/windows/win32/api/dbghelp/nf-dbghelp-syminitialize
    }

    ~SymInitialize_RAII()
    {
        result = SymCleanup(process_) == TRUE ? true : false; // https://docs.microsoft.com/en-us/windows/win32/api/dbghelp/nf-dbghelp-symcleanup
        assert(result);
    }
};

static std::string getBacktrace_(bool& supported, std::vector<std::string>& symbolNames)
{
    supported = true;

    // https://stackoverflow.com/a/5699483/8877
    const auto process = GetCurrentProcess();
    SymInitialize_RAII symInitialize(process);
    if (!symInitialize.result)
    {
        return "getBacktrace_(): SymInitialize() failed.";
    }

     PVOID stack[100];
     const auto frames = CaptureStackBackTrace(0, 100, stack, NULL);
     auto symbol = reinterpret_cast<PSYMBOL_INFO>(calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1));
     if (symbol == nullptr)
     {
         return "getBacktrace_(): calloc() failed.";
     }
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    std::string retval;
    for (unsigned int i = 0; i < frames; i++)
    {
        const auto address = reinterpret_cast<DWORD64>(stack[i]);
        const auto result = SymFromAddr(process, address, 0, symbol) == TRUE ? true : false;
        if (!result)
        {
            continue;
        }
        std::string symbolName = symbol->Name == nullptr ? "<no symbol->Name>" : symbol->Name;
        symbolName += "\n";
        retval += symbolName;
        symbolNames.push_back(std::move(symbolName));
    }

    free(symbol);
    return retval;
}

#else

#error "CODA_OSS_except_Backtrace inconsistency."

#endif
#endif // CODA_OSS_except_Backtrace

std::string except::getBacktrace(bool& supported, std::vector<std::string>& frames)
{
    return getBacktrace_(supported, frames);
}
