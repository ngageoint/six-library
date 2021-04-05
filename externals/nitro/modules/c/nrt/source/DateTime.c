/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef _MSC_VER
#pragma warning(disable: 4820) // '...': '...' bytes padding added after data member '...'
#endif

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "nrt/DateTime.h"

#define NRT_YEAR0 1900
#define NRT_EPOCH_YEAR 1970 /* EPOCH = Jan 1 1970 00:00:00 */

/* At the end of each month, the total number of days so far in the year.
 * Index 0 is for non-leap years, index 1 is for leap years */
const int NRT_CUMULATIVE_DAYS_PER_MONTH[2][12] =
{
    {31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
    {31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}
};

/* The number of days in a year.  Index 0 is for non-leap years, index 1 is
 * for leap years */
const int NRT_DAYS_PER_YEAR[2] = {365, 366};

/* Returns the appropriate index into NRT_CUMULATIVE_DAYS_PER_MONTH based on
 * whether 'year' is a leap year or not */
NRTPRIV(int) nrtYearIndex(int year)
{
    return (!(year % 4) && ((year % 100) || !(year % 400)));
}

/* Returns the # of full days so far in the year
 * 'month' and 'dayOfMonth' are 1-based
 * So, nrtGetNumFullDaysInYearSoFar(2000, 1, 1) = 0
 *     nrtGetNumFullDaysInYearSoFar(2000, 1, 2) = 1
 *     nrtGetNumFullDaysInYearSoFar(2000, 2, 1) = 31
 */
NRTPRIV(int) nrtGetNumFullDaysInYearSoFar(int year, int month, int dayOfMonth)
{
    /* The number of days for all the full months so far */
    int numFullDays = (month > 1) ?
        NRT_CUMULATIVE_DAYS_PER_MONTH[nrtYearIndex(year)][month - 2] :
        0;

    /* The number of full days in this month so far */
    numFullDays += dayOfMonth - 1;
    return numFullDays;
}

NRTPRIV(char *) _NRT_strptime(const char *buf, const char *fmt, struct tm *tm,
                              double *millis);

NRTAPI(nrt_DateTime *) nrt_DateTime_now(nrt_Error * error)
{
    return nrt_DateTime_fromMillis(nrt_Utils_getCurrentTimeMillis(), error);
}

NRTAPI(nrt_DateTime *) nrt_DateTime_fromMillis(double millis, nrt_Error * error)
{
    nrt_DateTime *dt = NULL;

    dt = (nrt_DateTime *) NRT_MALLOC(sizeof(nrt_DateTime));
    if (!dt)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        return NULL;
    }

    nrt_DateTime_setTimeInMillis(dt, millis, error);

    return dt;
}

NRTPRIV(NRT_BOOL) nrt_DateTime_setMonthInfoFromDayOfYear(int year,
                                                         int dayOfYear,
                                                         int *month,
                                                         int *dayOfMonth)
{
    int yearIndex;
    int monthIndex;
    int lastMonthDays = 0;

    if (year < 1970 || year > 2037 || dayOfYear < 1 ||
            dayOfYear > NRT_DAYS_PER_YEAR[nrtYearIndex(year)])
    {
        return NRT_FAILURE;
    }

    yearIndex = nrtYearIndex(year);

    /* Find the entry in cumulative days per month where the day of the
     * year fits. */
    for (monthIndex = 0; monthIndex < 12; ++monthIndex)
    {
        int nextMonthDays =
                NRT_CUMULATIVE_DAYS_PER_MONTH[yearIndex][monthIndex];

        if (dayOfYear <= nextMonthDays)
        {
            /* Get the offset into the month */
            *dayOfMonth = dayOfYear - lastMonthDays;
            *month = monthIndex + 1;
            break;
        }

        lastMonthDays = nextMonthDays;
    }

    return NRT_SUCCESS;
}

NRTPRIV(NRT_BOOL) nrt_DateTime_updateMillis(nrt_DateTime* dateTime,
                                            nrt_Error* error)
{
    long numDaysThisYear;
    long numDaysSinceEpoch;
    int year;

    /* Silence compiler warnings about unused variables */
    (void)error;

    /* Sanity checks.  If things aren't valid, just set timeInMillis to 0. */
    /* TODO: Not sure if we should error out instead.  The advantage of this
     *       approach is if a caller is gradually setting fields, it won't
     *       error out, and once they have set all 6 fields, the struct will
     *       be in a valid state */
    if (dateTime->second < 0.0 || dateTime->second >= 60.0 ||
        dateTime->minute > 59 ||
        dateTime->hour > 23 ||
        dateTime->dayOfMonth < 1 || dateTime->dayOfMonth > 31 ||
        dateTime->month < 1 || dateTime->month > 12 ||
        dateTime->year < 1970 || dateTime->year > 2037)
    {
        dateTime->timeInMillis = 0.0;
        dateTime->dayOfYear = dateTime->dayOfWeek = 0;
        return NRT_SUCCESS;
    }

    /* Essentially we are implementing a simplified variant of mktime() here.
     * Implementation loosely based on
     * http://www.raspberryginger.com/jbailey/minix/html/mktime_8c-source.html
     * The problem with mktime() is that it expects local time and we are in
     * GMT.  Note that we can't just call mktime(), then look at the
     * difference between localtime() and gmtime() and offset the result by
     * that amount because this approach can't reliably take daylight savings
     * time into account.  Another option would be to trick mktime() by
     * setting the TZ environment variable to UTC, but this wouldn't be
     * reentrant.
     * It is very unfortunate that there's no POSIX standard function similar
     * to mktime() that allows you to pass in the timezone you want.
     * */

    /* Count up the # of days this year */
    numDaysThisYear = nrtGetNumFullDaysInYearSoFar(dateTime->year,
                                                   dateTime->month,
                                                   dateTime->dayOfMonth);

    /* Count up the # of days for all the years prior to this one
     * TODO: This could be implemented more efficiently - see reference
     * implementation above. */
    numDaysSinceEpoch = 0;
    for (year = NRT_EPOCH_YEAR; year < dateTime->year; ++year)
    {
        numDaysSinceEpoch += NRT_DAYS_PER_YEAR[nrtYearIndex(year)];
    }
    numDaysSinceEpoch += numDaysThisYear;

    dateTime->timeInMillis =
        (dateTime->second +
         dateTime->minute * 60.0 +
         dateTime->hour * (60.0 * 60.0) +
         numDaysSinceEpoch * (60.0 * 60.0 * 24.0)) * 1000.0;

    dateTime->dayOfYear = numDaysThisYear + 1;

    /* January 1, 1970 was a Thursday (5) */
    dateTime->dayOfWeek = (numDaysSinceEpoch + 5) % 7;

    return NRT_SUCCESS;
}

NRTAPI(NRT_BOOL) nrt_DateTime_setYear(nrt_DateTime * dateTime, int year,
                                      nrt_Error * error)
{
    dateTime->year = year;
    return nrt_DateTime_updateMillis(dateTime, error);
}

NRTAPI(NRT_BOOL) nrt_DateTime_setMonth(nrt_DateTime * dateTime, int month,
                                       nrt_Error * error)
{
    dateTime->month = month;
    return nrt_DateTime_updateMillis(dateTime, error);
}

NRTAPI(NRT_BOOL) nrt_DateTime_setDayOfMonth(nrt_DateTime * dateTime,
                                            int dayOfMonth, nrt_Error * error)
{
    dateTime->dayOfMonth = dayOfMonth;
    return nrt_DateTime_updateMillis(dateTime, error);
}

NRTAPI(NRT_BOOL) nrt_DateTime_setHour(nrt_DateTime * dateTime, int hour,
                                      nrt_Error * error)
{
    dateTime->hour = hour;
    return nrt_DateTime_updateMillis(dateTime, error);
}

NRTAPI(NRT_BOOL) nrt_DateTime_setMinute(nrt_DateTime * dateTime, int minute,
                                        nrt_Error * error)
{
    dateTime->minute = minute;
    return nrt_DateTime_updateMillis(dateTime, error);
}

NRTAPI(NRT_BOOL) nrt_DateTime_setSecond(nrt_DateTime * dateTime, double second,
                                        nrt_Error * error)
{
    dateTime->second = second;
    return nrt_DateTime_updateMillis(dateTime, error);
}

NRTAPI(NRT_BOOL) nrt_DateTime_setDayOfYear(nrt_DateTime * dateTime,
                                           int dayOfYear,
                                           nrt_Error * error)
{
    int month=0, dayOfMonth=0;
    if (nrt_DateTime_setMonthInfoFromDayOfYear(dateTime->year,
                                               dayOfYear,
                                               &month,
                                               &dayOfMonth))
    {
        dateTime->dayOfYear = dayOfYear;
        dateTime->month = month;
        dateTime->dayOfMonth = dayOfMonth;

        return nrt_DateTime_updateMillis(dateTime, error);
    }

    return NRT_FAILURE;
}

static void nrt_DateTime_gmtime_s(struct tm* result, const time_t* const time)
{
#ifdef _WIN32
    gmtime_s(result, time);
#else
    // https://linux.die.net/man/3/gmtime_r
    gmtime_r(time, result);
#endif
}
#define gmtime_s(result, time) nrt_DateTime_gmtime_s(result, time)

NRTAPI(NRT_BOOL) nrt_DateTime_setTimeInMillis(nrt_DateTime * dateTime,
                                              double timeInMillis,
                                              nrt_Error * error)
{
    time_t timeInSeconds;
    struct tm t;

    /* Silence compiler warnings about unused variables */
    (void)error;

    timeInSeconds = (time_t) (timeInMillis / 1000);
    gmtime_s(&t, &timeInSeconds);

    dateTime->timeInMillis = timeInMillis;

    /* this is the year since 1900 */
    dateTime->year = t.tm_year + 1900;

    /* 0-based so add 1 */
    dateTime->month = t.tm_mon + 1;
    dateTime->dayOfMonth = t.tm_mday;
    dateTime->dayOfWeek = t.tm_wday + 1;
    dateTime->dayOfYear = t.tm_yday + 1;
    dateTime->hour = t.tm_hour;
    dateTime->minute = t.tm_min;
    dateTime->second = t.tm_sec + (timeInMillis / 1000.0 - timeInSeconds);

    return NRT_SUCCESS;
}

NRTAPI(nrt_DateTime *) nrt_DateTime_fromString(const char *string,
                                               const char *format,
                                               nrt_Error * error)
{
    struct tm t;
    nrt_DateTime *dateTime = NULL;
    double millis = 0.0;

    /* NOTE: _NRT_strptime() does not use the tm_isdst flag at all. */
    t.tm_isdst = -1;

    if (!_NRT_strptime(string, format, &t, &millis))
    {
        nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INVALID_OBJECT,
                        "Unknown error caused by the call to strptime with string [%s] and format string [%s]",
                        string, format);
        return NULL;
    }

    /* Create a DateTime object */
    dateTime = (nrt_DateTime *) NRT_MALLOC(sizeof(nrt_DateTime));
    if (!dateTime)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        return NULL;
    }

    /* Initialize it from the tm struct
     * TODO: Update _NRT_strptime() to just use a DateTime directly */
    dateTime->year = t.tm_year + 1900;

    /* 0-based so add 1 */
    dateTime->month = t.tm_mon + 1;
    dateTime->dayOfMonth = t.tm_mday;
    dateTime->dayOfWeek = t.tm_wday + 1;
    dateTime->dayOfYear = t.tm_yday + 1;
    dateTime->hour = t.tm_hour;
    dateTime->minute = t.tm_min;
    dateTime->second = t.tm_sec + millis / 1000.0;

    /* Compute the # of milliseconds */
    if (!nrt_DateTime_updateMillis(dateTime, error))
    {
        NRT_FREE(dateTime);
        return NULL;
    }

    return dateTime;
}

NRTAPI(void) nrt_DateTime_destruct(nrt_DateTime ** dt)
{
    if (*dt)
    {
        NRT_FREE(*dt);
        *dt = NULL;
    }
}

NRTAPI(NRT_BOOL) nrt_DateTime_format(const nrt_DateTime * dateTime,
                                     const char *format, char *outBuf,
                                     size_t maxSize, nrt_Error * error)
{
    return nrt_DateTime_formatMillis(dateTime->timeInMillis, format, outBuf,
                                     maxSize, error);
}

static int nrt_Date_Time_formatMillis_sscanf(char const* const buffer, char const* const format,
    int* decimalPlaces)
{
#ifdef _MSC_VER // Visual Studio
#pragma warning(push)
#pragma warning(disable: 4996) // '...' : This function or variable may be unsafe. Consider using ... instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#endif
    return sscanf(buffer, format, decimalPlaces);
#ifdef _MSC_VER // Visual Studio
#pragma warning(pop)
#endif
}
#define sscanf(buffer, format, decimalPlaces) nrt_Date_Time_formatMillis_sscanf(buffer, format, decimalPlaces)

NRTAPI(NRT_BOOL) nrt_DateTime_formatMillis(double millis, const char *format,
                                           char *outBuf, size_t maxSize,
                                           nrt_Error * error)
{
    time_t timeInSeconds;
    double fractSeconds;
    struct tm t;
    char *newFmtString = NULL;
    const char *endString = NULL;
    size_t begStringLen = 0;
    size_t formatLength;
    size_t startIndex = 0;
    size_t i, j;
    NRT_BOOL found = 0;

    timeInSeconds = (time_t) (millis / 1000);
    gmtime_s(&t, &timeInSeconds);
    fractSeconds = (millis / 1000.0) - timeInSeconds;

    /* Search for "%...S" string */
    formatLength = strlen(format);
    for (i = 0; i < formatLength && !found; ++i)
    {
        if (format[i] == '%')
        {
            startIndex = i;
            for (j = startIndex + 1; j < formatLength; ++j)
            {
                if (format[j] == '%')
                {
                    break;
                }

                if (format[j] == 'S')
                {
                    found = 1;
                    formatLength = j - startIndex + 1;
                    begStringLen = startIndex;
                    endString = format + j + 1;
                }
            }
        }
    }

    /* If we found a "%...S" string, parse it */
    /* to find out how many decimal places to use */
    if (found)
    {
        int decimalPlaces = 0;

        /* Figure out how many decimal places we need... */
        for (i = startIndex + 1; i < startIndex + (formatLength - 1); ++i)
        {
            if (format[i] == '.')
            {
                /* The digits that follow should be */
                /* the number of decimal places */
                sscanf(format + i + 1, "%d", &decimalPlaces);
            }
        }

        if (decimalPlaces > 0)
        {
            char buf[256];
            size_t newFmtLen = 0;
            size_t bufIdx = 0;
            size_t endStringLen = endString ? strlen(endString) : 0;

            newFmtLen = begStringLen + 1;
            newFmtString = (char *) NRT_MALLOC(newFmtLen);
            if (!newFmtString)
            {
                nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                               NRT_ERR_MEMORY);
                goto CATCH_ERROR;
            }
            memset(newFmtString, 0, newFmtLen);

            if (begStringLen > 0)
            {
                /* do the first part of the format */
                nrt_strncpy_s(newFmtString, newFmtLen, format, begStringLen);

                if (strftime(outBuf, maxSize, newFmtString, &t) == 0)
                {
                    nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INVALID_OBJECT,
                                    "Unknown error caused by the call to strftime with format string: [%s]",
                                    format);
                    goto CATCH_ERROR;
                }
                bufIdx = strlen(outBuf);
            }

            /* do the seconds - separately */
            memset(buf, 0, 256);
            if (strftime(buf, 256, "%S", &t) == 0)
            {
                nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INVALID_OBJECT,
                                "Unknown error caused by the call to strftime with format string: [%s]",
                                format);
                goto CATCH_ERROR;
            }

            size_t result_sz = strlen(buf) + bufIdx + 1;
            if (result_sz > maxSize)
            {
                nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INVALID_OBJECT,
                                "Format string will cause buffer to overflow: [%s]",
                                format);
                goto CATCH_ERROR;
            }

            /* tack it on the end */
            nrt_strcpy_s(outBuf + bufIdx, result_sz, buf);
            bufIdx = strlen(outBuf);

            memset(buf, 0, 256);
            NRT_SNPRINTF(buf, 256, "%.*f", decimalPlaces, fractSeconds);

            result_sz = strlen(buf) + bufIdx + 1;
            if (strlen(buf) + bufIdx + 1 > maxSize)
            {
                nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INVALID_OBJECT,
                                "Format string will cause buffer to overflow: [%s]",
                                format);
                goto CATCH_ERROR;
            }

            /* tack on the fractional seconds - spare the leading 0 */
            nrt_strcpy_s(outBuf + bufIdx, result_sz, buf + 1);
            bufIdx = strlen(outBuf);

            if (endStringLen > 0)
            {
                /* tack on the end part */
                memset(buf, 0, 256);
                if (strftime(buf, 256, endString, &t) == 0)
                {
                    nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INVALID_OBJECT,
                                    "Unknown error caused by the call to strftime with format string: [%s]",
                                    format);
                    goto CATCH_ERROR;
                }

                result_sz = strlen(buf) + bufIdx + 1;
                if (strlen(buf) + bufIdx + 1 > maxSize)
                {
                    nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INVALID_OBJECT,
                                    "Format string will cause buffer to overflow: [%s]",
                                    format);
                    goto CATCH_ERROR;
                }
                nrt_strcpy_s(outBuf + bufIdx, result_sz, buf);
            }
        }
    }

    if (newFmtString == NULL)
    {
        if (strftime
            (outBuf, maxSize, newFmtString != NULL ? newFmtString : format,
             &t) == 0)
        {
            nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INVALID_OBJECT,
                            "Unknown error caused by the call to strftime with format string: [%s]",
                            newFmtString != NULL ? newFmtString : format);
            goto CATCH_ERROR;
        }
    }
    else
        NRT_FREE(newFmtString);

    return NRT_SUCCESS;

    CATCH_ERROR:
    if (newFmtString)
        NRT_FREE(newFmtString);

    return NRT_FAILURE;
}

/* http://social.msdn.microsoft.com/forums/en-US/vcgeneral/thread/25a654f9-b6b6-490a-8f36-c87483bb36b7 */

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#define ALT_E          0x01
#define ALT_O          0x02
/* #define LEGAL_ALT(x)       { if (alt_format & ~(x)) return (0); } */
#define LEGAL_ALT(x)       { ; }
#define TM_YEAR_BASE NRT_YEAR0

static const char *day[7] = { "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};
static const char *abday[7] =
    { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
static const char *mon[12] = { "January", "February", "March", "April", "May",
    "June", "July", "August", "September",
    "October", "November", "December"
};

static const char *abmon[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
/*static const char *am_pm[2] = { "AM", "PM" };*/

NRTPRIV(int) _NRT_convNum(const char **, int *, int, int);

NRTPRIV(char *) _NRT_strptime(const char *buf, const char *fmt, struct tm *tm,
                              double *millis)
{
    char c;
    const char *bp;
    size_t len = 0;
    int alt_format, i, split_year = 0;
    NRT_BOOL isYearSet = NRT_FALSE;
    NRT_BOOL isDayOfYearSet = NRT_FALSE;

    bp = buf;
    *millis = 0.0;

    /* init */
    tm->tm_sec = tm->tm_min = tm->tm_hour = tm->tm_mday = tm->tm_mon =
        tm->tm_year = tm->tm_wday = tm->tm_yday = 0;
    /* tm->tm_isdst = lt.tm_isdst; */

    while ((c = *fmt) != '\0')
    {
        /* Clear `alternate' modifier prior to new conversion. */
        alt_format = 0;

        /* Eat up white-space. */
        if (isspace(c))
        {
            while (isspace(*bp))
                bp++;

            fmt++;
            continue;
        }

        if ((c = *fmt++) != '%')
            goto literal;

    again:switch (c = *fmt++)
        {
        case '%':              /* "%%" is converted to "%". */
    literal:
            if (c != *bp++)
                return NULL;
            break;

            /*
             * "Alternative" modifiers. Just set the appropriate flag
             * and start over again.
             */
        case 'E':              /* "%E?" alternative conversion modifier. */
            LEGAL_ALT(0);
            alt_format |= ALT_E;
            goto again;

        case 'O':              /* "%O?" alternative conversion modifier. */
            LEGAL_ALT(0);
            alt_format |= ALT_O;
            goto again;

            /*
             * "Complex" conversion rules, implemented through recursion.
             */
        case 'c':              /* Date and time, using the locale's format. */
            LEGAL_ALT(ALT_E);
            bp = _NRT_strptime(bp, "%x %X", tm, millis);
            if (!bp)
                return NULL;
            break;

        case 'D':              /* The date as "%m/%d/%y". */
            LEGAL_ALT(0);
            bp = _NRT_strptime(bp, "%m/%d/%y", tm, millis);
            if (!bp)
                return NULL;
            break;

        case 'R':              /* The time as "%H:%M". */
            LEGAL_ALT(0);
            bp = _NRT_strptime(bp, "%H:%M", tm, millis);
            if (!bp)
                return NULL;
            break;

        case 'r':              /* The time in 12-hour clock representation. */
            LEGAL_ALT(0);
            bp = _NRT_strptime(bp, "%I:%M:%S %p", tm, millis);
            if (!bp)
                return NULL;
            break;

        case 'T':              /* The time as "%H:%M:%S". */
            LEGAL_ALT(0);
            bp = _NRT_strptime(bp, "%H:%M:%S", tm, millis);
            if (!bp)
                return NULL;
            break;

        case 'X':              /* The time, using the locale's format. */
            LEGAL_ALT(ALT_E);
            bp = _NRT_strptime(bp, "%H:%M:%S", tm, millis);
            if (!bp)
                return NULL;
            break;

        case 'x':              /* The date, using the locale's format. */
            LEGAL_ALT(ALT_E);
            bp = _NRT_strptime(bp, "%m/%d/%y", tm, millis);
            if (!bp)
                return NULL;
            break;

            /*
             * "Elementary" conversion rules.
             */
        case 'A':              /* The day of week, using the locale's form. */
        case 'a':
            LEGAL_ALT(0);
            for (i = 0; i < 7; i++)
            {
                /* Full name. */
                len = strlen(day[i]);
                if (nrt_Utils_strncasecmp((char *) (day[i]), (char *) bp, len)
                    == 0)
                    break;

                /* Abbreviated name. */
                len = strlen(abday[i]);
                if (nrt_Utils_strncasecmp((char *) (abday[i]), (char *) bp, len)
                    == 0)
                    break;
            }

            /* Nothing matched. */
            if (i == 7)
                return NULL;

            tm->tm_wday = i;
            bp += len;
            break;

        case 'B':              /* The month, using the locale's form. */
        case 'b':
        case 'h':
            LEGAL_ALT(0);
            for (i = 0; i < 12; i++)
            {
                /* Full name. */
                len = strlen(mon[i]);
                if (nrt_Utils_strncasecmp((char *) (mon[i]), (char *) bp, len)
                    == 0)
                    break;

                /* Abbreviated name. */
                len = strlen(abmon[i]);
                if (nrt_Utils_strncasecmp((char *) (abmon[i]), (char *) bp, len)
                    == 0)
                    break;
            }

            /* Nothing matched. */
            if (i == 12)
                return NULL;

            tm->tm_mon = i;
            bp += len;
            break;

        case 'C':              /* The century number. */
            LEGAL_ALT(ALT_E);
            if (!(_NRT_convNum(&bp, &i, 0, 99)))
                return NULL;
            if (split_year)
            {
                tm->tm_year = (tm->tm_year % 100) + (i * 100);
            }
            else
            {
                tm->tm_year = i * 100;
                split_year = 1;
            }
            isYearSet = NRT_TRUE;
            break;

        case 'd':              /* The day of month. */
        case 'e':
            LEGAL_ALT(ALT_O);
            if (!(_NRT_convNum(&bp, &tm->tm_mday, 1, 31)))
                return NULL;
            break;

        case 'k':              /* The hour (24-hour clock representation). */
            LEGAL_ALT(0);
            /* FALLTHROUGH */
        case 'H':
            LEGAL_ALT(ALT_O);
            if (!(_NRT_convNum(&bp, &tm->tm_hour, 0, 23)))
                return NULL;
            break;

        case 'l':              /* The hour (12-hour clock representation). */
            LEGAL_ALT(0);
            /* FALLTHROUGH */
        case 'I':
            LEGAL_ALT(ALT_O);
            if (!(_NRT_convNum(&bp, &tm->tm_hour, 1, 12)))
                return NULL;
            if (tm->tm_hour == 12)
                tm->tm_hour = 0;
            break;

        case 'j':              /* The day of year. */
            LEGAL_ALT(0);
            if (!(_NRT_convNum(&bp, &i, 1, 366)))
                return NULL;
            tm->tm_yday = i - 1;
            isDayOfYearSet = NRT_TRUE;
            break;

        case 'M':              /* The minute. */
            LEGAL_ALT(ALT_O);
            if (!(_NRT_convNum(&bp, &tm->tm_min, 0, 59)))
                return NULL;
            break;

        case 'm':              /* The month. */
            LEGAL_ALT(ALT_O);
            if (!(_NRT_convNum(&bp, &i, 1, 12)))
                return NULL;
            tm->tm_mon = i - 1;
            break;

/*                        case 'p':  The locale's equivalent of AM/PM.
                            LEGAL_ALT(0);
                             AM?
                            if (strcasecmp(am_pm[0], bp) == 0)
                            {
                                if (tm->tm_hour > 11)
                                    return NULL;

                                bp += strlen(am_pm[0]);
                                break;
                            }
                             PM?
                            else if (strcasecmp(am_pm[1], bp) == 0)
                            {
                                if (tm->tm_hour > 11)
                                    return NULL;

                                tm->tm_hour += 12;
                                bp += strlen(am_pm[1]);
                                break;
                            }

                             Nothing matched.
                            return NULL;*/

        case 'S':              /* The seconds. */
            LEGAL_ALT(ALT_O);
            if (!(_NRT_convNum(&bp, &tm->tm_sec, 0, 61)))
                return NULL;

            /* Determine if the next character is a decimal... */
            if (*bp == '.')
            {
                int decimalPlaces = 0;
                /* Get the fractional seconds value */
                bp++;
                while (*bp >= '0' && *bp <= '9')
                {
                    const char num_ = *bp++ - '0';
                    double num = (double) (num_);
                    decimalPlaces++;

                    switch (decimalPlaces)
                    {
                    case 1:
                        num *= 100;
                        break;
                    case 2:
                        num *= 10;
                        break;
                    case 3:
                        break;
                    default:
                        for (i = 0; i < decimalPlaces - 3; ++i)
                            num /= 10.0;
                        break;
                    }
                    *millis += num;
                }
            }
            break;

        case 'U':              /* The week of year, beginning on sunday. */
        case 'W':              /* The week of year, beginning on monday. */
            LEGAL_ALT(ALT_O);
            /*
             * XXX This is bogus, as we can not assume any valid
             * information present in the tm structure at this
             * point to calculate a real value, so just check the
             * range for now.
             */
            if (!(_NRT_convNum(&bp, &i, 0, 53)))
                return NULL;
            break;

        case 'w':              /* The day of week, beginning on sunday. */
            LEGAL_ALT(ALT_O);
            if (!(_NRT_convNum(&bp, &tm->tm_wday, 0, 6)))
                return NULL;
            break;

        case 'Y':              /* The year. */
            LEGAL_ALT(ALT_E);
            i = TM_YEAR_BASE;
            if (!(_NRT_convNum(&bp, &i, 0, 9999)))
                return NULL;
            tm->tm_year = i - TM_YEAR_BASE;
            isYearSet = NRT_TRUE;
            break;

        case 'y':              /* The year within 100 years of the epoch. */
            LEGAL_ALT(ALT_E | ALT_O);
            if (!(_NRT_convNum(&bp, &i, 0, 99)))
                return NULL;
            isYearSet = NRT_TRUE;
            if (split_year)
            {
                tm->tm_year = ((tm->tm_year / 100) * 100) + i;
                break;
            }
            split_year = 1;
            if (i <= 68)
                tm->tm_year = i + 2000 - TM_YEAR_BASE;
            else
                tm->tm_year = i + 1900 - TM_YEAR_BASE;
            break;
            /*
             * Miscellaneous conversions.
             */
        case 'n':              /* Any kind of white-space. */
        case 't':
            LEGAL_ALT(0);
            while (isspace(*bp))
                bp++;
            break;

        default:               /* Unknown/unsupported conversion. */
            return NULL;
        }

    }

    /* If we have the day of year and year, infer the corresponding month
     * and day of month - this will overwrite the day of month and month
     * if either was provided */
    if (isYearSet && isDayOfYearSet)
    {
        int month=0, dayOfMonth=0;
        if (!nrt_DateTime_setMonthInfoFromDayOfYear(tm->tm_year + 1900,
                                                    tm->tm_yday + 1,
                                                    &month,
                                                    &dayOfMonth))
        {
            return NULL;
        }

        /* setMonthInfoFromDayOfYear sets the correct 1 indexed month -
         * subtract 1 as the tm struct's month field is 0 indexed */
        tm->tm_mon = month - 1;
        tm->tm_mday = dayOfMonth;
    }

    /* LINTED functional specification */
    return ((char *) bp);
}

static int _NRT_convNum(const char **buf, int *dest, int llim, int ulim)
{
    int result = 0;

    /* The limit also determines the number of valid digits. */
    int rulim = ulim;

    if (**buf < '0' || **buf > '9')
        return 0;

    do
    {
        result *= 10;
        result += *(*buf)++ - '0';
        rulim /= 10;
    }
    while ((result * 10 <= ulim) && rulim && **buf >= '0' && **buf <= '9');

    if (result < llim || result > ulim)
        return 0;

    *dest = result;
    return 1;
}
