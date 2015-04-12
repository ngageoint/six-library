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
    {NITF_BCS_A, 20, "System Type", "SYSTYPE" },
    {NITF_BCS_A, 12, "Prior Compression", "PC" },
    {NITF_BCS_A, 4, "Prior Enhancements", "PE" },
    {NITF_BCS_A, 1, "System Specific Remap", "REMAP_FLAG" },
    {NITF_BCS_N, 2, "Data Mapping ID from the ESD", "LUTID" },
    {NITF_BCS_N, 2, "Number of Processing Events", "NEVENTS" },
    {NITF_LOOP, 0, NULL, "NEVENTS" },

    {NITF_BCS_A, 14, "Processing Date And Time", "PDATE" },
    {NITF_BCS_A, 10, "Processing Site", "PSITE" },
    {NITF_BCS_A, 10, "Softcopy Processing Application", "PAS" },
    {NITF_BCS_N, 1, "Number of Image Processing Comments", "NIPCOM" }, 

    {NITF_LOOP, 0, NULL, "NIPCOM"},

    {NITF_BCS_A, 80, "Image Processing Comment", "IPCOM" },

    {NITF_ENDLOOP, 0, NULL, NULL},

    {NITF_BCS_N, 2, "Input Bit Depth (actual)", "IBPP" },
    {NITF_BCS_A, 3, "Input Pixel Value Type", "IPVTYPE" },
    {NITF_BCS_A, 10, "Input Bandwidth Compression", "INBWC" },
    {NITF_BCS_A, 1, "Display-Ready Flag", "DISP_FLAG" },
    {NITF_BCS_N, 1, "Image Rotation", "ROT_FLAG" },

    {NITF_IF, 0, "== 1", "ROT_FLAG" },

    {NITF_BCS_A, 8, "Angle Rotation", "ROT_ANGLE" },

    {NITF_ENDIF, 0, NULL, NULL},

    {NITF_BCS_A, 1, "Image Projection", "ASYM_FLAG" },

    {NITF_IF, 0, "eq 1", "ASYM_FLAG" },

    {NITF_BCS_A, 7, "Mag in Line (row) Direction", "ZOOMROW" },
    {NITF_BCS_A, 7, "Mag in Element (column) Direction", "ZOOMCOL" }, 

    {NITF_ENDIF, 0, NULL, NULL},

    {NITF_BCS_A, 1, "Asymmetric Correction", "PROJ_FLAG" },

    {NITF_BCS_N, 1, "Sharpening", "SHARP_FLAG" },

    {NITF_IF, 0, "== 1", "SHARP_FLAG" },

    {NITF_BCS_N, 2, "Sharpening Family Number", "SHARPFAM" },
    {NITF_BCS_N, 2, "Sharpening Member Number", "SHARPMEM" },

    {NITF_ENDIF, 0, NULL, NULL},

    {NITF_BCS_N, 1, "Symmetrical Magnification", "MAG_FLAG" },

    {NITF_IF, 0, "== 1", "MAG_FLAG" },

    {NITF_BCS_A, 7, "Level of Relative Magnification", "MAG_LEVEL" }, 

    {NITF_ENDIF, 0, NULL, NULL},


    {NITF_BCS_N, 1, "Dynamic Range Adjustment (DRA)", "DRA_FLAG" },

    {NITF_IF, 0, "== 1", "DRA_FLAG" },

    {NITF_BCS_A, 7, "DRA Multiplier", "DRA_MULT" },

    {NITF_BCS_N, 5, "DRA Subtractor", "DRA_SUB" },

    {NITF_ENDIF, 0, NULL, NULL},

    {NITF_BCS_N, 1, "Tonal Transfer Curve (TTC)", "TTC_FLAG" },


    {NITF_IF, 0, "== 1", "TTC_FLAG" },

    {NITF_BCS_N, 2, "TTC Family Number", "TTCFAM" },
        
    {NITF_BCS_N, 2, "TTC Member Number", "TTCMEM" },

    {NITF_ENDIF, 0, NULL, NULL},


    {NITF_BCS_N, 1, "Device LUT", "DEVLUT_FLAG" },


    {NITF_BCS_N, 2, "Output Bit Depth (actual)", "OBPP" },
    {NITF_BCS_A, 3, "Output Pixel Value Type", "OPVTYPE" },
    {NITF_BCS_A, 10, "Output Bandwidth Compression", "OUTBWC" },
    
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(HISTOA, description)

NITF_CXX_ENDGUARD
