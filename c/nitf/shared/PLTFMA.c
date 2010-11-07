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
    {NITF_BCS_A, 12, "P_NAME", "P_NAME" },
    {NITF_BCS_A, 40, "P_DESCR", "P_DESCR" },
    {NITF_BCS_A, 8, "P_DATE", "P_DATE" },
    {NITF_BCS_A, 9, "P_TIME", "P_TIME" },
    {NITF_BCS_A, 1, "P_TYPE", "P_TYPE" },
    {NITF_IF, 0, "eq G", "P_TYPE"},
    {NITF_BCS_A, 3, "SNSR_HT", "SNSR_HT" },
    {NITF_BCS_A, 21, "SNSRLOC", "SNSRLOC" },
    {NITF_BCS_A, 3, "SNSRHDNG", "SNSRHDNG" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "eq T", "P_TYPE"},
    {NITF_BCS_A, 3, "SNSR_HT", "SNSR_HT" },
    {NITF_BCS_A, 21, "SNSRLOC", "SNSRLOC" },
    {NITF_BCS_A, 3, "SNSRHDNG", "SNSRHDNG" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "eq M", "P_TYPE"},
    {NITF_BCS_A, 3, "SNSR_HT", "SNSR_HT" },
    {NITF_BCS_A, 21, "SNSRLOC", "SNSRLOC" },
    {NITF_BCS_A, 3, "SNSRHDNG", "SNSRHDNG" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "eq A", "P_TYPE"},
    {NITF_BCS_A, 15, "AC_TYPE", "AC_TYPE" },
    {NITF_BCS_A, 12, "AC_SERIAL", "AC_SERIAL" },
    {NITF_BCS_A, 10, "AC_T_NUM", "AC_T_NUM" },
    {NITF_BCS_A, 5, "AC_PITCH", "AC_PITCH" },
    {NITF_BCS_A, 5, "AC_ROLL", "AC_ROLL" },
    {NITF_BCS_A, 3, "AC_HDNG", "AC_HDNG" },
    {NITF_BCS_A, 1, "AC_REF_PT", "AC_REF_PT" },
    {NITF_BCS_A, 9, "AC_POS_X", "AC_POS_X" },
    {NITF_BCS_A, 9, "AC_POS_Y", "AC_POS_Y" },
    {NITF_BCS_A, 9, "AC_POS_Z", "AC_POS_Z" },
    {NITF_BCS_A, 9, "AC_VEL_X", "AC_VEL_X" },
    {NITF_BCS_A, 9, "AC_VEL_Y", "AC_VEL_Y" },
    {NITF_BCS_A, 9, "AC_VEL_Z", "AC_POS_Z" },
    {NITF_BCS_A, 8, "AC_ACC_X", "AC_ACC_X" },
    {NITF_BCS_A, 8, "AC_ACC_Y", "AC_ACC_Y" },
    {NITF_BCS_A, 8, "AC_ACC_Z", "AC_POS_Z" },
    {NITF_BCS_A, 5, "AC_SPEED", "AC_SPEED" },
    {NITF_BCS_A, 21, "ENTLOC", "ENTLOC" },
    {NITF_BCS_A, 6, "ENTALT", "ENTALT" },
    {NITF_BCS_A, 21, "EXITLOC", "EXITLOC" },
    {NITF_BCS_A, 6, "EXITALT", "EXITALTH" },
    {NITF_BCS_A, 5, "INS_V_NC", "INS_V_NC" },
    {NITF_BCS_A, 5, "INS_V_EC", "INS_V_EC" },
    {NITF_BCS_A, 5, "INS_V_DC", "INS_V_DC" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(PLTFMA, description)

NITF_CXX_ENDGUARD
