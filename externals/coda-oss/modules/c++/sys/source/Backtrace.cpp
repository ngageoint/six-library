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

#include <cstdlib>
#include <sstream>

#include <str/Format.h>

#if !CODA_OSS_sys_Backtrace

static std::string getBacktrace(bool* pSupported, std::vector<std::string>*)
{
    if (pSupported != nullptr)
    {
        *pSupported = false;
    }
    return "sys::getBacktrace() is not supported "
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

static std::string getBacktrace(bool* pSupported, std::vector<std::string>* pFrames)
{
    if (pSupported != nullptr)
    {
        *pSupported = true;
    }

    void* stackBuffer[MAX_STACK_ENTRIES];
    int currentStackSize = backtrace(stackBuffer, MAX_STACK_ENTRIES);
    BacktraceHelper stackSymbols(backtrace_symbols(stackBuffer,
                                                   currentStackSize));

    std::stringstream ss;
    for (int ii = 0; ii < currentStackSize; ++ii)
    {
        auto stackSymbol = stackSymbols[ii] + "\n"; 
        ss << stackSymbol;
        if (pFrames != nullptr)
        {
            pFrames->push_back(std::move(stackSymbol));
        }
    }

    return ss.str();
}

#elif _WIN32
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp")

static std::string getBacktrace(bool* pSupported, std::vector<std::string>* pFrames)
{
    if (pSupported != nullptr)
    {
        *pSupported = true;
    }

    // https://stackoverflow.com/a/5699483/8877
    HANDLE process = GetCurrentProcess();
    auto result = SymInitialize(process, NULL, TRUE) == TRUE ? true : false; // https://docs.microsoft.com/en-us/windows/win32/api/dbghelp/nf-dbghelp-syminitialize
    if (!result)
    {
        return "sys::getBacktrace(): SymInitialize() failed";
    }

     PVOID stack[100];
     auto frames = CaptureStackBackTrace(0, 100, stack, NULL);
     auto symbol = reinterpret_cast<PSYMBOL_INFO>(calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1));
     if (symbol == nullptr)
     {
         return "sys::getBacktrace(): calloc() failed";
     }
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    std::string retval;
    for (unsigned int i = 0; i < frames; i++)
    {
        const auto address = reinterpret_cast<DWORD64>(stack[i]);
        result = SymFromAddr(process, address, 0, symbol) == TRUE ? true : false;
        if (!result)
        {
            continue;
        }
        auto frame = str::format("%i: %s - 0x%0X\n",
            frames - i - 1,
            symbol->Name,
            symbol->Address);
        retval += frame;
        if (pFrames != nullptr)
        {
            pFrames->push_back(std::move(frame));
        }
    }

    free(symbol);
    return retval;
}

#else

#error "CODA_OSS_sys_Backtrace inconsistency."

#endif
#endif // CODA_OSS_sys_Backtrace

std::string sys::getBacktrace(bool* pSupported)
{
    return ::getBacktrace(pSupported, nullptr /*frames*/);
}
std::string sys::getBacktrace(bool& supported, std::vector<std::string>& frames)
{
    return ::getBacktrace(&supported, &frames);
}
