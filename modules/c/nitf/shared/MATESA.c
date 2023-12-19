/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2020, Brad Hards <bradh@frogmouth.net>
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

/* From STDI-0002-1 Appendix AK: Table AK.6-5: MATESA */
static nitf_TREDescription MATESA_description[] = {
    {NITF_BCS_A, 42, "Current File/Segment Source", "CUR_SOURCE" },
    {NITF_BCS_A, 16, "Current File/Segment Mate Type", "CUR_MATE_TYPE" },
    {NITF_BCS_N, 4, "Length of the CUR_FILE_ID field", "CUR_FILE_ID_LEN" },
    {NITF_BCS_A, NITF_TRE_CONDITIONAL_LENGTH, "ID of the Current File/Segment", "CUR_FILE_ID", "CUR_FILE_ID_LEN" },
    {NITF_BCS_N, 4, "Number of Mate Relationship Groups", "NUM_GROUPS" },
    {NITF_LOOP, 0, NULL, "NUM_GROUPS" },
        {NITF_BCS_A, 24, "Mate Relationship", "RELATIONSHIP" },
        {NITF_BCS_N, 4, "Number of Mates in the Group", "NUM_MATES" },
        {NITF_LOOP, 0, NULL, "NUM_MATES" },
            {NITF_BCS_A, 42, "Mate Source", "SOURCE" },
            {NITF_BCS_A, 16, "Mate Identifier Type", "MATE_TYPE" },
            {NITF_BCS_N, 4, "Length of the MATE_ID field", "MATE_ID_LEN" },
            {NITF_BCS_A, NITF_TRE_CONDITIONAL_LENGTH, "Mate File Identifier", "MATE_ID", "MATE_ID_LEN" },
        {NITF_ENDLOOP, 0, NULL, NULL },
    {NITF_ENDLOOP, 0, NULL, NULL },
    {NITF_END, 0, NULL, NULL },
};

NITF_DECLARE_SINGLE_PLUGIN_SIMPLE(MATESA)

NITF_CXX_ENDGUARD
