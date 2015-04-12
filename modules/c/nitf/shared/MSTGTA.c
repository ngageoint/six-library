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
    {NITF_BCS_A, 5, "TGT NUM", "TGTNUM" },
    {NITF_BCS_A, 12, "TGT ID", "TGTID" },
    {NITF_BCS_A, 15, "TGT BE", "TGTBE" },
    {NITF_BCS_A, 3, "TGT PRI", "TGTPRI" },
    {NITF_BCS_A, 12, "TGT REQ", "TGTREQ" },
    {NITF_BCS_A, 12, "TGT LTIOV", "TGTLTIOV" },
    {NITF_BCS_A, 1, "TGT TYPE", "TGTTYPE" },
    {NITF_BCS_A, 1, "TGT COLL", "TGTCOLL" },
    {NITF_BCS_A, 5, "TGT CAT", "TGTCAT" },
    {NITF_BCS_A, 7, "TGT UTC", "TGTUTC" },
    {NITF_BCS_A, 6, "TGT EVEL", "TGTEVEL" },
    {NITF_BCS_A, 1, "TGT EVEL UNIT", "TGTEVELUNIT" },
    {NITF_BCS_A, 21, "TGT LOC", "TGTLOC" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(MSTGTA, description)

NITF_CXX_ENDGUARD
