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
    {NITF_BCS_A, 64, "Access ID", "accid" },
    {NITF_BCS_A, 32, "FM Control Number", "fmctl" },
    {NITF_BCS_A, 1, "Subjective Detail", "sdet" },
    {NITF_BCS_A, 2, "Product Code", "pcode" },
    {NITF_BCS_A, 6, "Producer Subelement", "psube" },
    {NITF_BCS_A, 20, "Product ID Number", "pidnm" },
    {NITF_BCS_A, 10, "Product Short Name", "pname" },
    {NITF_BCS_A, 2, "Producer Code", "maker" },
    {NITF_BCS_A, 14, "Product Create Time", "ctime" },
    {NITF_BCS_A, 40, "Map ID", "mapid" },
    {NITF_BCS_N, 2, "SECTITLE Repetitions", "strep" },

    {NITF_LOOP, 0, NULL, "strep"},

    {NITF_BCS_A, 48, "SECTITLE", "SECTITLE" },

    {NITF_ENDLOOP, 0, NULL, NULL},

    {NITF_BCS_N, 2, "REQORG Repetitions", "rorep" },
    {NITF_LOOP, 0, NULL, "rorep" },
    {NITF_BCS_A, 64, "Requesting Organization", "REQORG" },

    {NITF_ENDLOOP, 0, NULL, NULL},

    {NITF_BCS_N, 2, "KEYWORD Repetitions", "kwrep" },
    {NITF_LOOP, 0, NULL, "kwrep"},

    {NITF_BCS_A, 255, "KEYWORD", "KEYWORD" },

    {NITF_ENDLOOP, 0, NULL, NULL},

    {NITF_BCS_N, 2, "ASSRPT Repetitions", "arrep" },

    {NITF_LOOP, 0, NULL, "arrep"},

    {NITF_BCS_A, 20, "ASSRPT", "ASSRPT" },

    {NITF_ENDLOOP, 0, NULL, NULL},

    {NITF_BCS_N, 2, "ATEXT Repetitions", "atrep" },

    {NITF_LOOP, 0, NULL, "atrep" },

    {NITF_BCS_A, 255, "ATEXT", "ATEXT" },

    {NITF_ENDLOOP, 0, NULL, NULL},

    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(PIAPRC, description)

NITF_CXX_ENDGUARD
