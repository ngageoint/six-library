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


static nitf_TREDescription descrip_00155[] = {
    {NITF_BCS_A, 21, "CCRP Location", "CCRP_LOC" },
    {NITF_BCS_A, 6, "CCRP Altitude", "CCRP_ALT" },
    {NITF_BCS_A, 7, "Range Offset Between CCRP and Patch Center", "OF_PC_R" },
    {NITF_BCS_A, 7, "Azimuth Offset Between CCRP and Patch Center", "OF_PC_A" },
    {NITF_BCS_A, 7, "Cosine of Grazing Angle", "COSGRZ" },
    {NITF_BCS_A, 7, "Range to CCRP", "RGCCRP" },
    {NITF_BCS_A, 1, "Right/Left Map", "RLMAP" },
    {NITF_BCS_A, 5, "CCRP Row Number", "CCRP_ROW" },
    {NITF_BCS_A, 5, "CCRP Col Number", "CCRP_COL" },
    {NITF_BCS_A, 21, "Aircraft Location", "ACFT_LOC" },
    {NITF_BCS_A, 5, "Aircraft Altitude", "ACFT_ALT" },
    {NITF_BCS_A, 7, "Range Unit Vector North", "C_R_NC" },
    {NITF_BCS_A, 7, "Range Unit Vector East", "C_R_EC" },
    {NITF_BCS_A, 7, "Range Unit Vector Down", "C_R_DC" },
    {NITF_BCS_A, 7, "Azimuth Unit Vect North", "C_AZ_NC" },
    {NITF_BCS_A, 7, "Azimuth Unit Vect East", "C_AZ_EC" },
    {NITF_BCS_A, 7, "Azimuth Unit Vect Down", "C_AZ_DC" },
    {NITF_BCS_A, 7, "Altitude North", "C_AL_NC" },
    {NITF_BCS_A, 7, "Altitude East", "C_AL_EC" },
    {NITF_BCS_A, 7, "Altitude Down", "C_AL_DC" },
    {NITF_END, 0, NULL, NULL}
};

static nitf_TREDescription descrip_00174[] = {
    {NITF_BCS_A, 21, "Aircraft Location", "ACFT_LOC" },
    {NITF_BCS_A, 6, "Aircraft Altitude", "ACFT_ALT" },
    {NITF_BCS_A, 21, "CCRP Location", "CCRP_LOC" },
    {NITF_BCS_A, 6, "CCRP Altitude", "CCRP_ALT" },
    {NITF_BCS_A, 7, "Range Offset Between CCRP and Patch Center", "OF_PC_R" },
    {NITF_BCS_A, 7, "Azimuth Offset Between CCRP and Patch Center", "OF_PC_A" },
    {NITF_BCS_A, 7, "Cos of Grazing Angle", "COSGRZ" },
    {NITF_BCS_A, 7, "Range to CCRP", "RGCCRP" },
    {NITF_BCS_A, 1, "Right/Left Map", "RLMAP" },
    {NITF_BCS_A, 5, "CCRP Row Number", "CCRP_ROW" },
    {NITF_BCS_A, 5, "CCRP Col Number", "CCRP_COL" },
    {NITF_BCS_A, 9, "Range Unit Vector North", "C_R_NC" },
    {NITF_BCS_A, 9, "Range Unit Vector East", "C_R_EC" },
    {NITF_BCS_A, 9, "Range Unit Vector Down", "C_R_DC" },
    {NITF_BCS_A, 9, "Azimuth Unit Vect North", "C_AZ_NC" },
    {NITF_BCS_A, 9, "Azimuth Unit Vect East", "C_AZ_EC" },
    {NITF_BCS_A, 9, "Azimuth Unit Vect Down", "C_AZ_DC" },
    {NITF_BCS_A, 9, "Altitude North", "C_AL_NC" },
    {NITF_BCS_A, 9, "Altitude East", "C_AL_EC" },
    {NITF_BCS_A, 9, "Altitude Down", "C_AL_DC" },
    {NITF_END, 0, NULL, NULL}
};

static nitf_TREDescription descrip_00185[] = {
    {NITF_BCS_A, 25, "Aircraft Location", "ACFT_LOC" },
    {NITF_BCS_A, 6, "Aircraft Altitude", "ACFT_ALT" },
    {NITF_BCS_A, 25, "CCRP Location", "CCRP_LOC" },
    {NITF_BCS_A, 6, "CCRP Altitude", "CCRP_ALT" },
    {NITF_BCS_A, 7, "Range Offset Between CCRP and Patch Center", "OF_PC_R" },
    {NITF_BCS_A, 7, "Azimuth Offset Between CCRP and Patch Center", "OF_PC_A" },
    {NITF_BCS_A, 7, "Cos of Grazing Angle", "COSGZ" },
    {NITF_BCS_A, 7, "Range to CCRP", "RGCCRP" },
    {NITF_BCS_A, 1, "Right/Left Map", "RLMAP" },
    {NITF_BCS_A, 5, "CCRP Row Number", "CCRP_ROW" },
    {NITF_BCS_A, 5, "CCRP Col Number", "CCRP_COL" },
    {NITF_BCS_A, 10, "Range Unit Vector North", "C_R_NC" },
    {NITF_BCS_A, 10, "Range Unit Vector East", "C_R_EC" },
    {NITF_BCS_A, 10, "Range Unit Vector Down", "C_R_DC" },
    {NITF_BCS_A, 9, "Azimuth Unit Vect North", "C_AZ_NC" },
    {NITF_BCS_A, 9, "Azimuth Unit Vect East", "C_AZ_EC" },
    {NITF_BCS_A, 9, "Azimuth Unit Vect Down", "C_AZ_DC" },
    {NITF_BCS_A, 9, "Altitude North", "C_AL_NC" },
    {NITF_BCS_A, 9, "Altitude East", "C_AL_EC" },
    {NITF_BCS_A, 9, "Altitude Down", "C_AL_DC" },
    {NITF_BCS_A, 3, "Total Number of Tiles in Column Direction",
     "TOTAL_TILES_COLS" },
    {NITF_BCS_A, 5, "Total Number of Tiles in Row direction",
     "TOTAL_TILES_ROWS" },
    {NITF_END, 0, NULL, NULL}
};

/* Define the available descriptions and the default one */
static nitf_TREDescriptionInfo descriptions[] = {
    { "MENSRA_155", descrip_00155, 155 },
    { "MENSRA_174", descrip_00174, 174 },
    { "MENSRA_185", descrip_00185, 185 },
    { NULL, NULL, NITF_TRE_DESC_NO_LENGTH }
};
static nitf_TREDescriptionSet descriptionSet = { 0, descriptions };

NITF_DECLARE_PLUGIN(MENSRA)

NITF_CXX_ENDGUARD
