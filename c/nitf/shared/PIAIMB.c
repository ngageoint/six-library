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
    {NITF_BCS_A, 3, "Cloud Cover", "cloud" },
    {NITF_BCS_A, 1, "Standard Radiometric?", "stdrd" },
    {NITF_BCS_A, 12, "Sensor Mode", "smode" },
    {NITF_BCS_A, 18, "Sensor Name", "sname" },
    {NITF_BCS_A, 255, "Source", "srce" },
    {NITF_BCS_A, 2, "Compression Generation", "cmgen" },
    {NITF_BCS_A, 1, "Subjective Quality", "squal" },
    {NITF_BCS_A, 7, "PIA Mission Number", "misnm" },
    {NITF_BCS_A, 32, "Camera Specs", "cspec" },
    {NITF_BCS_A, 2, "Project ID Code", "pjtid" },
    {NITF_BCS_A, 1, "Generation", "gener" },
    {NITF_BCS_A, 1, "Exploitation Support", "expls" },
    {NITF_BCS_A, 2, "Other Conditions", "othrc" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(PIAIMB, description)

NITF_CXX_ENDGUARD
