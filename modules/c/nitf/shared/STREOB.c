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
    {NITF_BCS_A, 1, "Nunber of Stereo Mates", "N_MATES" },
    {NITF_BCS_A, 1, "Mate Instance", "MATE_INSTANCE" },
    {NITF_BCS_A, 5, "Begining Convergence Angle", "B_CONV" },
    {NITF_BCS_A, 5, "Ending Convergence Angle", "E_CONV" },
    {NITF_BCS_A, 5, "Begining Asymmetric Angle", "B_ASYM" },
    {NITF_BCS_A, 5, "Ending Asymmetric Angle", "E_ASYM" },
    {NITF_BCS_A, 6, "Begining Bisector Intercept Elevation", "B_BIE" },
    {NITF_BCS_A, 6, "Ending Bisector Intercept Elevation", "E_BIE" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(STREOB, description)

NITF_CXX_ENDGUARD
