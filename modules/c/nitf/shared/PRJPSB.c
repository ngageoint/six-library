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
    {NITF_BCS_A, 80, "Projection Name", "PRN" },
    {NITF_BCS_A, 2, "Projection Code", "PCO" },
    {NITF_BCS_N, 1, "Number of Projection Parameters", "NUMPRJ" },
    {NITF_LOOP, 0, NULL, "NUMPRJ"},
    {NITF_BCS_N, 15, "Projection Parameter", "PRJ" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_N, 15, "Projection False X (Easting) Origin", "XOR" },
    {NITF_BCS_N, 15, "Projection False Y (Northing) Origin" "YOR" }, 
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(PRJPSB, description)

NITF_CXX_ENDGUARD
