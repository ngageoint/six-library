/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
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

#include "nitf/DateTime.h"

#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#elif defined(_WIN32)
#include <windows.h>
#endif


NITFAPI(nitf_DateTime*) nitf_DateTime_now(nitf_Error *error)
{
    double millis = 0;
#if defined(__POSIX) && defined(USE_CLOCK_GETTIME)
    struct timespec now;
    clock_gettime(CLOCK_REALTIME,&now);
    millis = (now.tv_sec + 1.0e-9 * now.tv_nsec) * 1000;
#elif defined(HAVE_SYS_TIME_H)
    struct timeval now;
    gettimeofday(&now,NULL);
    millis = (now.tv_sec + 1.0e-6 * now.tv_usec) * 1000;
#elif defined(_WIN32)
    // Getting time twice may be inefficient but is quicker
    // than converting the SYSTEMTIME structure into
    // milliseconds
    // We could add an additional flag here if the user
    // does not need millisecond accuracy
    SYSTEMTIME now;
    GetLocalTime(&now);
    millis = (double)time(NULL) * 1000 + now.wMilliseconds;
#else
    millis = (double)time(NULL) * 1000;
#endif

    return nitf_DateTime_fromMillis(millis, error);
}

NITFAPI(nitf_DateTime*) nitf_DateTime_fromMillis(double millis,
        nitf_Error *error)
{
    time_t timeInSeconds;
    struct tm t;
    nitf_DateTime *dt = NULL;

    timeInSeconds = millis / 1000;
    t = *gmtime(&timeInSeconds);

    dt = (nitf_DateTime*)NITF_MALLOC(sizeof(nitf_DateTime));
    if (!dt)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    dt->timeInMillis = millis;

    /* this is the year since 1900 */
    dt->year = t.tm_year + 1900;

    /* 0-based so add 1 */
    dt->month = t.tm_mon + 1;
    dt->dayOfMonth = t.tm_mday;
    dt->dayOfWeek = t.tm_wday + 1;
    dt->dayOfYear = t.tm_yday + 1;
    dt->hour = t.tm_hour;
    dt->minute = t.tm_min;
    dt->second = t.tm_sec + (millis / 1000.0 - timeInSeconds);

    return dt;
}

NITFAPI(void) nitf_DateTime_destruct(nitf_DateTime **dt)
{
    if (*dt)
    {
        NITF_FREE(*dt);
        *dt = NULL;
    }
}


NITFAPI(char*) nitf_DateTime_format(nitf_DateTime *dateTime,
        const char* format, nitf_Error *error)
{
    return nitf_DateTime_formatMillis(dateTime->timeInMillis,
            format, error);
}

NITFAPI(char*) nitf_DateTime_formatMillis(double millis,
        const char* format, nitf_Error *error)
{
    time_t timeInSeconds;
    struct tm t;
    char *buf = NULL;
    size_t strfReturn;

    timeInSeconds = millis / 1000;
    t = *gmtime(&timeInSeconds);

    buf = (char*)NITF_MALLOC(NITF_MAX_DATE_STRING + 1);
    if (!buf)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    strfReturn = strftime(buf, NITF_MAX_DATE_STRING, format, &t);
    return buf;
}
