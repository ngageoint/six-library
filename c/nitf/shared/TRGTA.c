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
    {NITF_BCS_N, 4, "VERNUM", "VERNUM" },
    {NITF_BCS_N, 3, "NO_VALID_TGTS", "NO_VALID_TGTS" },
    {NITF_BCS_N, 3, "NO_SCENE_TGTS", "NO_SCENE_TGTS" },
    {NITF_LOOP, 0, NULL, "NO_VALID_TGTS"},
    {NITF_BCS_A, 25, "TGT_NAME", "TGT_NAME" },
    {NITF_BCS_A, 15, "TGT_TYPE", "TGT_TYPE" },
    {NITF_BCS_A, 6, "TGT_VER", "TGT_VER" },
    {NITF_BCS_A, 5, "TGT_CAT", "TGT_CAT" },
    {NITF_BCS_A, 17, "TGT_BE", "TGT_BE" },
    {NITF_BCS_A, 10, "TGT_SN", "TGT_SN" },
    {NITF_BCS_A, 2, "TGT_POSNUM", "TGT_POSNUM" },
    {NITF_BCS_A, 6, "TGT_ATTITUDE_PITCH", "TGT_ATTITUDE_PITCH" },
    {NITF_BCS_A, 6, "TGT_ATTITUDE_ROLL", "TGT_ATTITUDE_ROLL" },
    {NITF_BCS_A, 6, "TGT_ATTITUDE_YAW", "TGT_ATTITUDE_YAW" },
    {NITF_BCS_A, 5, "TGT_DIM_LENGTH", "TGT_DIM_LENGTH" },
    {NITF_BCS_A, 5, "TGT_DIM_WIDTH", "TGT_DIM_WIDTH" },
    {NITF_BCS_A, 5, "TGT_DIM_HEIGHT", "TGT_DIM_HEIGHT" },
    {NITF_BCS_A, 6, "TGT_AZIMUTH", "TGT_AZIMUTH" },
    {NITF_BCS_A, 8, "TGT_CLTR_RATIO", "TGT_CLTR_RATIO" },
    {NITF_BCS_A, 10, "TGT_STATE", "TGT_STATE" },
    {NITF_BCS_A, 30, "TGT_COND", "TGT_COND" },
    {NITF_BCS_A, 20, "TGT_OBSCR", "TGT_OBSCR" },
    {NITF_BCS_A, 3, "TGT_OBSCR%", "TGT_OBSCR%" },
    {NITF_BCS_A, 20, "TGT_CAMO", "TGT_CAMO" },
    {NITF_BCS_A, 3, "TGT_CAMO%", "TGT_CAMO%" },
    {NITF_BCS_A, 12, "TGT_UNDER", "TGT_UNDER" },
    {NITF_BCS_A, 30, "TGT_OVER", "TGT_OVER" },
    {NITF_BCS_A, 45, "TGT_TTEXTURE", "TGT_TTEXTURE" },
    {NITF_BCS_A, 40, "TGT_PAINT", "TGT_PAINT" },
    {NITF_BCS_A, 3, "TGT_SPEED", "TGT_SPEED" },
    {NITF_BCS_A, 3, "TGT_HEADING", "TGT_HEADING" },
    {NITF_BCS_N, 1, "TGT_QC_NUM", "TGT_QC_NUM" },
    {NITF_LOOP, 0, NULL, "TGT_QC_NUM"},
    {NITF_BCS_A, 40, "TGT_QCOMMENT", "TGT_QCOMMENT" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_N, 1, "TGT_CC_NUM", "TGT_CC_NUM" },
    {NITF_LOOP, 0, NULL, "TGT_CC_NUM"},
    {NITF_BCS_A, 40, "TGT_CCOMMENT", "TGT_CCOMMENT" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_N, 1, "NO_REF_PT", "NO_REF_PT" },
    {NITF_LOOP, 0, NULL, "NO_REF_PT"},
    {NITF_BCS_A, 10, "TGT_REF", "TGT_REF" },
    {NITF_BCS_A, 21, "TGT_LL", "TGT_LL" },
    {NITF_BCS_A, 8, "TGT_ELEV", "TGT_ELEV" },
    {NITF_BCS_A, 3, "TGT_BAND", "TGT_BAND" },
    {NITF_BCS_N, 8, "TGT_ROW", "TGT_ROW" },
    {NITF_BCS_N, 8, "TGT_COL", "TGT_COL" },
    {NITF_BCS_N, 8, "TGT_PROW", "TGT_PROW" },
    {NITF_BCS_N, 8, "TGT_PCOL", "TGT_PCOL" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_N, 3, "NO_ATTRIBUTES", "NO_ATTRIBUTES" },
    {NITF_LOOP, 0, NULL, "NO_ATTRIBUTES"},
    {NITF_BCS_N, 3, "ATTR_TGT_NUM", "ATTR_TGT_NUM" },
    {NITF_BCS_A, 30, "ATTR_NAME", "ATTR_NAME" },
    {NITF_BCS_A, 35, "ATTR_CONDTN", "ATTR_CONDTN" },
    {NITF_BCS_A, 10, "ATTR_VALUE", "ATTR_VALUE" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(TRGTA, description)

NITF_CXX_ENDGUARD
