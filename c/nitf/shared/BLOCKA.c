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
    {NITF_BCS_N, 2, "Block Number", "BLOCK_INSTANCE" },
    {NITF_BCS_A, 5, "No. of Gray Pixels", "N_GRAY" },
    {NITF_BCS_N, 5, "Lines", "L_LINES" },
    {NITF_BCS_A, 3, "Layover Angle", "LAYOVER_ANGLE" },
    {NITF_BCS_A, 3, "Shadow Angle", "SHADOW_ANGLE" },
    {NITF_BCS_A, 16, "reserved 1", "RESERVED-001" },
    {NITF_BCS_A, 21, "FRLC Location", "FRLC_LOC" },
    {NITF_BCS_A, 21, "LRLC Location", "LRLC_LOC" },
    {NITF_BCS_A, 21, "LRFC Location", "LRFC_LOC" },
    {NITF_BCS_A, 21, "FRFC Location", "FRFC_LOC" },
    {NITF_BCS_A, 5, "reserved 2", "RESERVED-002" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(BLOCKA, description)

NITF_CXX_ENDGUARD
