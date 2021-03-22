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
#include "sys/DateTime.h"

#include <ctype.h>
#include <errno.h>

#include <vector>
#include <mutex>
#include <stdexcept>

#include <config/coda_oss_config.h>
#include "except/Exception.h"
#include "sys/Conf.h"
#include "str/Convert.h"
#include "str/Manip.h"

#if CODA_OSS_POSIX_SOURCE
#include <sys/time.h>
#elif _WIN32
#include <windows.h>
#endif

namespace
{
#define TM_YEAR_BASE 1900

bool conv_num(const char*& buf, int& result, int llim, int ulim)
{
    result = 0;

    // The limit also determines the number of valid digits.
    int rulim = ulim;
    std::string numStr("");

    do
    {
        if (!isdigit(*buf))
            return false;

        numStr.push_back(*buf++);
        rulim /= 10;
    }
    while (rulim);

    result = str::toType<int>(numStr);

    return (result >= llim && result <= ulim);
}

// http://social.msdn.microsoft.com/forums/en-US/vcgeneral/thread/25a654f9-b6b6-490a-8f36-c87483bb36b7

char* strptime(const char *buf, const char *fmt, struct tm& tm, double& millis)
{
    const std::string DAY[7] = {
        "sunday", "monday", "tuesday", "wednesday",
        "thursday", "friday", "saturday"
    };
    const std::string AB_DAY[7] = {
        "sun", "mon", "tue", "wed", "thu", "fri", "sat"
    };

    const std::string MONTH[12] = {
        "january", "february", "march", "april", "may", "june",
        "july", "august", "september", "october", "november", "december"
    };
    const std::string AB_MONTH[12] = {
        "jan", "feb", "mar", "apr", "may", "jun",
        "jul", "aug", "sep", "oct", "nov", "dec"
    };

    char bc, fc;
    size_t len = 0;
    int i, split_year = 0;

    const char* bp = buf;

    // Eat up initial white-space.
    while (isspace(*bp))
        bp++;

    while (*bp != '\0' && (fc = *fmt) != '\0')
    {
        if ((fc = *fmt++) != '%')
        {
            bc = *bp++;
            if (bc != fc)
                throw except::Exception(Ctxt(
                        "Value does not match format (" + str::toString(fc) +
                        "):  " + str::toString(bc)));
            continue;
        }

        // "Alternative" modifiers. Ignore for now and continue.
        if (*fmt == 'E' || *fmt == 'O')
        {
            ++fmt;
        }

        switch (fc = *fmt++)
        {
        case '%':              // "%%" is converted to "%".
            bc = *bp++;
            if (bc != '%')
                throw except::Exception(Ctxt(
                        "Value does not match format (%%):  " + bc));
            break;

        /*
         * "Complex" conversion rules, implemented through recursion.
         */
        case 'c':              // Date and time, using the locale's format.
            bp = strptime(bp, "%x %X", tm, millis);
            break;

        case 'D':              // The date as "%m/%d/%y".
            bp = strptime(bp, "%m/%d/%y", tm, millis);
            break;

        case 'R':              // The time as "%H:%M".
            bp = strptime(bp, "%H:%M", tm, millis);
            break;

        case 'r':              // The time in 12-hour clock representation.
            bp = strptime(bp, "%I:%M:%S %p", tm, millis);
            break;

        case 'T':              // The time as "%H:%M:%S".
            bp = strptime(bp, "%H:%M:%S", tm, millis);
            break;

        case 'X':              // The time, using the locale's format.
            bp = strptime(bp, "%H:%M:%S", tm, millis);
            break;

        case 'x':              // The date, using the locale's format.
            bp = strptime(bp, "%m/%d/%y", tm, millis);
            break;

        /*
         * "Elementary" conversion rules.
         */
        case 'A':              // The day of week, using the locale's form.
        case 'a':
            /*
             *  The tm structure does not use this information to represent
             *  the real date.  For now, parse any valid value, but
             *  recalculate day from the rest of the date string.
             */
            for (i = 0; i < 7; i++)
            {
                // Full name.
                len = DAY[i].size();
                std::string day(bp, len);
                str::lower(day);
                if (day == DAY[i])
                    break;

                // Abbreviated name.
                len = AB_DAY[i].size();
                day = std::string(bp, len);
                str::lower(day);
                if (day == AB_DAY[i])
                    break;
            }

            // Nothing matched.
            if (i == 7)
                throw except::Exception(Ctxt("Invalid day of week"));

            tm.tm_wday = i;
            bp += len;
            break;

        case 'B':              // The month, using the locale's form.
        case 'b':
        case 'h':
            /*
             *  The tm structure does not use this information to represent
             *  the real date.  For now, parse any valid value, but
             *  recalculate month from the rest of the date string.
             */
            for (i = 0; i < 12; i++)
            {
                // Full name.
                len = MONTH[i].size();
                std::string month(bp, len);
                str::lower(month);
                if (month == MONTH[i])
                    break;

                // Abbreviated name.
                len = AB_MONTH[i].size();
                month = std::string(bp, len);
                str::lower(month);
                if (month == AB_MONTH[i])
                    break;
            }

            // Nothing matched.
            if (i == 12)
                throw except::Exception(Ctxt("Invalid month"));

            tm.tm_mon = i;
            bp += len;
            break;

        case 'C':              // The century number.
            if (!(conv_num(bp, i, 0, 99)))
                throw except::Exception(Ctxt("Invalid year"));

            if (split_year)
            {
                tm.tm_year = (tm.tm_year % 100) + (i * 100);
            }
            else
            {
                tm.tm_year = i * 100;
                split_year = 1;
            }
            break;

        case 'd':              // The day of month.
        case 'e':
            if (!(conv_num(bp, tm.tm_mday, 1, 31)))
                throw except::Exception(Ctxt("Invalid day of month"));
            break;

        case 'k':              // The hour (24-hour clock representation).
        case 'H':
            if (!(conv_num(bp, tm.tm_hour, 0, 23)))
                throw except::Exception(Ctxt("Invalid time"));
            break;

        case 'l':              // The hour (12-hour clock representation).
        case 'I':
            if (!(conv_num(bp, tm.tm_hour, 1, 12)))
                throw except::Exception(Ctxt("Invalid time"));
            if (tm.tm_hour == 12)
                tm.tm_hour = 0;
            break;

        case 'j':              // The day of year.
            if (!(conv_num(bp, i, 1, 366)))
                throw except::Exception(Ctxt("Invalid day of year"));
            tm.tm_yday = i - 1;
            break;

        case 'M':              // The minute.
            if (!(conv_num(bp, tm.tm_min, 0, 59)))
                throw except::Exception(Ctxt("Invalid minutes"));
            break;

        case 'm':              // The month.
            if (!(conv_num(bp, i, 1, 12)))
                throw except::Exception(Ctxt("Invalid month"));
            tm.tm_mon = i - 1;
            break;

        case 'S':              // The seconds.
            if (!(conv_num(bp, tm.tm_sec, 0, 61)))
                throw except::Exception(Ctxt("Invalid seconds"));

            // Determine if the next character is a decimal...
            if (*bp == '.')
            {
                std::string milliStr("0.");
                bp++;

                // Get the fractional seconds value
                while (isdigit(*bp))
                {
                    milliStr.push_back(*bp++);
                }

                millis = str::toType<double>(milliStr) * 1000;
            }
            break;

        case 'U':              // The week of year, beginning on sunday.
        case 'W':              // The week of year, beginning on monday.
            /*
             * XXX This is bogus, as we can not assume any valid
             * information present in the tm structure at this
             * point to calculate a real value, so just check the
             * range for now.
             */
            if (!(conv_num(bp, i, 0, 53)))
                throw except::Exception(Ctxt("Invalid week of year"));
            break;

        case 'w':              // The day of week, beginning on sunday.
            if (!(conv_num(bp, tm.tm_wday, 0, 6)))
                throw except::Exception(Ctxt("Invalid day of week"));
            break;

        case 'Y':              // The year.
            i = TM_YEAR_BASE;
            if (!(conv_num(bp, i, 0, 9999)))
                throw except::Exception(Ctxt("Invalid year: " + str::toString(i)));
            tm.tm_year = i - TM_YEAR_BASE;
            break;

        case 'y':              // The year within 100 years of the epoch.
            if (!(conv_num(bp, i, 0, 99)))
                throw except::Exception(Ctxt("Invalid year"));

            if (split_year)
            {
                tm.tm_year = ((tm.tm_year / 100) * 100) + i;
                break;
            }
            split_year = 1;
            if (i <= 68)
                tm.tm_year = i + 2000 - TM_YEAR_BASE;
            else
                tm.tm_year = i + 1900 - TM_YEAR_BASE;
            break;

        case 'n':              // Any kind of white-space.
        case 't':
            while (isspace(*bp))
                bp++;
            break;

        default:               // Unknown/unsupported conversion.
            throw except::Exception(Ctxt(
                    "Unknown/unsupported format type:  %" + fc));
        }
    }

    // LINTED functional specification
    return ((char *) bp);
};
}

void sys::DateTime::fromMillis()
{
    tm t;
    getTime(t);
    fromMillis(t);
}

void sys::DateTime::fromMillis(const tm& t)
{
    mYear = t.tm_year + TM_YEAR_BASE;
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

static double getNowInMillis()
{
    // https://linux.die.net/man/2/gettimeofday
    // "SVr4, 4.3BSD. POSIX.1-2001 describes gettimeofday() ... POSIX.1-2008 marks
    // gettimeofday() as obsolete, recommending the use of clock_gettime(2) instead."
#if CODA_OSS_POSIX2008_SOURCE
    struct timespec now;
    clock_gettime(CLOCK_REALTIME,&now);
    return (now.tv_sec + 1.0e-9 * now.tv_nsec) * 1000;
#elif CODA_OSS_POSIX_SOURCE
    struct timeval now;
    gettimeofday(&now,NULL);
    return (now.tv_sec + 1.0e-6 * now.tv_usec) * 1000;
#elif _WIN32
    // Getting time twice may be inefficient but is quicker
    // than converting the SYSTEMTIME structure into
    // milliseconds
    // We could add an additional flag here if the user
    // does not need millisecond accuracy
    SYSTEMTIME now;
    GetLocalTime(&now);
    return (double)time(NULL) * 1000 + now.wMilliseconds;
#else
    return (double)time(NULL) * 1000;
#endif
}
void sys::DateTime::setNow()
{
    mTimeInMillis = getNowInMillis();
    fromMillis();
}

void sys::DateTime::getTime(tm& t) const
{
    getTime(static_cast<time_t>(mTimeInMillis / 1000), t);
}

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
        default: throw except::InvalidArgumentException(
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
        default: throw except::InvalidArgumentException(
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

int sys::DateTime::monthToValue(const std::string& month)
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
        throw except::InvalidArgumentException(
                        "Value not in the valid range {Jan:Dec}");
}

int sys::DateTime::dayOfWeekToValue(const std::string& dayOfWeek)
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
        throw except::InvalidArgumentException(
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

void sys::DateTime::setTime(const std::string& time, const std::string& format)
{
    // init
    struct tm t;
    t.tm_sec = t.tm_min = t.tm_hour = t.tm_mday = t.tm_mon =
            t.tm_year = t.tm_wday = t.tm_yday = 0;
    t.tm_isdst = -1;

    strptime(time.c_str(), format.c_str(), t, mTimeInMillis);
    fromMillis(t);
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

// https://en.cppreference.com/w/c/chrono/localtime
// "The structure may be shared between gmtime, localtime, and ctime ... ."
static std::mutex g_dateTimeMutex;
template<typename F>
static inline int time_s(F f, tm* t, const time_t* numSecondsSinceEpoch)
{
    tm* result = nullptr;
    {
        std::lock_guard<std::mutex> guard(g_dateTimeMutex);
        result = f(numSecondsSinceEpoch);
    }
    if (result == nullptr)
    {
        return errno;
    }

    *t = *result;
    return 0; // no error
}
int sys::details::localtime_s(tm* t, const time_t* numSecondsSinceEpoch)
{
    #if CODA_OSS_POSIX_SOURCE || _WIN32
    (void)t; (void)numSecondsSinceEpoch;
    throw std::logic_error("Should be calling OS-specific routine.");
    #else
    return time_s(localtime, t, numSecondsSinceEpoch);
    #endif
}
int sys::details::gmtime_s(tm* t, const time_t* numSecondsSinceEpoch)
{
    #if CODA_OSS_POSIX_SOURCE || _WIN32
    (void)t; (void)numSecondsSinceEpoch;
    throw std::logic_error("Should be calling OS-specific routine.");
    #else
    return time_s(gmtime, t, numSecondsSinceEpoch);
    #endif
}

void sys::DateTime::localtime(time_t numSecondsSinceEpoch, tm& t)
{
    // Would like to use the reentrant version.  If we don't have one, cross
    // our fingers and hope the regular function actually is reentrant
    // (supposedly this is the case on Windows).
#if CODA_OSS_POSIX_SOURCE
    if (::localtime_r(&numSecondsSinceEpoch, &t) == NULL)
    {
        int const errnum = errno;
        throw except::Exception(Ctxt("localtime_r() failed (" +
            std::string(::strerror(errnum)) + ")"));
    }
#elif _WIN32
    const auto errnum = ::localtime_s(&t, &numSecondsSinceEpoch);
    if (errnum != 0)
    {
        char buffer[1024];
        strerror_s(buffer, errnum);
        throw except::Exception(Ctxt("localtime_s() failed (" + std::string(buffer) + ")"));
    }
#else
    const auto errnum = sys::details::localtime_s(&t, &numSecondsSinceEpoch);
    if (errnum != 0)
    {
        throw except::Exception(Ctxt("localtime failed (" +
            std::string(::strerror(errnum)) + ")"));
    }
#endif
}

void sys::DateTime::gmtime(time_t numSecondsSinceEpoch, tm& t)
{
    // Would like to use the reentrant version.  If we don't have one, cross
    // our fingers and hope the regular function actually is reentrant
    // (supposedly this is the case on Windows).
#if CODA_OSS_POSIX_SOURCE
        if (::gmtime_r(&numSecondsSinceEpoch, &t) == NULL)
    {
        int const errnum = errno;
        throw except::Exception(Ctxt("gmtime_r() failed (" +
            std::string(::strerror(errnum)) + ")"));
    }
#elif _WIN32
    const auto errnum = ::gmtime_s(&t, &numSecondsSinceEpoch);
    if (errnum != 0)
    {
        char buffer[1024];
        strerror_s(buffer, errnum);
        throw except::Exception(Ctxt("gmtime_s() failed (" + std::string(buffer) + ")"));
    }
#else
    const auto errnum = sys::details::gmtime_s(&t, &numSecondsSinceEpoch);
    if (errnum != 0)
    {
        throw except::Exception(Ctxt("gmtime failed (" +
            std::string(::strerror(errnum)) + ")"));
    }
#endif
}

int64_t sys::DateTime::getEpochSeconds() noexcept
{
    // https://en.cppreference.com/w/cpp/chrono/c/time_t
    // Although not defined, this is almost always an integral value holding the number of seconds (not counting leap seconds)
    // since 00:00, Jan 1 1970 UTC, corresponding to POSIX time.
    // https://en.cppreference.com/w/cpp/chrono/c/time
    const time_t result = std::time(nullptr);
    size_t sizeof_time_t = sizeof(time_t); // "conditional expression is constant"
    if (sizeof_time_t == sizeof(int64_t))
    {
        return static_cast<int64_t>(result);
    }
    if (sizeof_time_t == sizeof(int32_t))
    {
        return static_cast<int32_t>(result);
    }
    static_assert(sizeof(time_t) >= sizeof(int32_t), "should have at least a 32-bit time_t");
    return -1;
}
