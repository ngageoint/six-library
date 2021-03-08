/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
    {NITF_BCS_A, 36, "Assigned UUID for DES", "ID" },

    {NITF_BCS_A, 3, "Number of Associated Image Segments", "NUMAIS" },
    {NITF_IF, 0, "neq ALL", "NUMAIS" },
    {NITF_LOOP, 0, NULL, "NUMAIS"},
    {NITF_BCS_N, 3, "Associated Image Segment Display Level", "AISDLVL" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_ENDIF, 0, NULL, NULL},

    {NITF_BCS_N, 3, "Number of Associated Elements", "NUM_ASSOC_ELEM" },
    {NITF_LOOP, 0, NULL, "NUM_ASSOC_ELEM"},
    {NITF_BCS_N, 36, "Associated Element UUID", "ASSOC_ELEM_ID" },
    {NITF_ENDLOOP, 0, NULL, NULL},

    {NITF_BCS_N, 4, "Reserved Subheader Length", "RESERVEDSUBH_LEN" },
    {NITF_IF, 0, "> 0", "RESERVEDSUBH_LEN"},
    {NITF_BINARY, 0, "Reserved field - should not be present in current TRE version", "RESERVEDSUBH"},
    {NITF_ENDIF, 0, NULL, NULL},

    {NITF_BCS_N, 1, "Attitude Data Quality Flag", "QUAL_FLAG_ATT" },
    {NITF_BCS_N, 1, "Interpolation Type", "INTERP_TYPE_ATT" },
    {NITF_IF, 0, "eq 2", "INTERP_TYPE_ATT"},
    {NITF_BCS_N, 1, "Order of Lagrange Interpolation Polynomials", "INTERP_ORDER_ATT" },
    {NITF_ENDIF, 0, NULL, NULL},

    {NITF_BCS_N, 1, "Attitude Type", "ATT_TYPE" },
    {NITF_BCS_N, 1, "Coordinate Reference Frame Flag", "ECI_ECF_ATT" },
    {NITF_BCS_N, 13, "Time Interval Between Attitude Reference Points", "DT_ATT" },
    {NITF_BCS_N, 8, "Date of First Attitude Reference Point", "DATE_ATT" },
    {NITF_BCS_N, 16, "UTC Timestamp of First Attitude Reference Point", "TO_ATT" },

    {NITF_BCS_N, 5, "Number of Attitude Reference Points", "NUM_ATT" },
    {NITF_LOOP, 0, NULL, "NUM_ATT"},
    {NITF_BCS_N, 18, "Quaternion Q1", "Q1" },
    {NITF_BCS_N, 18, "Quaternion Q2", "Q2" },
    {NITF_BCS_N, 18, "Quaternion Q3", "Q3" },
    {NITF_BCS_N, 18, "Quaternion Q4", "Q4" },
    {NITF_ENDLOOP, 0, NULL, NULL},

    {NITF_BCS_N, 4, "Reserved Length", "RESERVED_LEN" },
    {NITF_IF, 0, "> 0", "RESERVED_LEN"},
    {NITF_BINARY, 0, "Reserved field - should not be present in current TRE version", "RESERVED"},
    {NITF_ENDIF, 0, NULL, NULL},

    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(CSATTB, description)

NITF_CXX_ENDGUARD
