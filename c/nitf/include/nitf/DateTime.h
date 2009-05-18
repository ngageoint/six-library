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

#ifndef __NITF_DATE_TIME_H__
#define __NITF_DATE_TIME_H__

#include "nitf/Error.h"
#include "nitf/System.h"
#include <time.h>

#define NITF_MAX_DATE_STRING    1024

#define NITF_FORMAT_21  "%Y%m%d%H%M%S"


NITF_CXX_GUARD


/*!
 * The DateTime structure represents time. All dates are stored to reflect the
 * coordinated universal time (UTC). The precision depends on the host machine.
 * In many cases, seconds is the highest amount of granularity.
 */
typedef struct _nitf_DateTime
{
    int year;
    int month;
    int dayOfMonth;
    int dayOfWeek;
    int dayOfYear;
    int hour;
    int minute;
    double second;
    double timeInMillis;
} nitf_DateTime;


/*!
 * Returns a DateTime object representing the current moment in time.
 */
NITFAPI(nitf_DateTime*) nitf_DateTime_now(nitf_Error*);

/*!
 * Returns a DateTime object created from the milliseconds since the epoch:
 * January 1, 1970, 00:00:00 GMT.
 */
NITFAPI(nitf_DateTime*) nitf_DateTime_fromMillis(double millis, nitf_Error*);

/*!
 * Destroys the DateTime object.
 */
NITFAPI(void) nitf_DateTime_destruct(nitf_DateTime**);

/*!
 * Formats a DateTime object using the given format string, returning a newly
 * allocated string (which must be freed by the user using NITF_FREE).
 */
NITFAPI(char*) nitf_DateTime_format(nitf_DateTime *dateTime,
        const char* format, nitf_Error*);
/*!
 *
 * Uses the input milliseconds since the epoch as the DateTime to format
 * using the given format string, returning a newly
 * allocated string (which must be freed by the user using NITF_FREE).
 */
NITFAPI(char*) nitf_DateTime_formatMillis(double millis,
        const char* format, nitf_Error*);


NITF_CXX_ENDGUARD

#endif
