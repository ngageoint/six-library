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

#include <import/nrt.h>
#include "Test.h"

#define MAX_DATE_STRING    1024
#define NRT_DATE_FORMAT_21  "%Y%j%m%d%H%M%S"
#define NRT_FDT_SZ 17

/*
NRTPRIV(void) printDate(nrt_DateTime * date)
{
    printf("Year: %d\n", date->year);
    printf("Month: %d\n", date->month);
    printf("Day Of Month: %d\n", date->dayOfMonth);
    printf("Day Of Week: %d\n", date->dayOfWeek);
    printf("Day Of Year: %d\n", date->dayOfYear);
    printf("Hour: %d\n", date->hour);
    printf("Minute: %d\n", date->minute);
    printf("Second: %f\n", date->second);
    printf("Millis: %f\n", date->timeInMillis);
}
*/

TEST_CASE(testNow)
{
    nrt_DateTime *date = NULL;
    nrt_Error e;

    date = nrt_DateTime_now(&e);
    TEST_ASSERT(date != NULL);

    nrt_DateTime_destruct(&date);
    TEST_ASSERT_NULL(date);
}

TEST_CASE(testFromMillis)
{
    nrt_DateTime *date = NULL, *date2 = NULL;
    nrt_Error e;

    date = nrt_DateTime_now(&e);
    TEST_ASSERT(date != NULL);

    date2 = nrt_DateTime_fromMillis(date->timeInMillis, &e);
    TEST_ASSERT(date2 != NULL);

    TEST_ASSERT((date->timeInMillis == date2->timeInMillis));

    nrt_DateTime_destruct(&date);
    nrt_DateTime_destruct(&date2);
    TEST_ASSERT_NULL(date);
    TEST_ASSERT_NULL(date2);
}

TEST_CASE(testParseDayOfYearTimeStr)
{
    /* Test input without month and day of month information */
    const char *timeStr = "2017-304T22:55:37.123456Z";
    nrt_DateTime *date = NULL;

    /* Test input with correct month and day of month information */
    const char *timeStr2 = "2017-304-10-31T22:55:37.123456Z";
    nrt_DateTime *date2 = NULL;

    /* Test input with incorrect month and day of month information */
    const char *timeStr3 = "2017-304-08-08T22:55:37.123456Z";
    nrt_DateTime *date3 = NULL;

    nrt_Error e;

    /* Test without month and day of month information */
    date = nrt_DateTime_fromString(timeStr, "%Y-%jT%H:%M:%SZ", &e);
    TEST_ASSERT(date != NULL);

    TEST_ASSERT_EQ_INT(date->month, 10);
    TEST_ASSERT_EQ_INT(date->dayOfMonth, 31);
    TEST_ASSERT_EQ_INT(date->dayOfWeek, 3);
    TEST_ASSERT_EQ_INT(date->dayOfYear, 304);

    /* Test input with correct month and day of month information */
    date2 = nrt_DateTime_fromString(timeStr2, "%Y-%j-%m-%dT%H:%M:%SZ", &e);
    TEST_ASSERT(date2 != NULL);

    TEST_ASSERT_EQ_INT(date2->month, 10);
    TEST_ASSERT_EQ_INT(date2->dayOfMonth, 31);
    TEST_ASSERT_EQ_INT(date2->dayOfWeek, 3);
    TEST_ASSERT_EQ_INT(date2->dayOfYear, 304);

    /* Test input with incorrect month and day of month information */
    date3 = nrt_DateTime_fromString(timeStr3, "%Y-%j-%m-%dT%H:%M:%SZ", &e);
    TEST_ASSERT(date3 != NULL);

    TEST_ASSERT_EQ_INT(date3->month, 10);
    TEST_ASSERT_EQ_INT(date3->dayOfMonth, 31);
    TEST_ASSERT_EQ_INT(date3->dayOfWeek, 3);
    TEST_ASSERT_EQ_INT(date3->dayOfYear, 304);

    nrt_DateTime_destruct(&date);
    nrt_DateTime_destruct(&date2);
    nrt_DateTime_destruct(&date3);

    TEST_ASSERT_NULL(date);
    TEST_ASSERT_NULL(date2);
    TEST_ASSERT_NULL(date3);
}

TEST_CASE(testRoundTrip)
{
    nrt_DateTime *date = NULL, *date2 = NULL;
    char buf[MAX_DATE_STRING], buf2[MAX_DATE_STRING];
    nrt_Error e;

    date = nrt_DateTime_now(&e);
    TEST_ASSERT(date != NULL);
    /* printDate(date); */

    TEST_ASSERT((nrt_DateTime_format
                 (date, NRT_DATE_FORMAT_21, buf, NRT_FDT_SZ + 1, &e)));
    /* printf("Date: %s\n", buf); */

    date2 = nrt_DateTime_fromString(buf, NRT_DATE_FORMAT_21, &e);
    TEST_ASSERT(date2 != NULL);

    TEST_ASSERT((nrt_DateTime_format
                 (date2, NRT_DATE_FORMAT_21, buf2, NRT_FDT_SZ + 1, &e)));
    /* printf("Date: %s\n", buf2); */
    /* printDate(date2); */

    TEST_ASSERT_EQ_STR(buf, buf2);

    /* must subtract off the millis - since our format string doesn't include
     * them */
    nrt_DateTime_setSecond(date, (int) date->second, &e);
    nrt_DateTime_setSecond(date2, (int) date2->second, &e);
    TEST_ASSERT_EQ_FLOAT(date->timeInMillis, date2->timeInMillis);

    nrt_DateTime_destruct(&date);
    nrt_DateTime_destruct(&date2);
    TEST_ASSERT_NULL(date);
    TEST_ASSERT_NULL(date2);
}

TEST_CASE(testSetIdentity)
{
    nrt_DateTime *date = NULL;
    char buf[MAX_DATE_STRING], buf2[MAX_DATE_STRING];
    nrt_Error e;

    date = nrt_DateTime_now(&e);
    TEST_ASSERT(date != NULL);

    TEST_ASSERT((nrt_DateTime_format
                 (date, NRT_DATE_FORMAT_21, buf, NRT_FDT_SZ + 1, &e)));

    /* set hour */
    TEST_ASSERT(nrt_DateTime_setHour(date, date->hour, &e));
    TEST_ASSERT((nrt_DateTime_format
                 (date, NRT_DATE_FORMAT_21, buf2, NRT_FDT_SZ + 1, &e)));
    TEST_ASSERT_EQ_STR(buf, buf2);

    /* set minute */
    TEST_ASSERT(nrt_DateTime_setMinute(date, date->minute, &e));
    TEST_ASSERT((nrt_DateTime_format
                 (date, NRT_DATE_FORMAT_21, buf2, NRT_FDT_SZ + 1, &e)));
    TEST_ASSERT_EQ_STR(buf, buf2);

    /* set second */
    TEST_ASSERT(nrt_DateTime_setSecond(date, date->second, &e));
    TEST_ASSERT((nrt_DateTime_format
                 (date, NRT_DATE_FORMAT_21, buf2, NRT_FDT_SZ + 1, &e)));
    TEST_ASSERT_EQ_STR(buf, buf2);

    /* set month */
    TEST_ASSERT(nrt_DateTime_setMonth(date, date->month, &e));
    TEST_ASSERT((nrt_DateTime_format
                 (date, NRT_DATE_FORMAT_21, buf2, NRT_FDT_SZ + 1, &e)));
    TEST_ASSERT_EQ_STR(buf, buf2);

    /* set month */
    TEST_ASSERT(nrt_DateTime_setYear(date, date->year, &e));
    TEST_ASSERT((nrt_DateTime_format
                 (date, NRT_DATE_FORMAT_21, buf2, NRT_FDT_SZ + 1, &e)));
    TEST_ASSERT_EQ_STR(buf, buf2);

    /* set day of year */
    TEST_ASSERT(nrt_DateTime_setDayOfYear(date, date->dayOfYear, &e));
    TEST_ASSERT((nrt_DateTime_format
                 (date, NRT_DATE_FORMAT_21, buf2, NRT_FDT_SZ + 1, &e)));
    TEST_ASSERT_EQ_STR(buf, buf2);

    nrt_DateTime_destruct(&date);
    TEST_ASSERT_NULL(date);
}

TEST_CASE(testMillis)
{
    nrt_Error e;
    char buf[MAX_DATE_STRING];
    const char *timeStr = "2010-01-12T22:55:37.123456Z";
    nrt_DateTime *date = NULL;

    date = nrt_DateTime_fromString(timeStr, "%Y-%m-%dT%H:%M:%SZ", &e);
    TEST_ASSERT(date != NULL);

    TEST_ASSERT_EQ_INT((int) (1000 * (date->second - (int) date->second)), 123);

    nrt_DateTime_format(date, "%S", buf, MAX_DATE_STRING, &e);
    TEST_ASSERT_EQ_STR(buf, "37");

    nrt_DateTime_format(date, "%.3S", buf, MAX_DATE_STRING, &e);
    TEST_ASSERT_EQ_STR(buf, "37.123");

    nrt_DateTime_format(date, "%Y%.3S", buf, MAX_DATE_STRING, &e);
    TEST_ASSERT_EQ_STR(buf, "201037.123");

    nrt_DateTime_format(date, "%Y-%m-%dT%H:%M:%.6SZ", buf, MAX_DATE_STRING, &e);
    TEST_ASSERT_EQ_STR(buf, timeStr);

    nrt_DateTime_destruct(&date);
    TEST_ASSERT_NULL(date);
}

TEST_MAIN
(
    (void)argc;
    (void)argv;

    CHECK(testNow);
    CHECK(testFromMillis);
    CHECK(testRoundTrip);
    CHECK(testSetIdentity);
    CHECK(testMillis);
    CHECK(testParseDayOfYearTimeStr)
)

