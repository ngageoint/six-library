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
    {NITF_BCS_N, 1, "Success", "SUCCESS" },
    {NITF_BCS_A, 7, "Error - Bias", "ERR_BIAS" },
    {NITF_BCS_A, 7, "Error - Random", "ERR_RAND" },
    {NITF_BCS_N, 6, "Line Offset", "LINE_OFF" },
    {NITF_BCS_N, 5, "Sample Offset", "SAMP_OFF" },
    {NITF_BCS_A, 8, "Geodetic Latitude Offset", "LAT_OFF" },
    {NITF_BCS_A, 9, "Geodetic Longitude Offset", "LONG_OFF" },
    {NITF_BCS_N, 5, "Geodetic Height Offset", "HEIGHT_OFF" },
    {NITF_BCS_N, 6, "Line Scale", "LINE_SCALE" },
    {NITF_BCS_N, 5, "Sample Scale", "SAMP_SCALE" },
    {NITF_BCS_A, 8, "Geodetic Latitude Scale", "LAT_SCALE" },
    {NITF_BCS_A, 9, "Geodetic Longitude Scale", "LONG_SCALE" },
    {NITF_BCS_N, 5, "Geodetic Height Scale", "HEIGHT_SCALE" },
    {NITF_LOOP, 0, NITF_CONST_N, "20"},
    {NITF_BCS_A, 12, "Line Numerator Coefficient",
     "LINE_NUM_COEFF" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_LOOP, 0, NITF_CONST_N, "20"},
    {NITF_BCS_A, 12, "Line Denominator Coefficient", "LINE_DEN_COEFF" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_LOOP, 0, NITF_CONST_N, "20"},
    {NITF_BCS_A, 12, "Sample Numerator Coefficient", "SAMP_NUM_COEFF" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_LOOP, 0, NITF_CONST_N, "20"},
    {NITF_BCS_A, 12, "Sample Denominator Coefficient", "SAMP_DEN_COEFF" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(RPC00B, description)

NITF_CXX_ENDGUARD
