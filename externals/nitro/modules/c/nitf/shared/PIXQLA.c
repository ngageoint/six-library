/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

static nitf_TREDescription PIXQLA_description[] = {
    {NITF_BCS_A, 3, "Number of Associated Image Segments", "NUMAIS", },
    {NITF_IF, 0, "ne ALL", "NUMAIS"},
    {NITF_LOOP, 0, NULL, "NUMAIS"},
    {NITF_BCS_N, 3, "Associated Image Segment Display Level", "AISDLVL" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_BCS_N, 4, "Number of Pixel Quality Conditions", "NPIXQUAL", },
    {NITF_BCS_A, 1, "Pixel Quality Bit Value", "PQ_BIT_VALUE", },
    {NITF_LOOP, 0, NULL, "NPIXQUAL"},
    {NITF_BCS_A, 40, "Pixel Quality Condition", "PQ_CONDITION" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN_SIMPLE(PIXQLA)

NITF_CXX_ENDGUARD
