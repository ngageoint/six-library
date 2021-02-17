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
    {NITF_BCS_A, 14, "NITF Date Time", "FDATTIM"},
    {NITF_BCS_A, 9, "NITF Version Flag", "NITFVER"},
    {NITF_BCS_A, 207, "NITF Security Fields", "NFSECFLDS"},
    {NITF_BCS_A, 8, "Security Standard", "SECSTD"},
    {NITF_BCS_A, 8, "Security Field Compression", "SECCOMP"},
    {NITF_BCS_N, 5, "Security Length", "SECLEN"},
    {NITF_BINARY, NITF_TRE_CONDITIONAL_LENGTH, "Security Data", "SECURITY", "SECLEN"},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(SECURA, description)

NITF_CXX_ENDGUARD
