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


static nitf_TREDescription descrip_00087[] = {
    {NITF_BCS_A, 3, "Angle to True North", "ANGLE_TO_NORTH" },
    {NITF_BCS_A, 3, "Squint Angle", "SQUINT_ANGLE" },
    {NITF_BCS_A, 3, "Imaging Mode", "MODE" },
    {NITF_BCS_A, 16, "reserved 1", "RESVD001" },
    {NITF_BCS_A, 2, "FP Grazing Angle", "GRAZE_ANG" },
    {NITF_BCS_A, 2, "FP Slope Angle", "SLOPE_ANG" },
    {NITF_BCS_A, 2, "Polarization", "POLAR" },
    {NITF_BCS_A, 5, "Pixels per Line", "NSAMP" },
    {NITF_BCS_A, 1, "reserved 2", "RESVD002" },
    {NITF_BCS_A, 1, "Sequence Number", "SEQ_NUM" },
    {NITF_BCS_A, 12, "Primary Target ID", "PRIME_ID" },
    {NITF_BCS_A, 15, "Primary Target BE Number", "PRIME_BE" },
    {NITF_BCS_A, 1, "reserved 3", "RESVD003" },
    {NITF_BCS_A, 2, "Number of Secondary Targets", "N_SEC" },
    {NITF_BCS_A, 2, "Commanded IPR", "IPR" },
    {NITF_BCS_A, 2, "reserved 4", "RESVD004" },
    {NITF_BCS_A, 2, "reserved 5", "RESVD005" },
    {NITF_BCS_A, 5, "reserved 6", "RESVD006" },
    {NITF_BCS_A, 8, "reserved 7", "RESVD007" },
    {NITF_END, 0, NULL, NULL}
};

static nitf_TREDescription descrip_00101[] = {
    {NITF_BCS_A, 7, "Angle to True North", "ANGLE_TO_NORTH" },
    {NITF_BCS_A, 7, "Squint Angle", "SQUINT_ANGLE" },
    {NITF_BCS_A, 3, "Imaging Mode", "MODE" },
    {NITF_BCS_A, 16, "reserved 1", "RESVD001" },
    {NITF_BCS_A, 5, "FP Grazing Angle", "GRAZE_ANG" },
    {NITF_BCS_A, 5, "FP Slope Angle", "SLOPE_ANG" },
    {NITF_BCS_A, 2, "Polarization", "POLAR" },
    {NITF_BCS_A, 5, "Pixels per Line", "NSAMP" },
    {NITF_BCS_A, 1, "reserved 2", "RESVD002" },
    {NITF_BCS_A, 1, "Sequence Number", "SEQ_NUM" },
    {NITF_BCS_A, 12, "Primary Target ID", "PRIME_ID" },
    {NITF_BCS_A, 15, "Primary Target BE Number", "PRIME_BE" },
    {NITF_BCS_A, 1, "reserved 3", "RESVD003" },
    {NITF_BCS_A, 2, "Number of Secondary Target", "N_SEC" },
    {NITF_BCS_A, 2, "Commanded IPR", "IPR" },
    {NITF_END, 0, NULL, NULL}
};

/* Define the available descriptions and the default one */
static nitf_TREDescriptionInfo descriptions[] = {
    { "EXPLTA_87", descrip_00087, 87 },
    { "EXPLTA_101", descrip_00101, 101 },
    { NULL, NULL, NITF_TRE_DESC_NO_LENGTH }
};
static nitf_TREDescriptionSet descriptionSet = { 0, descriptions };

NITF_DECLARE_PLUGIN(EXPLTA)

NITF_CXX_ENDGUARD
