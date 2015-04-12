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
    {NITF_BCS_A, 25, "Aircraft Location", "ACFT_LOC" },
    {NITF_BCS_A, 6, "Aircraft Location Accuracy", "ACFT_LOC_ACCY" },
    {NITF_BCS_A, 6, "Aircraft Altitude", "ACFT_ALT" },
    {NITF_BCS_A, 25, "Reference Point Location", "RP_LOC" },
    {NITF_BCS_A, 6, "Reference Point Location Accuracy", "RP_LOC_ACCY" },
    {NITF_BCS_A, 6, "Reference Point Elevation", "RP_ELV" },
    {NITF_BCS_A, 7, "Range Offset", "OF_PC_R" },
    {NITF_BCS_A, 7, "Azimuth Offset", "OF_PC_A" },
    {NITF_BCS_A, 7, "Cosine of Grazing Angle", "COSGRZ" },
    {NITF_BCS_A, 7, "Estimated Slant Range", "RGCRP" },
    {NITF_BCS_A, 1, "Right/Left Map", "RLMAP" },
    {NITF_BCS_A, 5, "RP Row Number", "RP_ROW" },
    {NITF_BCS_A, 5, "RP Column Number", "RP_COL" },
    {NITF_BCS_A, 10, "Range Unit Vector North","C_R_NC" },
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
    {NITF_BCS_A, 5, "Total Number of Tiles in Row Direction",
     "TOTAL_TILES_ROWS" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(MENSRB, description)

NITF_CXX_ENDGUARD
