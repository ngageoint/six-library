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

void sys::dbgPrintf(const char *format, ...)
{
    if (sys::debugging)
    {
        va_list args;
        va_start(args, format);
        fprintf(DEBUG_STREAM, "<DBG> ");
        vfprintf(DEBUG_STREAM, format, args);
        va_end(args);
    }
}

void sys::diePrintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(DEBUG_STREAM, format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

