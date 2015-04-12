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

   /* from ACFTA ver 1.0 */
static nitf_TREDescription descrip_00132[] = {
    {NITF_BCS_A, 10, "Aircraft Mission ID", "AC_MSN_ID" },
    {NITF_BCS_A, 1, "Scene Type", "SCTYPE" },
    {NITF_BCS_A, 4, "Scene No.", "SCNUM"},
    {NITF_BCS_A, 3, "Sensor ID", "SENSOR_ID"},
    {NITF_BCS_A, 4, "Total No. of Patches", "PATCH_TOT"},
    {NITF_BCS_A, 3, "Total No. of MTI Packets", "MTI_TOT"},
    {NITF_BCS_A, 7, "Processing Date", "PDATE"},
    {NITF_BCS_A, 3, "Immediate Scene Host", "IMHOSTNO"},
    {NITF_BCS_A, 5, "Immediate Scene Req ID", "IMREQID"},
    {NITF_BCS_A, 1, "Scene Source", "SCENE_SOURCE"},
    {NITF_BCS_A, 2, "Mission Plan Mode", "MPLAN"},
    {NITF_BCS_A, 21, "Entry Location", "ENTLOC"},
    {NITF_BCS_A, 6, "Entry Altitude", "ENTALT"},
    {NITF_BCS_A, 21, "Exit Location", "EXITLOC"},
    {NITF_BCS_A, 6, "Exit Altitude", "EXITALT"},
    {NITF_BCS_A, 7, "True Map Angle", "TMAP"},
    {NITF_BCS_A, 3, "RCS Calibration Coef.", "RCS"},
    {NITF_BCS_A, 7, "Row Spacing", "ROW_SPACING"},
    {NITF_BCS_A, 7, "Col Spacing", "COL_SPACING"},
    {NITF_BCS_A, 4, "Sensor Serial No.", "SENSERIAL"},
    {NITF_BCS_A, 7, "Airborne SW Version", "ABSWVER"},
    {NITF_END, 0, NULL, NULL}
};

   /* from draft ACFTA ver 0.9a */
static nitf_TREDescription descrip_00154[] = {
    {NITF_BCS_A, 10, "Aircraft Mission ID", "AC_MSN_ID"},
    {NITF_BCS_A, 10, "Aircraft Tail Number", "AC_TAIL_NO"},
    {NITF_BCS_A, 10, "Sensor ID", "SENSOR_ID"},
    {NITF_BCS_A, 1, "Scene Source", "SCENE_SOURCE"},
    {NITF_BCS_A, 6, "Scene No.", "SCNUM"},
    {NITF_BCS_A, 8, "Processing Date", "PDATE"},
    {NITF_BCS_A, 6, "Immediate Scene Host", "IMHOSTNO"},
    {NITF_BCS_A, 5, "Immediate Scene Req ID", "IMREQID"},
    {NITF_BCS_A, 3, "Mission Plan Mode", "MPLAN"},
    {NITF_BCS_A, 21, "Entry Location", "ENTLOC"},
    {NITF_BCS_A, 6, "Entry Altitude", "ENTALT"},
    {NITF_BCS_A, 21, "Exit Location", "EXITLOC"},
    {NITF_BCS_A, 6, "Exit Altitude", "EXITALT"},
    {NITF_BCS_A, 7, "True Map Angle", "TMAP"},
    {NITF_BCS_A, 7, "Row Spacing", "ROW_SPACING"},
    {NITF_BCS_A, 7, "Col Spacing", "COL_SPACING"},
    {NITF_BCS_A, 6, "Sensor Serial No.", "SENSERIAL"},
    {NITF_BCS_A, 7, "Airborne SW Version", "ABSWVER"},
    {NITF_BCS_A, 4, "Total No. of Patches", "PATCH_TOT"},
    {NITF_BCS_A, 3, "Total No. of MTI Packets", "MTI_TOT"},
    {NITF_END, 0, NULL, NULL}
};

   /* as found version on newly processed ASARS2 legacy images */
   /* marked ACFTA, though seems to be from a draft of ACFTB   */
static nitf_TREDescription descrip_00199[] = {
    {NITF_BCS_A, 20, "Aircraft Mission ID", "AC_MSN_ID"},
    {NITF_BCS_A, 10, "Aircraft Tail Number", "AC_TAIL_NO"},
    {NITF_BCS_A, 12, "Acrft Takeoff Date/Time", "AC_TO"},
    {NITF_BCS_A, 4, "Sensor ID Type", "SENSOR_ID_TYPE"},
    {NITF_BCS_A, 6, "Sensor ID", "SENSOR_ID"},
    {NITF_BCS_N, 1, "Scene Source", "SCENE_SOURCE"},
    {NITF_BCS_N, 6, "Scene No.", "SCNUM"},
    {NITF_BCS_N, 8, "Processing Date", "PDATE"},
    {NITF_BCS_N, 6, "Immediate Scene Host", "IMHOSTNO"},
    {NITF_BCS_N, 5, "Immediate Scene Req ID", "IMREQID"},
    {NITF_BCS_N, 3, "Mission Plan Mode", "MPLAN"},
    {NITF_BCS_A, 25, "Entry Location", "ENTLOC"},
    {NITF_BCS_A, 6, "Entry Elevation", "ENTELV"},
    {NITF_BCS_A, 1, "Elevation Units", "ELVUNIT"},
    {NITF_BCS_A, 25, "Exit Location", "EXITLOC"},
    {NITF_BCS_A, 6, "Exit Elevation", "EXITELV"},
    {NITF_BCS_A, 7, "True Map Angle", "TMAP"},
    {NITF_BCS_A, 1, "spacing units?", "RESERVD1"},
    {NITF_BCS_A, 7, "Row Spacing", "ROW_SPACING"},
    {NITF_BCS_A, 7, "Col Spacing", "COL_SPACING"},
    {NITF_BCS_A, 6, "Sensor Focal Length", "FOCAL_LENGTH"},
    {NITF_BCS_A, 6, "Sensor Serial No.", "SENSERIAL" },
    {NITF_BCS_A, 7, "Airborne SW Version", "ABSWVER"},
    {NITF_BCS_A, 8, "Calibration Date", "CAL_DATE"},
    {NITF_BCS_N, 4, "Total No. of Patches", "PATCH_TOT"},
    {NITF_BCS_A, 3, "Total No. of MTI Packets", "MTI_TOT"},
    {NITF_END, 0, NULL, NULL}
};

/* Define the available descriptions and the default one */
static nitf_TREDescriptionInfo descriptions[] = {
    { "ACFTA_132", descrip_00132, 132 },
    { "ACFTA_154", descrip_00154, 154 },
    { "ACFTA_199", descrip_00199, 199 },
    { NULL, NULL, NITF_TRE_DESC_NO_LENGTH }
};
static nitf_TREDescriptionSet descriptionSet = { 0, descriptions };


#if 1
NITF_DECLARE_PLUGIN(ACFTA)
#else
static char *ident[] = { NITF_PLUGIN_TRE_KEY, "ACFTA", NULL }; 
    

static nitf_TREHandler* acftaHandler = NULL;
NITFAPI(char**) ACFTA_init(nitf_Error* error)
{
	// Init here!
	acftaHandler = nitf_TREUtils_createBasicHandler(&descriptionSet, error);
	if (!acftaHandler)
		return NULL;

	return ident;
} 
    
NITFAPI(void) ACFTA_cleanup(void){}
#endif






NITF_CXX_ENDGUARD
