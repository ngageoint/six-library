/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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


#include "sys/Dbg.h"

#include <stdarg.h>

#include <config/compiler_extensions.h>

inline void va_end_(va_list& args) noexcept
{
    CODA_OSS_disable_warning_push
    #if _MSC_VER
    #pragma warning(disable : 26477)  // Use '...' rather than 0 or NULL(es .47).
    #endif

    va_end(args);

    CODA_OSS_disable_warning_pop
}

void sys::dbgPrintf(const char *format, ...) noexcept
{
    if (sys::debugging)
    {
        va_list args;
        va_start(args, format);
        fprintf(DEBUG_STREAM, "<DBG> ");
        vfprintf(DEBUG_STREAM, format, args);
        va_end_(args);
    }
}

void sys::diePrintf(const char* format, ...) noexcept
{
    va_list args;
    va_start(args, format);
    vfprintf(DEBUG_STREAM, format, args);
    va_end_(args);
    exit(EXIT_FAILURE);
}

bool sys::debug_build()
{
    return sys::debug;
}