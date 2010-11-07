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
    {NITF_BCS_A, 2, "Non-linear transform flag", "XFRM_FLAG" },
    {NITF_BCS_A, 10, "Scale Factor Relative to RO", "SCALE_FACTOR" },
    {NITF_BCS_A, 2, "Anamorphic Correction Indicator", "ANAMRPH_CORR" },
    {NITF_BCS_A, 2, "Scan Block Number", "SCANBLK_NUM" },
    {NITF_BCS_A, 12,
     "Output Product row number component of grid point index(1,1)",
     "OP_ROW_11" },

    {NITF_BCS_A, 12,
     "Output product column number component of grid point index(1,1)",
     "OP_COL_11" },

    {NITF_BCS_A, 12,
     "Output product row number component of grid point index(1,2)",
     "OP_ROW_12" },

    {NITF_BCS_A, 12,
     "Output product column number component of grid point index(1,2)",
     "OP_COL_12" },

    {NITF_BCS_A, 12,
     "Output product row number component of grid point index(2,1)",
     "OP_ROW_21" },

    {NITF_BCS_A, 12,
     "Output product column number component of grid point index(2,1)",
     "OP_COL_21" },

    {NITF_BCS_A, 12,
     "Output product row number component of grid point index(2,2)",
     "OP_ROW_22" },

    {NITF_BCS_A, 12,
     "Output product column number component of grid point index(2,2)",
     "OP_COL_22" },

    {NITF_BCS_A, 12,
     "Grid point (1,1) row number in full image coodinates",
     "FI_ROW_11" },

    {NITF_BCS_A, 12,
     "Grid point (1,1) column number in full image coodinates",
     "FI_COL_11" },

    {NITF_BCS_A, 12,
     "Grid point (1,2) row number in full image coodinates",
     "FI_ROW_12" },

    {NITF_BCS_A, 12,
     "Grid point (1,2) column number in full image coodinates",
     "FI_COL_12" },

    {NITF_BCS_A, 12,
     "Grid point (2,1) row number in full image coodinates",
     "FI_ROW_21" },

    {NITF_BCS_A, 12,
     "Grid point (2,1) column number in full image coodinates",
     "FI_COL_21" },

    {NITF_BCS_A, 12,
     "Grid point (2,2) row number in full image coodinates",
     "FI_ROW_22" },

    {NITF_BCS_A, 12,
     "Grid point (2,2) column number in full image coodinates",
     "FI_COL_22" },

    {NITF_BCS_A, 8,"Full image number of rows","FI_ROW" },
    {NITF_BCS_A, 8, "Full image number of columns", "FI_COL" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(ICHIPB, description)

NITF_CXX_ENDGUARD
