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

/* FIXME This plugin clobbers its own variables! 
   May need revision using 2.0 API to support
 */
#include <import/nitf.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_N, 4, "VERNUM", "VERNUM" },
    {NITF_BCS_A, 32, "FILENAME", "FILENAME" },
    {NITF_BCS_A, 32, "PARENT_FNAME", "PARENT_FNAME" },
    {NITF_BCS_A, 32, "CHECKSUM", "CHECKSUM" },
    {NITF_BCS_N, 10, "ISIZE", "ISIZE" },
    {NITF_BCS_A, 1, "STATUS", "STATUS" },
    {NITF_BCS_A, 8, "CDATE", "CDATE" },
    {NITF_BCS_A, 10, "CTIME", "CTIME" },
    {NITF_BCS_A, 8, "PDATE", "PDATE" },
    {NITF_BCS_A, 1, "SENTYPE", "SENTYPE" },
    {NITF_BCS_A, 1, "DATA_PLANE", "DATA_PLANE" },
    {NITF_BCS_A, 4, "DATA_TYPE", "DATA_TYPE" },
    {NITF_BCS_N, 6, "NUM_ROWS", "NUM_ROWS" },
    {NITF_BCS_N, 6, "NUM_COLS", "NUM_COLS" },
    {NITF_BCS_A, 1, "SEN_POS", "SEN_POS" },
    {NITF_BCS_A, 15, "SEN_CAL_FAC", "SEN_CAL_FAC" },
    {NITF_BCS_A, 50, "IMGQUAL", "IMGQUAL" },
    {NITF_BCS_N, 2, "NUM_VER", "NUM_VER" },
    {NITF_LOOP, 0, NULL, "NUM_VER"},
    {NITF_BCS_A, 15, "VER_NAME", "VER_NAME" },
    {NITF_BCS_N, 10, "VERNUM", "VERNUM" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_IF, 0, "eq R", "SENTYPE"},
    {NITF_BCS_A, 1, "SEN_LOOK", "SEN_LOOK" },
    {NITF_BCS_A, 7, "CR_RES", "CR_RES" },
    {NITF_BCS_A, 7, "RANGE_RES", "RANGE_RES" },
    {NITF_BCS_A, 7, "CR_PIXELSP", "CR_PIXELSP" },
    {NITF_BCS_A, 7, "RANGE_PIXELSP", "RANGE_PIXELSP" },
    {NITF_BCS_A, 40, "CR_WEIGHT", "CR_WEIGHT" },
    {NITF_BCS_A, 40, "RANGE_WEIGHT", "RANGE_WEIGHT" },
    {NITF_BCS_A, 6, "R_OVR_SAMP", "R_OVR_SAMP" },
    {NITF_BCS_A, 6, "CR_OVR_SAMP", "CR_OVR_SAMP" },
    {NITF_BCS_A, 6, "D_DEPRES", "D_DEPRES" },
    {NITF_BCS_A, 7, "D_GP_SQ", "D_GP_SQ" },
    {NITF_BCS_A, 7, "D_SP_SQ", "D_SP_SQ" },
    {NITF_BCS_A, 7, "D_RANGE", "D_RANGE" },
    {NITF_BCS_A, 21, "D_AP_LL", "D_AP_LL" },
    {NITF_BCS_A, 7, "D_AP_ELV", "D_AP_ELV" },
    {NITF_BCS_A, 6, "M_DEPRES", "M_DEPRES" },
    {NITF_BCS_A, 7, "M_GP_SQ", "M_GP_SQ" },
    {NITF_BCS_A, 7, "M_SP_SQ", "M_SP_SQ" },
    {NITF_BCS_A, 7, "M_RANGE", "M_RANGE" },
    {NITF_BCS_A, 21, "M_AP_LL", "M_AP_LL" },
    {NITF_BCS_A, 7, "M_AP_ELV", "M_AP_ELV" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "eq E", "SENTYPE"},
    {NITF_BCS_A, 6, "GRNDSAMPDIS", "GRNDSAMPDIS" },
    {NITF_BCS_A, 6, "SWATHSIZE", "SWATHSIZE" },
    {NITF_BCS_A, 7, "D_RANGE", "D_RANGE" },
    {NITF_BCS_A, 6, "D_AZ_LOOK", "D_AZ_LOOK" },
    {NITF_BCS_A, 5, "D_EL_LOOK", "D_EL_LOOK" },
    {NITF_BCS_A, 7, "M_RANGE", "M_RANGE" },
    {NITF_BCS_A, 6, "M_AZ_LOOK", "M_AZ_LOOK" },
    {NITF_BCS_A, 5, "M_EL_LOOK", "M_EL_LOOK" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "eq I", "SENTYPE"},
    {NITF_BCS_A, 6, "GRNDSAMPDIS", "GRNDSAMPDIS" },
    {NITF_BCS_A, 6, "SWATHSIZE", "SWATHSIZE" },
    {NITF_BCS_A, 7, "D_RANGE", "D_RANGE" },
    {NITF_BCS_A, 6, "D_AZ_LOOK", "D_AZ_LOOK" },
    {NITF_BCS_A, 5, "D_EL_LOOK", "D_EL_LOOK" },
    {NITF_BCS_A, 7, "M_RANGE", "M_RANGE" },
    {NITF_BCS_A, 6, "M_AZ_LOOK", "M_AZ_LOOK" },
    {NITF_BCS_A, 5, "M_EL_LOOK", "M_EL_LOOK" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(IMGDTA, description)

NITF_CXX_ENDGUARD
