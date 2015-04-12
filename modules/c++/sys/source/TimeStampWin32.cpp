/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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


#if defined(_WIN32) || defined(WIN32)

#include "sys/TimeStamp.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

const char* sys::TimeStamp::getFormat()
{
    // What is strftime for windows??
    //if (m24HourTime) { return "%m/%d/%Y, %H:%M:%S"; }
    //return "%m/%d/%Y, %I:%M:%S%p";
    return "%02d/%02d/%02d, %02d:%02d:%02d";
}

std::string sys::TimeStamp::gmt()
{
    SYSTEMTIME gmTime;
    GetSystemTime(&gmTime);

    return str::format(getFormat(), gmTime.wMonth, gmTime.wDay, gmTime.wYear,
                       gmTime.wHour, gmTime.wMinute, gmTime.wSecond);

}

std::string sys::TimeStamp::local()
{
    SYSTEMTIME localTime;
    GetLocalTime(&localTime);
    return str::format(getFormat(), localTime.wMonth, localTime.wDay, localTime.wYear,
                       localTime.wHour, localTime.wMinute, localTime.wSecond);

}

#endif
