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
    {NITF_BCS_A, 25, "Shapefile Use", "SHAPE_USE" },
    {NITF_BCS_A, 10, "Type of shapes", "SHAPE_CLASS" },
    {NITF_IF, 0, "eq CLOUD_SHAPES             ", "SHAPE_USE"},
    {NITF_BCS_A, 18, "Source sensors(s) for determining cloud cover", "CC_SOURCE" },
    {NITF_ENDIF, 0, NULL, NULL },
    {NITF_BCS_A, 3, "Name of first file in shapefile", "SHAPE1_NAME" },
    {NITF_BCS_N, 6, "Start location in bytes of the file as offset in the DES USER data", "SHAPE1_START" },
    {NITF_BCS_A, 3, "Name of second file in shapefile", "SHAPE2_NAME" },
    {NITF_BCS_N, 6, "Start location in bytes of the file as offset in the DES USER data", "SHAPE2_START" },
    {NITF_BCS_A, 3, "Name of third file in shapefile", "SHAPE3_NAME" },
    {NITF_BCS_N, 6, "Start location in bytes of the file as offset in the DES USER data", "SHAPE3_START" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(CSSHPA, description)

NITF_CXX_ENDGUARD
