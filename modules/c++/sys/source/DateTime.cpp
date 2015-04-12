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

#include "except/Exception.h"
#include "sys/DateTime.h"
#include "str/Convert.h"
#include "str/Manip.h"
#include <vector>

#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

void sys::DateTime::fromMillis()
{
    tm t;
    getTime(t);
    fromMillis(t);
}

void sys::DateTime::fromMillis(const tm& t)
{
    // this is year since 1900 so need to add that
    mYear = t.tm_year + 1900;
    // 0-based so add 1
    mMonth = t.tm_mon + 1;
    mDayOfMonth = t.tm_mday;
    mDayOfWeek = t.tm_wday + 1;
    mDayOfYear = t.tm_yday + 1;
    mHour = t.tm_hour;
    mMinute = t.tm_min;

    const size_t timeInSeconds = (size_t)(mTimeInMillis / 1000);
    const double timediff = ((double)mTimeInMillis / 1000.0) - timeInSeconds;
    mSecond = t.tm_sec + timediff;
}

double sys::DateTime::toMillis(tm t) const
{
    time_t timeInSeconds = mktime(&t);
    double timediff = mSecond - t.tm_sec;
    return (timeInSeconds + timediff) * 1000;
}

void sys::DateTime::setNow()
{
#if defined(__POSIX) && defined(USE_CLOCK_GETTIME)
    struct timespec now;
    clock_gettime(CLOCK_REALTIME,&now);
    mTimeInMillis = (now.tv_sec + 1.0e-9 * now.tv_nsec) * 1000;
#elif defined(HAVE_SYS_TIME_H)
    struct timeval now;
    gettimeofday(&now,NULL);
    mTimeInMillis = (now.tv_sec + 1.0e-6 * now.tv_usec) * 1000;
#elif defined(_WIN32)
    // Getting time twice may be inefficient but is quicker
    // than converting the SYSTEMTIME structure into
    // milliseconds
    // We could add an additional flag here if the user
    // does not need millisecond accuracy
    SYSTEMTIME now;
    GetLocalTime(&now);
    mTimeInMillis = (double)time(NULL) * 1000 + now.wMilliseconds;
#else
    mTimeInMillis = (double)time(NULL) * 1000;
#endif
    fromMillis();
}

void sys::DateTime::getTime(tm& t) const
{
    getTime(static_cast<time_t>(mTimeInMillis / 1000), t);
}

sys::DateTime::DateTime() :
    mYear(0),
    mMonth(0),
    mDayOfMonth(0),
    mDayOfWeek(0),
    mDayOfYear(0),
    mHour(0),
    mMinute(0),
    mSecond(0.0),
    mTimeInMillis(0.0)
{ }

sys::DateTime::~DateTime()
{}

std::string sys::DateTime::monthToString(int month)
{
    switch (month)
    {
        case 1: { return "January"; }break;
        case 2: { return "February"; }break;
        case 3: { return "March"; }break;
        case 4: { return "April"; }break;
        case 5: { return "May"; }break;
        case 6: { return "June"; }break;
        case 7: { return "July"; }break;
        case 8: { return "August"; }break;
        case 9: { return "September"; }break;
        case 10: { return "October"; }break;
        case 11: { return "November"; }break;
        case 12: { return "December"; }break;
        default: throw new except::InvalidArgumentException(
                        "Value not in the valid range {1:12}");
    }
}

std::string sys::DateTime::dayOfWeekToString(int dayOfWeek)
{
    switch (dayOfWeek)
    {
        case 1: { return "Sunday"; }break;
        case 2: { return "Monday"; }break;
        case 3: { return "Tuesday"; }break;
        case 4: { return "Wednesday"; }break;
        case 5: { return "Thursday"; }break;
        case 6: { return "Friday"; }break;
        case 7: { return "Saturday"; }break;
        default: throw new except::InvalidArgumentException(
                        "Value not in the valid range {1:7}");
    }
}

std::string sys::DateTime::monthToStringAbbr(int month)
{
    return monthToString(month).substr(0,3);
}

std::string sys::DateTime::dayOfWeekToStringAbbr(int dayOfWeek)
{
    return dayOfWeekToString(dayOfWeek).substr(0,3);
}

int monthToValue(const std::string& month)
{
    std::string m = month;
    str::lower(m);

    if (str::startsWith(m, "jan"))
        return 1;
    else if (str::startsWith(m, "feb"))
        return 2;
    else if (str::startsWith(m, "mar"))
        return 3;
    else if (str::startsWith(m, "apr"))
        return 4;
    else if (str::startsWith(m, "may"))
        return 5;
    else if (str::startsWith(m, "jun"))
        return 6;
    else if (str::startsWith(m, "jul"))
        return 7;
    else if (str::startsWith(m, "aug"))
        return 8;
    else if (str::startsWith(m, "sep"))
        return 9;
    else if (str::startsWith(m, "oct"))
        return 10;
    else if (str::startsWith(m, "nov"))
        return 11;
    else if (str::startsWith(m, "dec"))
        return 12;
    else
        throw new except::InvalidArgumentException(
                        "Value not in the valid range {Jan:Dec}");
}

int dayOfWeekToValue(const std::string& dayOfWeek)
{
    std::string d = dayOfWeek;
    str::lower(d);

    if (str::startsWith(d, "sun"))
        return 1;
    else if (str::startsWith(d, "mon"))
        return 2;
    else if (str::startsWith(d, "tue"))
        return 3;
    else if (str::startsWith(d, "wed"))
        return 4;
    else if (str::startsWith(d, "thu"))
        return 5;
    else if (str::startsWith(d, "fri"))
        return 6;
    else if (str::startsWith(d, "sat"))
        return 7;
    else
        throw new except::InvalidArgumentException(
                        "Value not in the valid range {Sun:Sat}");
}

void sys::DateTime::setDayOfMonth(int dayOfMonth)
{
    mDayOfMonth = dayOfMonth;
    toMillis();
}

void sys::DateTime::setMonth(int month)
{
    mMonth = month;
    toMillis();
}

void sys::DateTime::setHour(int hour)
{
    mHour = hour;
    toMillis();
}

void sys::DateTime::setMinute(int minute)
{
    mMinute = minute;
    toMillis();
}

void sys::DateTime::setSecond(double second)
{
    mSecond = second;
    toMillis();
}

void sys::DateTime::setTimeInMillis(double time)
{
    mTimeInMillis = time;
    fromMillis();
}

void sys::DateTime::setYear(int year)
{
    mYear = year;
    toMillis();
}

std::string sys::DateTime::format(const std::string& formatStr) const
{
    // the longest string expansion is 
    // %c => 'Thu Aug 23 14:55:02 2001' 
    // which is an expansion of 22 characters
    size_t maxSize = formatStr.length() * 22 + 1;
    std::vector<char> expanded(maxSize);
    char* str = &expanded[0];

    tm localTime;
    getTime(localTime);
    if (!strftime(str, maxSize, formatStr.c_str(), &localTime))
        throw except::InvalidFormatException(
            "The format string was unable to be expanded");

    return std::string(str);
}

