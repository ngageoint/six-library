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

#include <import/nitf.h>


int main(int argc, char** argv)
{
    nitf_DateTime *date = NULL;
    char *dateStr = NULL;
    nitf_Error error;

    date = nitf_DateTime_now(&error);
    if (!date)
    {
        nitf_Error_print(&error, stdout, "Exiting (1) ...");
        exit(EXIT_FAILURE);
    }

    printf("Year: %d\n", date->year);
    printf("Month: %d\n", date->month);
    printf("Day Of Month: %d\n", date->dayOfMonth);
    printf("Day Of Week: %d\n", date->dayOfWeek);
    printf("Day Of Year: %d\n", date->dayOfYear);
    printf("Hour: %d\n", date->hour);
    printf("Minute: %d\n", date->minute);
    printf("Second: %f\n", date->second);
    printf("Millis: %f\n", date->timeInMillis);

    dateStr = nitf_DateTime_format(date, NITF_FORMAT_21, &error);
    if (!dateStr)
    {
        nitf_Error_print(&error, stdout, "Exiting (2) ...");
        exit(EXIT_FAILURE);
    }
    printf("The Current NITF 2.1 Formatted Date: %s\n", dateStr);
    if (dateStr) NITF_FREE(dateStr);

    /* If you know the millis (since the epoch) you can do a quick-format: */
    dateStr = nitf_DateTime_formatMillis(date->timeInMillis, NITF_FORMAT_21, &error);
    if (!dateStr)
    {
        nitf_Error_print(&error, stdout, "Exiting (2) ...");
        exit(EXIT_FAILURE);
    }
    printf("The Current NITF 2.1 Formatted Date: %s\n", dateStr);
    if (dateStr) NITF_FREE(dateStr);

    if (date) nitf_DateTime_destruct(&date);

    return 0;
}
