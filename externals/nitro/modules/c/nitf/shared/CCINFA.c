/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2017, Sigma Bravo Pty Ltd
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

static nitf_TREDescription CCINFA_description[] = {
    {NITF_BCS_N, 3, "Number of defined codes", "NUMCODE" },
    {NITF_LOOP, 0, NULL, "NUMCODE"},
        {NITF_BCS_N, 1, "Length of the CODE field", "CODE_LEN" },
        {NITF_BCS_A, NITF_TRE_CONDITIONAL_LENGTH, "Country Code", "CODE", "CODE_LEN" },
        {NITF_BCS_A, 1, "Type of equivalence", "EQTYPE" },
        {NITF_BCS_N, 2, "Length of ESURN field", "ESURN_LEN" },
        {NITF_BCS_A, NITF_TRE_CONDITIONAL_LENGTH, "Equivalent short URN-based individual item identifier", "ESURN", "ESURN_LEN" },
        {NITF_BCS_N, 5, "Length of the DETAIL field", "DETAIL_LEN" },
        {NITF_IF, 0, "> 0", "DETAIL_LEN"},
            {NITF_BCS_A, 1, "Code Detail Compression", "DETAIL_CMPR" },
            {NITF_BCS_A, NITF_TRE_CONDITIONAL_LENGTH, "Code Detail", "DETAIL", "DETAIL_LEN" },
        {NITF_ENDIF, 0, NULL, NULL},

    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN_SIMPLE(CCINFA)

NITF_CXX_ENDGUARD
