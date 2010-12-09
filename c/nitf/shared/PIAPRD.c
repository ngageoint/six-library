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
    {NITF_BCS_A, 64, "Access ID", "ACCESSID" },
    {NITF_BCS_A, 32, "FM Control Number", "FMCNTROL" },
    {NITF_BCS_A, 1, "Subjective Detail", "SUBDET" },
    {NITF_BCS_A, 2, "Product Code", "PRODCODE" },
    {NITF_BCS_A, 6, "Producer Subelement", "PRODCRSE" },
    {NITF_BCS_A, 20, "Product ID Number", "PRODIDNO" },
    {NITF_BCS_A, 10, "Product Short Name", "PRODSNME" },
    {NITF_BCS_A, 2, "Producer Code", "PRODCRCD" },
    {NITF_BCS_A, 14, "Product Create Time", "PRODCRTM" },
    {NITF_BCS_A, 40, "Map ID", "MAPID" },
    {NITF_BCS_N, 2, "SECTITLE Repetitions", "SECTTREP" },
    {NITF_LOOP, 0, NULL, "SECTTREP"},
    {NITF_BCS_A, 40, "Section Title", "SECTITLE" },
    {NITF_BCS_A, 5, "Page/Part Number", "PPNUM" },
    {NITF_BCS_N, 3, "Total Pages/Parts", "TPP" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_N, 2, "REQORG Repetitions", "RQORGREP" },
    {NITF_LOOP, 0, NULL, "RQORGREP"},
    {NITF_BCS_A, 64, "Requesting Organization", "REQORG" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_N, 2, "Keyword Repetitions", "KEYWDREP" },
    {NITF_LOOP, 0, NULL, "KEYWDREP"},
    {NITF_BCS_A, 255, "Keyword", "KEYWORD" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_N, 2, "ASSRPT Repetitions", "ASRPTREP" },
    {NITF_LOOP, 0, NULL, "ASRPTREP"},
    {NITF_BCS_A, 20, "Associated Report", "ASSRPT" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_N, 2, "Assoc. Text Repetitions", "ATEXTREP" },
    {NITF_LOOP, 0, NULL, "ATEXTREP"},
    {NITF_BCS_A, 255, "Associated Text", "ATEXT" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(PIAPRD, description)

NITF_CXX_ENDGUARD
