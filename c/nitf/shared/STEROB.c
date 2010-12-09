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
    {NITF_BCS_A, 60, "Standard ID", "ST_ID" },
    {NITF_BCS_A, 1, "N Mates", "N_MATES" },
    {NITF_BCS_A, 1, "Mate Instance", "MATE_INSTANCE" },
    {NITF_BCS_A, 5, "B Conv", "B_CONV" },
    {NITF_BCS_A, 5, "E Conv", "E_CONV" },
    {NITF_BCS_A, 5, "B Asym", "B_ASYM" },
    {NITF_BCS_A, 5, "E Asym", "E_ASYM" },
    {NITF_BCS_A, 6, "B BIE", "B_BIE" },
    {NITF_BCS_A, 6, "E EIE", "E_EIE" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(STEROB, description)

NITF_CXX_ENDGUARD
