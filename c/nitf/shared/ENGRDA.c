/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
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

#include <nitf/IOHandle.h>
#include <nitf/TRE.h>
#include <nitf/Record.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_A, 20, "Unique Source System Name", "RESRC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 3, "Record Entry Count", "RECNT",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_LOOP, 0, NULL, "RECNT"},

        {NITF_BCS_N, 3, "Engineering Data Label Length", "ENGLN",
         NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
        /* This specifies that the next field should have length equal to the value of ENGLN field */
        {NITF_COMP_LEN, 0, NULL, "ENGLN"},
        /* This one we don't know the length of, so we have to use the special length tag */
        {NITF_BCS_A, NITF_TRE_CONDITIONAL_LENGTH, "Engineering Data Label", "ENGLBL",
         NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
        {NITF_BCS_N, 4, "Engineering Matrix Data Column Count", "ENGMTXC",
         NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
        {NITF_BCS_N, 4, "Engineering Matrix Data Row Count", "ENGMTXR",
         NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
        {NITF_BCS_A, 1, "Value Type of Engineering Data Element", "ENGTYP",
         NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
        {NITF_BCS_N, 1, "Engineering Data Element Size", "ENGDTS",
         NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
        {NITF_BCS_A, 2, "Engineering Data Units", "ENGDATU",
         NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
        {NITF_BCS_N, 8, "Engineering Data Count", "ENGDATC",
         NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
         /* This specifies that the next field should have length equal to the value of ENGDATC field */
        {NITF_COMP_LEN, 0, NULL, "ENGDATC"},
        /* This one we don't know the length of, so we have to use the special length tag */
        {NITF_BINARY, NITF_TRE_CONDITIONAL_LENGTH, "Engineering Data", "ENGDATA",
         NITF_VAL_BCS"+", NITF_NO_RANGE, NULL, NULL},

    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(ENGRDA, description)

NITF_CXX_ENDGUARD
