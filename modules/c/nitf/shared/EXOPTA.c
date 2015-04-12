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
    {NITF_BCS_A, 3, "ANGLE TO NORTH", "ANGLETONORTH" },
    {NITF_BCS_A, 5, "MEAN GSD", "MEANGSD" },
    {NITF_BCS_A, 1, "Reserved 1", "RESERV01" },
    {NITF_BCS_A, 5, "DYNAMIC RANGE", "DYNAMICRANGE" },
    {NITF_BCS_A, 7, "Reserved 2", "RESERV02" },
    {NITF_BCS_A, 5, "OBL ANG", "OBLANG" },
    {NITF_BCS_A, 6, "ROLL ANG", "ROLLANG" },
    {NITF_BCS_A, 12, "PRIME ID", "PRIMEID" },
    {NITF_BCS_A, 15, "PRIME BE", "PRIMEBE" },
    {NITF_BCS_A, 5, "Reserved 3", "RESERV03" },
    {NITF_BCS_A, 3, "N SEC", "NSEC" },
    {NITF_BCS_A, 2, "Reserved 4", "RESERV04" },
    {NITF_BCS_A, 7, "Reserved 5", "RESERV05" },
    {NITF_BCS_A, 3, "N SEG", "NSEG" },
    {NITF_BCS_A, 6, "MAX LP SEG", "MAXLPSEG" },
    {NITF_BCS_A, 12, "Reserved 6", "RESERV06" },
    {NITF_BCS_A, 5, "SUN EL", "SUNEL" },
    {NITF_BCS_A, 5, "SUN AZ", "SUNAZ" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(EXOPTA, description)

NITF_CXX_ENDGUARD
