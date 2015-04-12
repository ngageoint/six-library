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


#if !defined(WIN32)
#include "sys/TimeStamp.h"


std::string sys::TimeStamp::local()
{
    time_t __time = time(0);
    char timeStamp[MAX_TIME_STAMP];
    strftime(timeStamp, MAX_TIME_STAMP - 1,
             getFormat(), localtime(&__time));
    return timeStamp;
}
std::string sys::TimeStamp::gmt()
{
    time_t __time = time(0);
    char timeStamp[MAX_TIME_STAMP];
    strftime(timeStamp, MAX_TIME_STAMP - 1,
             getFormat(), localtime(&__time));
    return timeStamp;
}
const char* sys::TimeStamp::getFormat()
{
    if (m24HourTime)
    {
        return "%m/%d/%Y, %H:%M:%S";
    }
    return "%m/%d/%Y, %I:%M:%S%p";
}
#endif

