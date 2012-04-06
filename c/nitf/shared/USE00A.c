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
    {NITF_BCS_N, 3, "Angle to North", "ANGLE_TO_NORTH" },
    {NITF_BCS_A, 5, "Mean GSD     ", "MEAN_GSD" },
    {NITF_BCS_A, 1, "Reserved 1", "rsrvd01" },
    {NITF_BCS_N, 5, "Dynamic Range", "DYNAMIC_RANGE" },
    {NITF_BCS_A, 3, "Reserved 2", "rsrvd02" },
    {NITF_BCS_A, 1, "Reserved 3", "rsrvd03" },
    {NITF_BCS_A, 3, "Reserved 4", "rsrvd04" },
    {NITF_BCS_A, 5, "Obliquity Angle", "OBL_ANG" },
    {NITF_BCS_A, 6, "Roll Angle", "ROLL_ANG" },
    {NITF_BCS_A, 12, "Reserved 5", "rsrvd05" },
    {NITF_BCS_A, 15, "Reserved 6", "rsrvd06" },
    {NITF_BCS_A, 4, "Reserved 7", "rsrvd07" },
    {NITF_BCS_A, 1, "Reserved 8", "rsrvd08" },
    {NITF_BCS_A, 3, "Reserved 9", "rsrvd09" },
    {NITF_BCS_A, 1, "Reserved 10", "rsrvd10" },
    {NITF_BCS_A, 1, "Reserved 11", "rsrvd11" },
    {NITF_BCS_N, 2, "Number of Reference Lines", "N_REF" },
    {NITF_BCS_N, 5, "Revolution Number", "REV_NUM" },
    {NITF_BCS_N, 3, "Number of Segments", "N_SEG" },
    {NITF_BCS_N, 6, "Max Lines per Segment", "MAX_LP_SEG" },
    {NITF_BCS_A, 6, "Reserved 12", "rsrvd12" },
    {NITF_BCS_A, 6, "Reserved 13", "rsrvd13" },
    {NITF_BCS_A, 5, "Sun Elevation", "SUN_EL" },
    {NITF_BCS_A, 5, "Sun Azimuth", "SUN_AZ" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(USE00A, description)

NITF_CXX_ENDGUARD
