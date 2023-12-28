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

// MIL-PRF-89034, Table 69 (page 131).
static nitf_TREDescription IMRFCA_description[] = {
    {NITF_LOOP, 0, NITF_CONST_N, "20"},
    {NITF_BCS_A, 22, "X Image Numerator", "XINC" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_LOOP, 0, NITF_CONST_N, "20"},
    {NITF_BCS_A, 22, "X Image Denominator", "XIDC" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_LOOP, 0, NITF_CONST_N, "20"},
    {NITF_BCS_A, 22, "Y Image Numerator", "YINC" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_LOOP, 0, NITF_CONST_N, "20"},
    {NITF_BCS_A, 22, "Y Image Denominator", "YIDC" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN_SIMPLE(IMRFCA)

NITF_CXX_ENDGUARD
