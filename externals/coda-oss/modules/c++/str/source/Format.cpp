/* =========================================================================
 * This file is part of str-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * str-c++ is free software; you can redistribute it and/or modify
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


#include "str/Format.h"

#include <stdio.h>
#include <stdarg.h>

#include <config/compiler_extensions.h>

static std::string vformat(const char* format, va_list& args)
{
    char buffer[1024];
#if !_WIN32
    vsprintf(buffer, format, args);
#else
    vsprintf_s(buffer, format, args);
#endif
    return std::string(buffer);
}

inline void va_end_(va_list& args)
{
    CODA_OSS_disable_warning_push
    #if _MSC_VER
    #pragma warning(disable : 26477)  // Use '...' rather than 0 or NULL(es .47).
    #endif

    va_end(args);

    CODA_OSS_disable_warning_pop
}

std::string str::Format(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    auto result = vformat(format, args);
    va_end_(args);
    return result;
}
