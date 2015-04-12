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
    {NITF_BCS_A,  12,  "ephemeris flag",            "EPHEM_FLAG" },
    {NITF_BCS_N,   5,  "time b/w eph vectors",      "DT_EPHEM" },
    {NITF_BCS_N,   8,  "day of first eph vector",   "DATE_EPHEM" },
    {NITF_BCS_N,  13,  "UTC of first eph vector",   "T0_EPHEM" },
    {NITF_BCS_N,   3,  "number of eph vectors",     "NUM_EPHEM" },
    {NITF_LOOP, 0, NULL, "NUM_EPHEM"},
    {NITF_BCS_N,  12,  "x-coor of eph vector",      "EPHEM_X" },
    {NITF_BCS_N,  12,  "y-coor of eph vector",      "EPHEM_Y" },
    {NITF_BCS_N,  12,  "z-coor of eph vector",      "EPHEM_Z" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(CSEPHA, description)

NITF_CXX_ENDGUARD
