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
    {NITF_BCS_N, 3, "Cloud Cover", "CLOUDCVR" },
    {NITF_BCS_A, 1, "Standard Radiometric Product", "SRP" },
    {NITF_BCS_A, 12, "Sensor Mode", "SENSMODE" },
    {NITF_BCS_A, 18, "Sensor Name", "SENSNAME" },
    {NITF_BCS_A, 255, "Source", "SOURCE" },
    {NITF_BCS_N, 2, "Compression Generation", "COMGEN" },
    {NITF_BCS_A, 1, "Subjective Quality", "SUBQUAL" },
    {NITF_BCS_A, 7, "PIA Mission Number", "PIAMSNNUM" },
    {NITF_BCS_A, 32, "Camera Specs", "CAMSPECS" },
    {NITF_BCS_A, 2, "Project ID Code", "PROJID" },
    {NITF_BCS_N, 1, "Generation", "GENERATION" },
    {NITF_BCS_A, 1, "Exploitation Support Data", "ESD" },
    {NITF_BCS_A, 2, "Other Conditions", "OTHERCOND" },
    {NITF_BCS_N, 7, "Mean GSD", "MEANGSD" },
    {NITF_BCS_A, 3, "Image Datum", "IDATUM" },
    {NITF_BCS_A, 3, "Image Ellipsoid", "IELLIP" },
    {NITF_BCS_A, 2, "Image Processing Level", "PREPROC" },
    {NITF_BCS_A, 2, "Image Projection System", "IPROJ" },
    {NITF_BCS_N, 8, "Satellite Track", "SATTRACK" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(PIAIMC, description)

NITF_CXX_ENDGUARD
