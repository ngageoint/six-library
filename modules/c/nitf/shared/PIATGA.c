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
    {NITF_BCS_A, 15, "Target UTM", "TGTUTM" },
    {NITF_BCS_A, 15, "Target ID", "PIATGAID" },
    {NITF_BCS_A, 2, "Country Code", "PIACTRY" },
    {NITF_BCS_A, 5, "Category Code", "PIACAT" },
    {NITF_BCS_A, 15, "Target Geographic Coordinates", "TGTGEO" },
    {NITF_BCS_A, 3, "Target Coordinate Datum", "DATUM" },
    {NITF_BCS_A, 38, "Target Name", "TGTNAME" },
    {NITF_BCS_N, 3, "Percent Coverage", "PERCOVER" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(PIATGA, description)

NITF_CXX_ENDGUARD
