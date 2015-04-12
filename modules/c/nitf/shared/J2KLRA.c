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
    {NITF_BCS_N,  1,  "original compressed data",     "ORIG" },
    {NITF_BCS_N,  2,  "number wavelet levels",        "NLEVELS_O" },
    {NITF_BCS_N,  5,  "number bands",                 "NBANDS_O" },
    {NITF_BCS_N,  3,  "number layers",                "NLAYERS_O" },
    {NITF_LOOP, 0, 0, "NLAYERS_O"},
    {NITF_BCS_N,  3,  "layer ID number",              "LAYER_ID" },
    {NITF_BCS_A,  9,  "bitrate",                      "BITRATE" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    /* These fields only appear if it's a parsed stream
     * (i.e. ORIG is 1, 3, or 9) */
    {NITF_IF, 0, "== 1", "ORIG"},
    {NITF_BCS_N,  2,  "number wavelet levels",        "NLEVELS_I" },
    {NITF_BCS_N,  5,  "number bands",                 "NBANDS_I" },
    {NITF_BCS_N,  3,  "number layers",                "NLAYERS_I" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "== 3", "ORIG"},
    {NITF_BCS_N,  2,  "number wavelet levels",        "NLEVELS_I" },
    {NITF_BCS_N,  5,  "number bands",                 "NBANDS_I" },
    {NITF_BCS_N,  3,  "number layers",                "NLAYERS_I" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "== 9", "ORIG"},
    {NITF_BCS_N,  2,  "number wavelet levels",        "NLEVELS_I" },
    {NITF_BCS_N,  5,  "number bands",                 "NBANDS_I" },
    {NITF_BCS_N,  3,  "number layers",                "NLAYERS_I" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(J2KLRA, description)

NITF_CXX_ENDGUARD
