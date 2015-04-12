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

static nitf_TREDescription descrip_00006[] = {
    {NITF_BCS_N, 3, "Band Identifier", "BAND_NUMBER" },
    {NITF_BCS_N, 1, "Mapping Method to Apply", "MAP_SELECT" },
    {NITF_BCS_N, 2, "Scale Factor 2", "S2" },
    {NITF_END, 0, NULL, NULL}
};

static nitf_TREDescription descrip_08202[] = {
    {NITF_BCS_N, 3, "Band Identifier", "BAND_NUMBER" },
    {NITF_BCS_N, 1, "Mapping Method to Apply", "MAP_SELECT" },
    {NITF_BCS_N, 2, "I/O TABLE USED", "TABLE_ID" },
    {NITF_BCS_N, 2, "Scale Factor 1", "S1" },
    {NITF_BCS_N, 2, "Scale Factor 2", "S2" },
    {NITF_LOOP, 0, NITF_CONST_N, "4096"},
    {NITF_BCS_N, 2, "Output Mapping Value", "OUTPUT_MAP_VALUE" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

static nitf_TREDescription descrip_00016[] = {
    {NITF_BCS_N, 3, "Band Identifier", "BAND_NUMBER" },
    {NITF_BCS_N, 1, "Mapping Method to Apply", "MAP_SELECT" },
    {NITF_BCS_N, 2, "I/O TABLE USED", "TABLE_ID" },
    {NITF_BCS_N, 2, "Scale Factor 1", "S1" },
    {NITF_BCS_N, 2, "Scale Factor 2", "S2" },
    {NITF_BCS_N, 3, "R Scaling Factor � Whole Part", "R_WHOLE" },
    {NITF_BCS_N, 3, "R Scaling Factor � Fractional Part" , "R_FRACTION" },
    {NITF_END, 0, NULL, NULL}
};

static nitf_TREDescription descrip_00091[] = {
    {NITF_BCS_N, 3, "Band Identifier", "BAND_NUMBER" },
    {NITF_BCS_N, 1, "Mapping Method to Apply", "MAP_SELECT" },
    {NITF_BCS_N, 1, "I/O TABLE USED", "TABLE_ID" },
    {NITF_BCS_N, 2, "Scale Factor 1", "S1" },
    {NITF_BCS_N, 2, "Scale Factor 2", "S2" },
    {NITF_BCS_N, 1, "Number of Segments", "NO_OF_SEGMENTS" },
    {NITF_BCS_N, 4, "Segment Boundary 1", "XOB_1" },
    {NITF_BCS_N, 4, "Segment Boundary 2", "XOB_2" },
    {NITF_LOOP, 0, NULL, "NO_OF_SEGMENTS"},
    {NITF_BINARY, 4, "B0 Coefficient", "OUT_B0" },
    {NITF_BINARY, 4, "B1 Coefficient", "OUT_B1" },
    {NITF_BINARY, 4, "B2 Coefficient", "OUT_B2" },
    {NITF_BINARY, 4, "B3 Coefficient", "OUT_B3" },
    {NITF_BINARY, 4, "B4 Coefficient", "OUT_B4" },
    {NITF_BINARY, 4, "B5 Coefficient", "OUT_B5" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

/* Define the available descriptions and the default one */
static nitf_TREDescriptionInfo descriptions[] = {
    { "IOMAPA_6", descrip_00006, 6 },
    { "IOMAPA_8202", descrip_08202, 8202 },
    { "IOMAPA_16", descrip_00016, 16 },
    { "IOMAPA_91", descrip_00091, 91 },
    { NULL, NULL, NITF_TRE_DESC_NO_LENGTH }
};
static nitf_TREDescriptionSet descriptionSet = { 0, descriptions };

NITF_DECLARE_PLUGIN(IOMAPA)

NITF_CXX_ENDGUARD
