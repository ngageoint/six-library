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

// MIL-PRF-89034, Table 68 (page 131).
static nitf_TREDescription description[] = {
    {NITF_BCS_A, 22, "Longitude Translation", "LONTR" },
    {NITF_BCS_A, 22, "Latitude Translation", "LATTR" },
    {NITF_BCS_A, 22, "Elevation Translation", "ELVTR" },
    {NITF_BCS_A, 22, "Longitude Scale", "LONSC" },
    {NITF_BCS_A, 22, "Latitude Scale", "LATSC" },
    {NITF_BCS_A, 22, "Elevation Scale", "ELVSC" },
    {NITF_BCS_A, 22, "X Image Translation", "XITR" },
    {NITF_BCS_A, 22, "Y Image Translation", "YITR" },
    {NITF_BCS_A, 22, "X Image Scale", "XISC" },
    {NITF_BCS_A, 22, "Y Image Scale", "YISC" },
    {NITF_BCS_A, 22, "Display Elevation", "DELEV" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(IMASDA, description)

NITF_CXX_ENDGUARD
