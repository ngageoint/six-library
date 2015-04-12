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

#include <import/nitf.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_N, 2, "Number of vertical accuracy regions",
     "NUMACVT" },
    {NITF_LOOP, 0, NULL, "NUMACVT"},
    {NITF_BCS_A, 3, "Unit of Measure for AAV", "UNIAAV" },
    {NITF_IF, 0, "ne    ", "UNIAAV"},
    {NITF_BCS_N, 5, "Absolute Vertical Accuracy", "AAV" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_BCS_A, 3, "Unit of Measure for APV", "UNIAPV" },
    {NITF_IF, 0, "ne    ", "UNIAPV"},
    {NITF_BCS_N, 5, "Point-to-Point Vertical Accuracy", "APV" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_BCS_N, 3, "Number of Points in Bounding Polygon",
     "NUMPTS" },
    {NITF_LOOP, 0, NULL, "NUMPTS"},
    {NITF_BCS_N, 15, "Longitude/Easting", "LON" },
    {NITF_BCS_N, 15, "Latitude/Northing", "LAT" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(ACCVTB, description)

NITF_CXX_ENDGUARD
