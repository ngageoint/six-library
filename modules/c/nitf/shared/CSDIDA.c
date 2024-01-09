/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_N,  2,  "Day of Dataset Collection",       "DAY" },
    {NITF_BCS_A,  3,  "Month of Dataset Collection",     "MONTH" },
    {NITF_BCS_N,  4,  "Year of Dataset Collection",      "YEAR" },
    {NITF_BCS_A,  2,  "Platform Identification",         "PLATFORM_CODE" },
    {NITF_BCS_N,  2,  "Vehicle Number",                  "VEHICLE_ID" },
    {NITF_BCS_N,  2,  "Pass Number",                     "PASS" },
    {NITF_BCS_N,  3,  "Operation Number",                "OPERATION" },
    {NITF_BCS_A,  2,  "Sensor ID",                       "SENSOR_ID" },
    {NITF_BCS_A,  2,  "Product ID",                      "PRODUCT_ID" },
    {NITF_BCS_A,  4,  "Reserved",                        "RESERVED_1" },
    {NITF_BCS_N, 14,  "Image Start Time",                "TIME" },
    {NITF_BCS_N, 14,  "Process Completion Time",         "PROCESS_TIME" },
    {NITF_BCS_N,  2,  "Reserved",                        "RESERVED_2" },
    {NITF_BCS_N,  2,  "Reserved",                        "RESERVED_3" },
    {NITF_BCS_A,  1,  "Reserved",                        "RESERVED_4" },
    {NITF_BCS_A,  1,  "Reserved",                        "RESERVED_5" },
    {NITF_BCS_A, 10,  "Software Version Used",           "SOFTWARE_VERSION_NUMBER" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(CSDIDA, description)

NITF_CXX_ENDGUARD

