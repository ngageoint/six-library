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
    {NITF_BINARY, 1, "location section length", "LOCLEN" },
    {NITF_IF, 0, "> 0", "LOCLEN"},
    {NITF_BINARY, 1, "component location table offset", "CLTOFF" },
    {NITF_BINARY, 1, "# of section location records", "SECRECS" },
    {NITF_BINARY, 1, "location record length", "RECLEN" },
    {NITF_BINARY, 1, "component aggregate length", "AGGLEN" },
    {NITF_LOOP, 0, NULL, "SECRECS"},
    {NITF_BINARY, 1, "location ID code", "LOCID" },
    {NITF_BINARY, 1, "location section length", "SECLEN" },
    {NITF_BINARY, 1, "physical index", "PHYSIDX" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_BINARY, NITF_TRE_GOBBLE, "unknown", "UNKNOWN" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(RPFDES, description)

NITF_CXX_ENDGUARD
