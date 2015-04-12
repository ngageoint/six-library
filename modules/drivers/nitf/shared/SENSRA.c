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
    {NITF_BCS_N, 8, "Ref Row", "REFROW" },
    {NITF_BCS_N, 8, "Ref Col", "REFCOL" },
    {NITF_BCS_A, 6, "Sensor Model", "SNSMODEL" },
    {NITF_BCS_A, 3, "Sensor Mount", "SNSMOUNT" },
    {NITF_BCS_A, 21, "Sensor Loc", "SENSLOC" },
    {NITF_BCS_A, 1, "Sensor Alt Src", "SNALTSRC" },
    {NITF_BCS_A, 6, "Sensor Alt", "SENSALT" },
    {NITF_BCS_A, 1, "Sensor Alt Unit", "SNALUNIT" },
    {NITF_BCS_A, 5, "Sensor AGL", "SENSAGL" },
    {NITF_BCS_A, 7, "Sensor Pitch", "SNSPITCH" },
    {NITF_BCS_A, 8, "Sensor Roll", "SENSROLL" },
    {NITF_BCS_A, 8, "Sensor Yaw", "SENSYAW" },
    {NITF_BCS_A, 7, "Platform Pitch", "PLTPITCH" },
    {NITF_BCS_A, 8, "Platform Roll", "PLATROLL" },
    {NITF_BCS_A, 5, "Platform Hdg", "PLATHDG" },
    {NITF_BCS_A, 1, "Ground Spd Src", "GRSPDSRC" },
    {NITF_BCS_A, 6, "Ground Speed", "GRDSPEED" },
    {NITF_BCS_A, 1, "Ground Spd Unit", "GRSPUNIT" },
    {NITF_BCS_A, 5, "Ground Track", "GRDTRACK" },
    {NITF_BCS_A, 5, "Vertical Vel", "VERTVEL" },
    {NITF_BCS_A, 1, "Vert Vel Unit", "VERTVELU" },
    {NITF_BCS_A, 4, "Swath Frames", "SWATHFRM" },
    {NITF_BCS_N, 4, "N Swaths", "NSWATHS" },
    {NITF_BCS_N, 3, "Spot Num", "SPOTNUM" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(SENSRA, description)

NITF_CXX_ENDGUARD
