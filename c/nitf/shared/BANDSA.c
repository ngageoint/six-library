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
    {NITF_BCS_A, 7, "ROW_SPACING", "ROW_SPACING" },
    {NITF_BCS_A, 1, "ROW_SPACING_UNITS", "ROW_SPACING_UNITS" },
    {NITF_BCS_A, 7, "COL_SPACING", "COL_SPACING" },
    {NITF_BCS_A, 1, "COL_SPACING_UNITS", "COL_SPACING_UNITS" },
    {NITF_BCS_A, 6, "FOCAL_LENGTH", "FOCAL_LENGTH" },
    {NITF_BCS_N, 4, "BANDCOUNT", "BANDCOUNT" },
    {NITF_LOOP, 0, NULL, "BANDCOUNT"},
    {NITF_BCS_A, 5, "BANDPEAK", "BANDPEAK" },
    {NITF_BCS_A, 5, "BANDLBOUND", "BANDLBOUND" },
    {NITF_BCS_A, 5, "BANDUBOUND", "BANDUBOUND" },
    {NITF_BCS_A, 5, "BANDWIDTH", "BANDWIDTH" },
    {NITF_BCS_A, 6, "BANDCALDRK", "BANDCALDRK" },
    {NITF_BCS_A, 5, "BANDCALINC", "BANDCALINC" },
    {NITF_BCS_A, 5, "BANDRESP", "BANDRESP" },
    {NITF_BCS_A, 5, "BANDASD", "BANDASD" },
    {NITF_BCS_A, 5, "BANDGSD", "BANDGSD" },
    {NITF_ENDLOOP, 0, NULL, NULL },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(BANDSA, description)

NITF_CXX_ENDGUARD
