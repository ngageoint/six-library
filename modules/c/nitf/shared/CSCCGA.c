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
    {NITF_BCS_A, 18, "Source of Grid", "CCG_SOURCE" },
    {NITF_BCS_A, 6, "Image Segment Sensor to which CCG is registered", "REG_SENSOR" },
    {NITF_BCS_N, 7, "CCG Origin - Line", "ORIGIN_LINE" },
    {NITF_BCS_N, 5, "CCG Origin - Sample", "ORIGIN_SAMPLE" },
    {NITF_BCS_N, 7, "Along Scan Cell Size - Lines", "AS_CELL_SIZE" },
    {NITF_BCS_N, 5, "Cross Scan Cell Size - Samples", "CS_CELL_SIZE" },
    {NITF_BCS_N, 7, "Number of Rows in CCG", "CCG_MAX_LINE" },
    {NITF_BCS_N, 5, "Number of Columns in CCG", "CCG_MAX_SAMPLE" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(CSCCGA, description)

NITF_CXX_ENDGUARD
