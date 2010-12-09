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
    {NITF_BCS_N, 3, "NUM_OBJ", "NUM_OBJ" },
    {NITF_BCS_A, 10, "OBJ_REF", "OBJ_REF" },
    {NITF_BCS_N, 3, "NUM_SCENE_OBJ", "NUM_SCENE_OBJ" },
    {NITF_LOOP, 0, NULL, "NUM_OBJ"},
    {NITF_BCS_A, 20, "OBJ_TY", "OBJ_TY" },
    {NITF_BCS_A, 15, "OBJ_NM", "OBJ_NM" },
    {NITF_BCS_A, 2, "OBJ_POS", "OBJ_POS" },
    {NITF_BCS_A, 10, "OBJ_SN", "OBJ_SN" },
    {NITF_BCS_A, 21, "OBJ_LL", "OBJ_LL" },
    {NITF_BCS_A, 8, "OBJ_ELEV", "OBJ_ELEV" },
    {NITF_BCS_A, 8, "OBJ_ROW", "OBJ_ROW" },
    {NITF_BCS_A, 8, "OBJ_COL", "OBJ_COL" },
    {NITF_BCS_A, 8, "OBJ_PROW", "OBJ_PROW" },
    {NITF_BCS_A, 8, "OBJ_PCOL", "OBJ_PCOL" },
    {NITF_BCS_A, 20, "OBJ_ATTR", "OBJ_ATTR" },
    {NITF_BCS_A, 2, "OBJ_SEN", "OBJ_SEN" },
    {NITF_IF, 0, "eq R ", "OBJ_SEN"},
    {NITF_BCS_A, 7, "OBJ_AZ_3DB_WIDTH", "OBJ_AZ_3DB_WIDTH" },
    {NITF_BCS_A, 7, "OBJ_RNG_3DB_WIDTH", "OBJ_RNG_3DB_WIDTH" },
    {NITF_BCS_A, 7, "OBJ_AZ_18DB_WIDTH", "OBJ_AZ_18DB_WIDTH" },
    {NITF_BCS_A, 7, "OBJ_RNG_18DB_WIDTH", "OBJ_RNG_18DB_WIDTH" },
    {NITF_BCS_A, 8, "OBJ_AZ_3_18DB_RATIO", "OBJ_AZ_3_18DB_RATIO" },
    {NITF_BCS_A, 8, "OBJ_RNG_3_18DB_RATIO", "OBJ_RNG_3_18DB_RATIO" },
    {NITF_BCS_A, 8, "OBJ_AZ_PK_SL_RATIO", "OBJ_AZ_PK_SL_RATIO" },
    {NITF_BCS_A, 8, "OBJ_RNG_PK_SL_RATIO", "OBJ_RNG_PK_SL_RATIO" },
    {NITF_BCS_A, 8, "OBJ_AZ_INT_SL_RATIO", "OBJ_AZ_INT_SL_RATIO" },
    {NITF_BCS_A, 8, "OBJ_RNGINT_SL_RATIO", "OBJ_RNGINT_SL_RATIO" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "eq EO", "OBJ_SEN"},
    {NITF_BCS_A, 6, "OBJ_CAL_TEMP", "OBJ_CAL_TEMP" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "eq IR", "OBJ_SEN"},
    {NITF_BCS_A, 6, "OBJ_CAL_TEMP", "OBJ_CAL_TEMP" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(OBJCTA, description)

NITF_CXX_ENDGUARD
