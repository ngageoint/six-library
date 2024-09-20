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

// MIL-PRF-89034, Table 7 (page 46).
static nitf_TREDescription MSDIRA_description[] = {
    {NITF_BCS_A, 9, "Product ID", "PRODID" },
    {NITF_BCS_N, 2, "Product Volume", "PRODVOL" },
    {NITF_BCS_N, 2, "Product Total Volume", "PRODTOTVOLS" },
    {NITF_BCS_N, 3, "Number of Image Files", "NUMIMFILE" },
    {NITF_LOOP, 0, NULL, "NUMIMFILE"},
    {NITF_BCS_A, 6, "Segment Image ID", "SEGIMGID" },
    {NITF_BCS_N, 3, "Longitude Continuous Coordinate System Designator", "LONCNTCRD" },
    {NITF_BCS_A, 84, "Display Image Geographic Location", "DIMGEOGLO" },
    {NITF_BCS_N, 2, "Number of Vertices for Segment Image", "NBRVERT" },
    {NITF_LOOP, 0, NULL, "NBRVERT"},
    {NITF_BCS_A, 21, "Segment Image Geographic Location", "SIMGEOGLO" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_N, 2, "Volume Number", "VOLNBR" },
    {NITF_BCS_N, 3, "File Location", "FLOC" },
    {NITF_BCS_N, 10, "File Length", "FLEN" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN_SIMPLE(MSDIRA)

NITF_CXX_ENDGUARD
