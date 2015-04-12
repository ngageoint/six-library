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
    {NITF_BCS_N, 2, "Number of Location Grids", "NUM_GRDS" },

    {NITF_LOOP, 0, NULL, "NUM_GRDS"},

    {NITF_BCS_N, 10, "Location Grid Elevation", "ZVL"},
    {NITF_BCS_A, 10, "Location Grid ID", "BAD"},
    {NITF_BCS_N, 12, "Data Density in Columns", "LOD"},
    {NITF_BCS_N, 12, "Data Density in Rows", "LAD"},
    {NITF_BCS_N, 11, "Origin in Columns", "LSO"},
    {NITF_BCS_N, 11, "Origin in Rows", "PSO"},

    {NITF_ENDLOOP, 0, NULL, NULL },

    {NITF_END, 0, NULL, NULL },
};

NITF_DECLARE_SINGLE_PLUGIN(GRDPSB, description)

NITF_CXX_ENDGUARD
