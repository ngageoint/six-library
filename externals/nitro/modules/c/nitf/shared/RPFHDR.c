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

#if _MSC_VER
#pragma warning(disable: 4820) // '...' : '...' bytes padding added after data member '...'
#pragma warning(disable: 4668) // '...' is not defined as a preprocessor macro, replacing with '...' for '...'
#pragma warning(disable: 5039) // '...': pointer or reference to potentially throwing function passed to '...' function under -EHc. Undefined behavior may occur if this function throws an exception.
#endif

#include <import/nitf.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BINARY, 1, "endian flag", "ENDIAN" },
    {NITF_BINARY, 2, "header section length", "HDSECL" },
    {NITF_BCS_A, 12, "filename", "FILENM" },
    {NITF_BINARY, 1, "new flag", "NEWFLG" },
    {NITF_BCS_A, 15, "standard number", "STDNUM" },
    {NITF_BCS_A, 8, "standard date", "STDDAT" },
    {NITF_BCS_A, 1, "classification", "CLASS" },
    {NITF_BCS_A, 2, "country code", "COUNTR" },
    {NITF_BCS_A, 2, "release", "RELEAS" },
    {NITF_BINARY, 4, "location sect phys loc", "LOCSEC" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(RPFHDR, description)

NITF_CXX_ENDGUARD
