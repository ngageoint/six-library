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


NITFAPI(nitf_DateTime*) nitf_DateTime_now(nitf_Error *error)
{
    return nitf_DateTime_fromMillis(nitf_Utils_getCurrentTimeMillis(), error);
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


NITFAPI(nitf_DateTime*) nitf_DateTime_fromString(const char* string,
        const char* format, nitf_Error *error)
{
#ifdef WIN32
    nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_OBJECT,
            "strptime (or a reasonable alternative) is not available for Windows");
    return NULL;
#else
    struct tm t;
    if (!strptime(string, format, &t))
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_OBJECT,
                "Unknown error caused by the call to strptime with format string: [%s]",
                format);
        return NULL;
    }
    return nitf_DateTime_fromMillis((double)time(&t) * 1000, error);
#endif
}

NITFAPI(void) nitf_DateTime_destruct(nitf_DateTime **dt)
{
    if (*dt)
    {
        NITF_FREE(*dt);
        *dt = NULL;
    }
}


NITFAPI(NITF_BOOL) nitf_DateTime_format(nitf_DateTime *dateTime,
        const char* format, char* outBuf, size_t maxSize, nitf_Error *error)
{
    return nitf_DateTime_formatMillis(dateTime->timeInMillis,
            format, outBuf, maxSize, error);
}

NITFAPI(NITF_BOOL) nitf_DateTime_formatMillis(double millis,
        const char* format, char* outBuf, size_t maxSize, nitf_Error *error)
{
    time_t timeInSeconds;
    struct tm t;

    timeInSeconds = millis / 1000;
    t = *gmtime(&timeInSeconds);

    if (strftime(outBuf, maxSize, format, &t) == 0)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_OBJECT,
                "Unknown error caused by the call to strftime with format string: [%s]",
                format);
        return NITF_FAILURE;
    }
    return NITF_SUCCESS;
}
