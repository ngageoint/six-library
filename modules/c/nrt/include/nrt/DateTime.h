/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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

#ifndef __NRT_DATE_TIME_H__
#define __NRT_DATE_TIME_H__

#include "nrt/System.h"
#include "nrt/Utils.h"

NRT_CXX_GUARD
/*!
 * The DateTime structure represents time. All dates are stored to reflect the
 * coordinated universal time (UTC). The precision depends on the host machine.
 * In many cases, seconds is the highest amount of granularity.
 */
typedef struct _NRT_DateTime
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
} nrt_DateTime;

/*!
 * Returns a DateTime object representing the current moment in time.
 */
NRTAPI(nrt_DateTime *) nrt_DateTime_now(nrt_Error *);

/*!
 * Returns a DateTime object created from the milliseconds since the epoch:
 * January 1, 1970, 00:00:00 GMT.
 */
NRTAPI(nrt_DateTime *) nrt_DateTime_fromMillis(double millis, nrt_Error *);

/*!
 * Sets the year of a DateTime object and updates the timeInMillis.
 */
NRTAPI(NRT_BOOL) nrt_DateTime_setYear(nrt_DateTime * dateTime, int year,
                                      nrt_Error * error);

/*!
 * Sets the month of a DateTime object and updates the timeInMillis.
 */
NRTAPI(NRT_BOOL) nrt_DateTime_setMonth(nrt_DateTime * dateTime, int month,
                                       nrt_Error * error);

/*!
 * Sets the dayOfMonth of a DateTime object and updates the timeInMillis.
 */
NRTAPI(NRT_BOOL) nrt_DateTime_setDayOfMonth(nrt_DateTime * dateTime,
                                            int dayOfMonth, nrt_Error * error);

/*!
 * Sets the dayOfWeek of a DateTime object and updates the timeInMillis.
 */
NRTAPI(NRT_BOOL) nrt_DateTime_setDayOfWeek(nrt_DateTime * dateTime,
                                           int dayOfWeek, nrt_Error * error);

/*!
 * Sets the dayOfYear of a DateTime object and updates the timeInMillis.
 */
NRTAPI(NRT_BOOL) nrt_DateTime_setDayOfYear(nrt_DateTime * dateTime,
                                           int dayOfYear, nrt_Error * error);

/*!
 * Sets the hour of a DateTime object and updates the timeInMillis.
 */
NRTAPI(NRT_BOOL) nrt_DateTime_setHour(nrt_DateTime * dateTime, int hour,
                                      nrt_Error * error);

/*!
 * Sets the minute of a DateTime object and updates the timeInMillis.
 */
NRTAPI(NRT_BOOL) nrt_DateTime_setMinute(nrt_DateTime * dateTime, int minute,
                                        nrt_Error * error);

/*!
 * Sets the second of a DateTime object and updates the timeInMillis.
 */
NRTAPI(NRT_BOOL) nrt_DateTime_setSecond(nrt_DateTime * dateTime, double second,
                                        nrt_Error * error);

/*!
 * Sets the timeInMillis of a DateTime object and updates
 * the remaining fields to match the new timeInMillis.
 */
NRTAPI(NRT_BOOL) nrt_DateTime_setTimeInMillis(nrt_DateTime * dateTime,
                                              double timeInMillis,
                                              nrt_Error * error);

/*!
 * Returns a DateTime object from the string with the given format.
 */
NRTAPI(nrt_DateTime *) nrt_DateTime_fromString(const char *string,
                                               const char *format,
                                               nrt_Error * error);

/*!
 * Destroys the DateTime object.
 */
NRTAPI(void) nrt_DateTime_destruct(nrt_DateTime **);

/*!
 * Formats a DateTime object using the given format string and output buffer.
 *
 * \param dateTime  the DateTime object
 * \param format    the format string (uses the srftime format)
 * \param outBuf    the output buffer to write to (passed in & pre-allocated)
 * \param maxSize   the maximum size of the formatted output
 * \param error     the error object
 * \return  NRT_SUCCESS or NRT_FAILURE
 */
NRTAPI(NRT_BOOL) nrt_DateTime_format(const nrt_DateTime * dateTime,
                                     const char *format, char *outBuf,
                                     size_t maxSize, nrt_Error * error);
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
 * \return  NRT_SUCCESS or NRT_FAILURE
 */
NRTAPI(NRT_BOOL) nrt_DateTime_formatMillis(double millis, const char *format,
                                           char *outBuf, size_t maxSize,
                                           nrt_Error * error);

NRT_CXX_ENDGUARD
#endif
