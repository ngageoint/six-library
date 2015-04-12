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
    {NITF_BCS_A, 3, "Coordinate System Type", "TYP" },
    {NITF_BCS_A, 3, "Coordinate Units", "UNI" },
    {NITF_BCS_A, 80, "Geodetic Datum Name", "DAG" },
    {NITF_BCS_A, 4, "Geodetic Datum Code", "DCD" },
    {NITF_BCS_A, 80, "Ellipsoid Name", "ELL" },
    {NITF_BCS_A, 3, "Ellipsoid Code", "ELC" },
    {NITF_BCS_A, 80, "Vertical Datum Reference", "DVR" },
    {NITF_BCS_A, 4, "Code of Vertical Reference", "VDCDVR" },
    {NITF_BCS_A, 80, "Sounding Datum Name", "SDA" },
    {NITF_BCS_A, 4, "Code for Sounding Datum", "VDCSDA" },
    {NITF_BCS_N, 15, "Z values False Origin", "ZOR" },
    {NITF_BCS_A, 3, "Grid Code", "GRD" },
    {NITF_BCS_A, 80, "Grid Description", "GRN" },
    {NITF_BCS_N, 4, "Grid Zone number", "ZNA" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(GEOPSB, description)

NITF_CXX_ENDGUARD
