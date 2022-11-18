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

std::string str::format(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    auto retval = vformat(format, args);
    va_end(args);
    return retval;
}

str::Format::Format(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    auto result = vformat(format, args);
    va_end(args);
    mString = std::move(result);
}
