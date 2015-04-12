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
    {NITF_BCS_A, 20, "SENNAME", "SENNAME" },
    {NITF_BCS_A, 1, "SENTYPE", "SENTYPE" },
    {NITF_BCS_A, 10, "SENMODE", "SENMODE" },
    {NITF_BCS_A, 12, "SENSCAN", "SENSCAN" },
    {NITF_BCS_A, 10, "SENSOR_ID", "SENSOR_ID" },
    {NITF_BCS_A, 3, "MPLAN", "MPLAN" },
    {NITF_BCS_A, 4, "SENSERIAL", "SENSERIAL" },
    {NITF_BCS_A, 10, "SENOPORG", "SENOPORG" },
    {NITF_BCS_A, 12, "SENMFG", "SENMFG" },
    {NITF_BCS_A, 7, "ABSWVER", "ABSWVER" },
    {NITF_BCS_A, 5, "AVG_ALT", "AVG_ALT" },
    {NITF_IF, 0, "eq R", "SENTYPE"},
    {NITF_BCS_A, 7, "FOC_X", "FOC_X" },
    {NITF_BCS_A, 7, "FOC_Y", "FOC_Y" },
    {NITF_BCS_A, 7, "FOC_Z", "FOC_Z" },
    {NITF_BCS_N, 1, "NUM_SENBAND", "NUM_SENBAND" },
    {NITF_LOOP, 0, NULL, "NUM_SENBAND"},
    {NITF_BCS_A, 10, "SENBAND", "SENBAND" },
    {NITF_BCS_A, 3, "SEN_BANDW", "SEN_BANDWL" },
    {NITF_BCS_A, 3, "SEN_CEN_F", "SEN_CEN_F" },
    {NITF_BCS_A, 2, "POLARIZATION", "POLARIZATION" },
    {NITF_BCS_A, 6, "AZ_BWIDTH", "AZ_BWIDTH" },
    {NITF_BCS_A, 6, "EL_BWIDTH", "EL_BWIDTH" },
    {NITF_BCS_A, 4, "DYN_RNGE", "DYN_RNGE" },
    {NITF_BCS_A, 15, "SENCALFAC", "SENCALFAC" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "eq I", "SENTYPE"},
    {NITF_BCS_N, 1, "NUM_SENBAND", "NUM_SENBAND" },
    {NITF_LOOP, 0, NULL, "NUM_SENBAND"},
    {NITF_BCS_A, 10, "SENBAND", "SENBAND" },
    {NITF_BCS_A, 3, "SEN_FOV_T", "SEN_FOV_T" },
    {NITF_BCS_A, 1, "SEN_FOV_T_U", "SEN_FOV_T_U" },
    {NITF_BCS_A, 3, "SEN_IFOV_T", "SEN_IFOV_T" },
    {NITF_BCS_A, 1, "SEN_IFOV_T_U", "SEN_IFOV_T_U" },
    {NITF_BCS_A, 5, "SEN_FOV_CT", "SEN_FOV_CT" },
    {NITF_BCS_A, 3, "SEN_IFOV_CT", "SEN_IFOV_CT" },
    {NITF_BCS_A, 1, "SEN_IFOV_CT_U", "SEN_IFOV_CT_U" },
    {NITF_BCS_A, 3, "SEN_FOR_T", "SEN_FOR_T" },
    {NITF_BCS_A, 3, "SEN_FOR_CT", "SEN_FOR_CT" },
    {NITF_BCS_A, 4, "SEN_L_WAVE", "SEN_L_WAVE" },
    {NITF_BCS_A, 4, "SEN_U_WAVE", "SEN_U_WAVE" },
    {NITF_BCS_A, 3, "SUBBANDS", "SUBBANDS" },
    {NITF_BCS_A, 4, "SENFLENGTH", "SENFLENGTH" },
    {NITF_BCS_A, 4, "SENFNUM", "SENFNUM" },
    {NITF_BCS_A, 4, "LINESAMPLES", "LINESAMPLES" },
    {NITF_BCS_A, 12, "DETECTTYPE", "DETECTTYPE" },
    {NITF_BCS_A, 2, "POLARIZATION", "POLARIZATION" },
    {NITF_BCS_A, 4, "DYN_RNGE", "DYN_RNGE" },
    {NITF_BCS_A, 15, "SENCALFAC", "SENCALFAC" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "eq E", "SENTYPE"},
    {NITF_BCS_N, 1, "NUM_SENBAND", "NUM_SENBAND" },
    {NITF_LOOP, 0, NULL, "NUM_SENBAND"},
    {NITF_BCS_A, 10, "SENBAND", "SENBAND" },
    {NITF_BCS_A, 3, "SEN_FOV_T", "SEN_FOV_T" },
    {NITF_BCS_A, 1, "SEN_FOV_T_U", "SEN_FOV_T_U" },
    {NITF_BCS_A, 3, "SEN_IFOV_T", "SEN_IFOV_T" },
    {NITF_BCS_A, 1, "SEN_IFOV_T_U", "SEN_IFOV_T_U" },
    {NITF_BCS_A, 5, "SEN_FOV_CT", "SEN_FOV_CT" },
    {NITF_BCS_A, 3, "SEN_IFOV_CT", "SEN_IFOV_CT" },
    {NITF_BCS_A, 1, "SEN_IFOV_CT_U", "SEN_IFOV_CT_U" },
    {NITF_BCS_A, 3, "SEN_FOR_T", "SEN_FOR_T" },
    {NITF_BCS_A, 3, "SEN_FOR_CT", "SEN_FOR_CT" },
    {NITF_BCS_A, 4, "SEN_L_WAVE", "SEN_L_WAVE" },
    {NITF_BCS_A, 4, "SEN_U_WAVE", "SEN_U_WAVE" },
    {NITF_BCS_A, 3, "SUBBANDS", "SUBBANDS" },
    {NITF_BCS_A, 4, "SENFLENGTH", "SENFLENGTH" },
    {NITF_BCS_A, 4, "SENFNUM", "SENFNUM" },
    {NITF_BCS_A, 4, "LINESAMPLES", "LINESAMPLES" },
    {NITF_BCS_A, 12, "DETECTTYPE", "DETECTTYPE" },
    {NITF_BCS_A, 2, "POLARIZATION", "POLARIZATION" },
    {NITF_BCS_A, 4, "DYN_RNGE", "DYN_RNGE" },
    {NITF_BCS_A, 15, "SENCALFAC", "SENCALFAC" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(SNSRA, description)

NITF_CXX_ENDGUARD
