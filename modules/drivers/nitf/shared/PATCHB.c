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
    
    {NITF_BCS_A, 4, "Patch Number", "PAT_NO" },
    {NITF_BCS_A, 1, "Last Patch of Search Scene", "LAST_PAT_FLAG" },
    {NITF_BCS_A, 7, "Patch Start Line", "LNSTRT" },
    {NITF_BCS_A, 7, "Patch End Line", "LNSTOP" },
    {NITF_BCS_A, 5, "Number of Azimuth Lines", "AZL" },
    {NITF_BCS_A, 5, "Number of Valid Azimuth Lines", "NVL" },
    {NITF_BCS_A, 3, "First Valid Line", "FVL" },
    {NITF_BCS_A, 5, "Number of Pixels per Line", "NPIXEL" },
    {NITF_BCS_A, 5, "First Valid Pixel Index", "FVPIX" },
    {NITF_BCS_A, 3, "Spot Frame Number", "FRAME" },
    {NITF_BCS_A, 8, "Coordinated Universal Time", "UTC" },
    {NITF_BCS_A, 7, "Scene Heading", "SHEAD" },
    {NITF_BCS_A, 7, "Local Gravity", "GRAVITY" },
    {NITF_BCS_A, 5, "Ins Platform Velocity, North", "INS_V_NC" },
    {NITF_BCS_A, 5, "Ins Platform Velocity, East", "INS_V_EC" },
    {NITF_BCS_A, 5, "Ins Platform Velocity, Down", "INS_V_DC" },
    {NITF_BCS_A, 8, "Geodetic Latitude Offset", "OFFLAT" },
    {NITF_BCS_A, 8, "Geodetic Longitude Offset", "OFFLONG" },
    {NITF_BCS_A, 3, "Track Heading", "TRACK" },
    {NITF_BCS_A, 6, "Ground Sweep Angle", "GSWEEP" },
    {NITF_BCS_A, 8, "Patch Shear Factor", "SHEAR" },
    {NITF_BCS_A, 6, "Batch Number", "BATCH_NO" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(PATCHB, description)

NITF_CXX_ENDGUARD
