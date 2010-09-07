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

#include "nrt/DateTime.h"

#define NITF_DATE_FORMAT_20  "%d%H%M%SZ%b%y"
#define NITF_DATE_FORMAT_21  "%Y%m%d%H%M%S"

typedef nrt_DateTime                    nitf_DateTime;
#define nitf_DateTime_now               nrt_DateTime_now
#define nitf_DateTime_fromMillis        nrt_DateTime_fromMillis
#define nitf_DateTime_setYear           nrt_DateTime_setYear
#define nitf_DateTime_setMonth          nrt_DateTime_setMonth
#define nitf_DateTime_setDayOfMonth     nrt_DateTime_setDayOfMonth
#define nitf_DateTime_setDayOfWeek      nrt_DateTime_setDayOfWeek
#define nitf_DateTime_setDayOfYear      nrt_DateTime_setDayOfYear
#define nitf_DateTime_setHour           nrt_DateTime_setHour
#define nitf_DateTime_setMinute         nrt_DateTime_setMinute
#define nitf_DateTime_setSecond         nrt_DateTime_setSecond
#define nitf_DateTime_setTimeInMillis   nrt_DateTime_setTimeInMillis
#define nitf_DateTime_fromString        nrt_DateTime_fromString
#define nitf_DateTime_destruct          nrt_DateTime_destruct
#define nitf_DateTime_format            nrt_DateTime_format
#define nitf_DateTime_formatMillis      nrt_DateTime_formatMillis

#endif
