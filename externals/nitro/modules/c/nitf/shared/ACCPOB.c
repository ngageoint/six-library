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

#if _MSC_VER
#pragma warning(disable: 4820) // '...' : '...' bytes padding added after data member '...'
#pragma warning(disable: 4668) // '...' is not defined as a preprocessor macro, replacing with '...' for '...'
#endif

#include <import/nitf.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_N, 2, "Number of positional accuracy regions",
     "NUMACPO" },
    {NITF_LOOP, 0, NULL, "NUMACPO"},
    {NITF_BCS_A, 3, "Unit of Measure for AAH", "UNIAAH" },
    {NITF_BCS_N, 5, "Absolute Horizontal Accuracy", "AAH" },
    {NITF_BCS_A, 3, "Unit of Measure for AAV", "UNIAAV" },
    {NITF_BCS_N, 5, "Absolute Vertical Accuracy", "AAV" },
    {NITF_BCS_A, 3, "Unit of Measure for APH", "UNIAPH" },
    {NITF_BCS_N, 5, "Point-to-Point Horizontal Accuracy", "APH" },
    {NITF_BCS_A, 3, "Unit of Measure for APV", "UNIAPV" },
    {NITF_BCS_N, 5, "Point-to-Point Vertical Accuracy", "APV" },
    {NITF_BCS_N, 3, "Number of Points in Bounding Polygon", "NUMPTS" },
    {NITF_LOOP, 0, NULL, "NUMPTS"},
    {NITF_BCS_N, 15, "Longitude/Easting", "LON" },
    {NITF_BCS_N, 15, "Latitude/Northing", "LAT" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(ACCPOB, description)

NITF_CXX_ENDGUARD
