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

/*
 * FIXME, these names dont seem right
 */
#include <import/nitf.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_A, 2, "Image Block Number", "blkno" },
    {NITF_BCS_A, 2, "Commanded IPR", "cdipr" },
    {NITF_BCS_N, 2, "No of Blocks in WDG", "nblkw" },
    {NITF_BCS_N, 5, "No of Rows in Block", "nrblk" },
    {NITF_BCS_N, 5, "No of Cols in Block", "ncblk" },
    {NITF_BCS_A, 9, "Output Ref Pt X (ECF)", "orpx" },
    {NITF_BCS_A, 9, "Output Ref Pt Y (ECF)", "orpy" },
    {NITF_BCS_A, 9, "Output Ref Pt Z (ECF)", "orpz" },
    {NITF_BCS_N, 5, "Row of ORP", "orpro" },
    {NITF_BCS_N, 5, "Col of ORP", "orpco" },
    {NITF_BCS_A, 7, "FP Normal Vector X", "fpnvx" },
    {NITF_BCS_A, 7, "FP Normal Vector Y", "fpnvy" },
    {NITF_BCS_A, 7, "FP Normal Vector Z", "fpnvz" },
    {NITF_BCS_A, 9, "Collection Start Time", "arptm" },
    {NITF_BCS_A, 14, "reserved 1", "resv1" },
    {NITF_BCS_A, 9, "Aircraft Position N", "arppn" },
    {NITF_BCS_A, 9, "Aircraft Position E", "arppe" },
    {NITF_BCS_A, 9, "Aircraft Position D", "arppd" },
    {NITF_BCS_A, 9, "Aircraft Velocity N", "arpvn" },
    {NITF_BCS_A, 9, "Aircraft Velocity E", "arpve" },
    {NITF_BCS_A, 9, "Aircraft Velocity D", "arpvd" },
    {NITF_BCS_A, 8, "Aircraft Accel N", "arpan" },
    {NITF_BCS_A, 8, "Aircraft Accel E", "arpae" },
    {NITF_BCS_A, 8, "Aircraft Accel D", "arpad" },
    {NITF_BCS_A, 13, "reserved 2", "resv2" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(MPDSRA, description)

NITF_CXX_ENDGUARD
