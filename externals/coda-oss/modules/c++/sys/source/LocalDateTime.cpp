/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <errno.h>

#include <config/coda_oss_config.h>

#include <sys/LocalDateTime.h>

#include <sys/Conf.h>
#include <except/Exception.h>
#include <str/Convert.h>
#include <str/Manip.h>

namespace sys
{
const char LocalDateTime::DEFAULT_DATETIME_FORMAT[] = "%Y-%m-%d_%H:%M:%S";

void LocalDateTime::fromMillis(const tm& t)
{
    DateTime::fromMillis(t);
    mDST = t.tm_isdst;
}

void LocalDateTime::toMillis()
{
    tm t;
    t.tm_year = mYear - 1900;
    t.tm_mon = mMonth - 1;
    t.tm_mday = mDayOfMonth;
    t.tm_wday = mDayOfWeek - 1;
    t.tm_yday = mDayOfYear - 1;
    t.tm_hour = mHour;
    t.tm_min = mMinute;
    t.tm_sec = (int)mSecond;
    t.tm_isdst = mDST;

    mTimeInMillis = DateTime::toMillis(t);
}

void LocalDateTime::getTime(time_t numSecondsSinceEpoch, tm& t) const
{
    // Would like to use the reentrant version.  If we don't have one, cross
    // our fingers and hope the regular function actually is reentrant
    // (supposedly this is the case on Windows).
#ifdef _WIN32
    const auto errnum = ::localtime_s(&t, &numSecondsSinceEpoch);
    if (errnum != 0)
    {
        throw except::Exception(Ctxt("localtime_s() failed (" +
            std::string(::strerror(errnum)) + ")"));
    }
#elif defined(HAVE_LOCALTIME_R)
    if (::localtime_r(&numSecondsSinceEpoch, &t) == NULL)
    {
        int const errnum = errno;
        throw except::Exception(Ctxt("localtime_r() failed (" +
            std::string(::strerror(errnum)) + ")"));
    }
#else
    tm const * const localTimePtr = ::localtime(&numSecondsSinceEpoch);
    if (localTimePtr == NULL)
    {
        int const errnum = errno;
        throw except::Exception(Ctxt("localtime failed (" +
            std::string(::strerror(errnum)) + ")"));
    }
    t = *localTimePtr;
#endif
}

LocalDateTime::LocalDateTime() :
    mDST(-1) // Tell mktime() we're not sure
{
    setNow();
    toMillis();
}

LocalDateTime::LocalDateTime(int hour, int minute, double second) :
    mDST(-1) // Tell mktime() we're not sure
{
    setNow();

    mHour = hour;
    mMinute = minute;
    mSecond = second;

    toMillis();
}

LocalDateTime::LocalDateTime(int year, int month, int day) :
    mDST(-1) // Tell mktime() we're not sure
{
    mYear = year;
    mMonth = month;
    mDayOfMonth = day;

    toMillis();
    DateTime::fromMillis();
}

LocalDateTime::LocalDateTime(int year, int month, int day,
                             int hour, int minute, double second) :
    mDST(-1) // Tell mktime() we're not sure
{
    mYear = year;
    mMonth = month;
    mDayOfMonth = day;
    mHour = hour;
    mMinute = minute;
    mSecond = second;

    toMillis();
    DateTime::fromMillis();
}

LocalDateTime::LocalDateTime(double timeInMillis) :
    mDST(-1) // Tell mktime() we're not sure
{
    mTimeInMillis = timeInMillis;
    DateTime::fromMillis();
}

LocalDateTime::LocalDateTime(const std::string& time,
                             const std::string& format) :
    mDST(-1) // Tell mktime() we're not sure
{
    setTime(time, format);
    DateTime::fromMillis();
}

void LocalDateTime::setDST(bool isDST)
{
    if(isDST)
        mDST = 1;
    else
        mDST = 0;
}

std::string LocalDateTime::format() const
{
    return format(DEFAULT_DATETIME_FORMAT);
}

std::ostream& operator<<(std::ostream& os, const LocalDateTime& dateTime)
{
    os << dateTime.format().c_str();
    return os;
}

std::istream& operator>>(std::istream& is, LocalDateTime& dateTime)
{
    std::string str;
    is >> str;
    dateTime.setTime(str, LocalDateTime::DEFAULT_DATETIME_FORMAT);
    return is;
}
}
