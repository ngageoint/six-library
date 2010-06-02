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
#include "nitf/Utils.h"


#define NITF_DATE_FORMAT_20  "%d%H%M%SZ%b%y"
#define NITF_DATE_FORMAT_21  "%Y%m%d%H%M%S"


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
 * Sets the year of a DateTime object and updates the timeInMillis.
 */
NITFAPI(NITF_BOOL) nitf_DateTime_setYear(nitf_DateTime *dateTime,
                                         int year,
                                         nitf_Error *error);

/*!
 * Sets the month of a DateTime object and updates the timeInMillis.
 */
NITFAPI(NITF_BOOL) nitf_DateTime_setMonth(nitf_DateTime *dateTime,
                                          int month,
                                          nitf_Error *error);

/*!
 * Sets the dayOfMonth of a DateTime object and updates the timeInMillis.
 */
NITFAPI(NITF_BOOL) nitf_DateTime_setDayOfMonth(nitf_DateTime *dateTime,
                                               int dayOfMonth,
                                               nitf_Error *error);

/*!
 * Sets the dayOfWeek of a DateTime object and updates the timeInMillis.
 */
NITFAPI(NITF_BOOL) nitf_DateTime_setDayOfWeek(nitf_DateTime *dateTime,
                                              int dayOfWeek,
                                              nitf_Error *error);

/*!
 * Sets the dayOfYear of a DateTime object and updates the timeInMillis.
 */
NITFAPI(NITF_BOOL) nitf_DateTime_setDayOfYear(nitf_DateTime *dateTime,
                                              int dayOfYear,
                                              nitf_Error *error);

/*!
 * Sets the hour of a DateTime object and updates the timeInMillis.
 */
NITFAPI(NITF_BOOL) nitf_DateTime_setHour(nitf_DateTime *dateTime,
                                         int hour,
                                         nitf_Error *error);

/*!
 * Sets the minute of a DateTime object and updates the timeInMillis.
 */
NITFAPI(NITF_BOOL) nitf_DateTime_setMinute(nitf_DateTime *dateTime,
                                           int minute,
                                           nitf_Error *error);

/*!
 * Sets the second of a DateTime object and updates the timeInMillis.
 */
NITFAPI(NITF_BOOL) nitf_DateTime_setSecond(nitf_DateTime *dateTime,
                                           double second,
                                           nitf_Error *error);

/*!
 * Sets the timeInMillis of a DateTime object and updates
 * the remaining fields to match the new timeInMillis.
 */
NITFAPI(NITF_BOOL) nitf_DateTime_setTimeInMillis(nitf_DateTime *dateTime,
                                                 double timeInMillis,
                                                 nitf_Error *error);

/*!
 * Returns a DateTime object from the string with the given format.
 */
NITFAPI(nitf_DateTime*) nitf_DateTime_fromString(const char* string,
        const char* format, nitf_Error *error);

/*!
 * Destroys the DateTime object.
 */
NITFAPI(void) nitf_DateTime_destruct(nitf_DateTime**);

/*!
 * Formats a DateTime object using the given format string and output buffer.
 *
 * \param dateTime  the DateTime object
 * \param format    the format string (uses the srftime format)
 * \param outBuf    the output buffer to write to (passed in & pre-allocated)
 * \param maxSize   the maximum size of the formatted output
 * \param error     the error object
 * \return  NITF_SUCCESS or NITF_FAILURE
 */
NITFAPI(NITF_BOOL) nitf_DateTime_format(nitf_DateTime *dateTime,
        const char* format, char* outBuf,
        size_t maxSize, nitf_Error *error);
/*!
 *
 * Uses the input milliseconds since the epoch as the DateTime to format
 * using the given format string and output buffer.
 *
 * \param millis    the # of milliseconds since the epoch
 * \param format    the format string (uses the srftime format)
 * \param outBuf    the output buffer to write to (passed in & pre-allocated)
 * \param maxSize   the maximum size of the formatted output
 * \param error     the error object
 * \return  NITF_SUCCESS or NITF_FAILURE
 */
NITFAPI(NITF_BOOL) nitf_DateTime_formatMillis(double millis,
        const char* format, char* outBuf,
        size_t maxSize, nitf_Error *error);


NITF_CXX_ENDGUARD

#endif
