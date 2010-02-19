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


#define MAX_DATE_STRING    1024

int main(int argc, char** argv)
{
    nitf_DateTime *date = NULL, *date2 = NULL;
    char dateBuf[MAX_DATE_STRING];
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

    if (!nitf_DateTime_format(date, NITF_DATE_FORMAT_21, dateBuf,
            NITF_FDT_SZ + 1, &error))
    {
        nitf_Error_print(&error, stdout, "Exiting (2) ...");
        exit(EXIT_FAILURE);
    }
    printf("The Current NITF 2.1 Formatted Date: %s\n", dateBuf);
    printf("Applying format string: %s\n", NITF_DATE_FORMAT_21);

    date2 = nitf_DateTime_fromString(dateBuf, NITF_DATE_FORMAT_21, &error);
    printf("Roundtripped: %f\n", date2->timeInMillis);
    if (date2) nitf_DateTime_destruct(&date2);
    
    /* If you know the millis (since the epoch) you can do a quick-format: */
    if (!nitf_DateTime_formatMillis(date->timeInMillis, NITF_DATE_FORMAT_21, dateBuf,
            NITF_FDT_SZ + 1, &error))
    {
        nitf_Error_print(&error, stdout, "Exiting (2) ...");
        exit(EXIT_FAILURE);
    }
    printf("The Current NITF 2.1 Formatted Date: %s\n", dateBuf);
    
    date2 = nitf_DateTime_fromString(dateBuf, NITF_DATE_FORMAT_21, &error);
    printf("Roundtripped: %f\n", date2->timeInMillis);
    if (date2) nitf_DateTime_destruct(&date2);
    
    if (!nitf_DateTime_format(date, NITF_DATE_FORMAT_20, dateBuf,
            NITF_FDT_SZ + 1, &error))
    {
        nitf_Error_print(&error, stdout, "Exiting (3) ...");
        exit(EXIT_FAILURE);
    }
    printf("The Current NITF 2.0 Formatted Date: %s\n", dateBuf);
    
    date2 = nitf_DateTime_fromString(dateBuf, NITF_DATE_FORMAT_20, &error);
    printf("Roundtripped: %f\n", date2->timeInMillis);
    if (date2) nitf_DateTime_destruct(&date2);

    if (date) nitf_DateTime_destruct(&date);
    if (date2) nitf_DateTime_destruct(&date2);

    return 0;
}
