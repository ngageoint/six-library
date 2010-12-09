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
    {NITF_BCS_A, 20, "Aircraft Mission ID", "AC_MSN_ID" },
    {NITF_BCS_A, 10, "Aircraft Tail Number", "AC_TAIL_NO" },
    {NITF_BCS_A, 12, "Acrft Takeoff Date/Time", "AC_TO" },
    {NITF_BCS_A, 4, "Sensor ID Type", "SENSOR_ID_TYPE" },
    {NITF_BCS_A, 6, "Sensor ID", "SENSOR_ID" },
    {NITF_BCS_N, 1, "Scene Source", "SCENE_SOURCE" },
    {NITF_BCS_N, 6, "Scene No.", "SCNUM" },
    {NITF_BCS_N, 8, "Processing Date", "PDATE" },
    {NITF_BCS_N, 6, "Immediate Scene Host", "IMHOSTNO" },
    {NITF_BCS_N, 5, "Immediate Scene Req ID", "IMREQID" },
    {NITF_BCS_N, 3, "Mission Plan Mode", "MPLAN" },
    {NITF_BCS_A, 25, "Entry Location", "ENTLOC" },
    {NITF_BCS_A, 6, "Location Accuracy", "LOC_ACCY" },
    {NITF_BCS_A, 6, "Entry Elevation", "ENTELV" },
    {NITF_BCS_A, 1, "Elevation Units", "ELV_UNIT" },
    {NITF_BCS_A, 25, "Exit Location", "EXITLOC" },
    {NITF_BCS_A, 6, "Exit Elevation", "EXITELV" },
    {NITF_BCS_A, 7, "True Map Angle", "TMAP" },
    {NITF_BCS_A, 7, "Row Spacing", "ROW_SPACING" },
    {NITF_BCS_A, 1, "Row Spacing Units", "ROW_SPACING_UNITS" },
    {NITF_BCS_A, 7, "Col Spacing", "COL_SPACING" },
    {NITF_BCS_A, 1, "Col Spacing Units", "COL_SPACING_UNITS" },
    {NITF_BCS_A, 6, "Sensor Focal Length", "FOCAL_LENGTH" },
    {NITF_BCS_A, 6, "Sensor Serial No.", "SENSERIAL" },
    {NITF_BCS_A, 7, "Airborne Software Version", "ABSWVER" },
    {NITF_BCS_A, 8, "Calibration Date", "CAL_DATE" },
    {NITF_BCS_N, 4, "Total Number of Patches", "PATCH_TOT" },
    {NITF_BCS_N, 3, "Total Number of MTIRP Extensions", "MTI_TOT" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(ACFTB, description)

NITF_CXX_ENDGUARD
