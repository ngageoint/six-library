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
    {NITF_BCS_N,   1,  "number of bands",           "NUM_BANDS" },
    {NITF_LOOP, 0, NULL, "NUM_BANDS"},
    {NITF_BCS_A,   1,  "band category",             "BAND_TYPE" },
    {NITF_BCS_A,   6,  "band wavelength center",    "BAND_ID" },
    {NITF_BCS_N,  11,  "focal length",              "FOC_LENGTH" },
    {NITF_BCS_N,   8,  "num linear arrays",         "NUM_DAP" },
    {NITF_BCS_N,   8,  "first sample number",       "NUM_FIR" },
    {NITF_BCS_N,   7,  "number detector elements",  "DELTA" },
    {NITF_BCS_N,   7,  "pp offset x",               "OPPOFF_X" },
    {NITF_BCS_N,   7,  "pp offset y",               "OPPOFF_Y" },
    {NITF_BCS_N,   7,  "pp offset z",               "OPPOFF_Z" },
    {NITF_BCS_N,  11,  "dm first pixel x",          "START_X" },
    {NITF_BCS_N,  11,  "dm first pixel y",          "START_Y" },
    {NITF_BCS_N,  11,  "dm last pixel x",           "FINISH_X" },
    {NITF_BCS_N,  11,  "dm last pixel y",           "FINISH_Y" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(CSSFAA, description)

NITF_CXX_ENDGUARD
