/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2017, Ball Aerospace
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
        {NITF_BCS_A, 3, "Number of Associated Image Segments", "NUMAIS"},
        {NITF_IF, 0, "ne ALL", "NUMAIS"},
        {NITF_LOOP, 0, NULL, "NUMAIS"},
        {NITF_BCS_N, 3, "Associated Image Segment Display Level", "AISDLVL"},
        {NITF_ENDLOOP, 0, NULL, NULL},  // NUMAIS
        {NITF_ENDIF, 0, NULL, NULL},  // NUMAOI ne ALL
        {NITF_BCS_A, 14, "Column Position of the Upper Left Pixel Metric Value", "ORIGIN_X"},
        {NITF_BCS_A, 14, "Row Position of the Upper Left Pixel Metric Value", "ORIGIN_Y"},
        {NITF_BCS_A, 14, "Column-based Scale Factor", "SCALE_X"},
        {NITF_BCS_A, 14, "Row-based Scale Factor", "SCALE_Y"},
        {NITF_BCS_A, 1, "Pixel Metric Sampling Mode", "SAMPLE_MODE"},
        {NITF_BCS_N, 5, "Number of Metrics Specified in the Pixel Metric Image Segment", "NUMMETRICS"},
        {NITF_BCS_A, 1, "Per Band Metric Flag", "PERBAND"},
        {NITF_LOOP, 0, NULL, "NUMMETRICS"},
        {NITF_BCS_A, 40, "Description of the mth Pixel Metric", "DESCRIPTION"},
        {NITF_BCS_A, 40, "Units of Measure", "UNIT"},
        {NITF_BCS_A, 1, "Mathematical Form of the Data Transformation", "FITTYPE"},
        {NITF_IF, 0, "eq P", "FITTYPE"},
        {NITF_BCS_N, 1, "Numer of Coefficients", "NUMCOEF"},
        {NITF_LOOP, 0, NULL, "NUMCOEF"},
        {NITF_BCS_A, 15, "Data Transformation Coefficient", "COEF"},
        {NITF_ENDLOOP, 0, NULL, NULL},  // NUMCOEF
        {NITF_ENDIF, 0, NULL, NULL},  // FITTYPE eq P
        {NITF_ENDLOOP, 0, NULL, NULL},  // NUMMETRICS
        {NITF_BCS_N, 5, "Size of Reserved Field", "RESERVED_LEN"},
        {NITF_IF, 0, "> 0", "RESERVED_LEN"},
        {NITF_BINARY, 0, "Reserved field - should not be present in current TRE version", "RESERVED"},
        {NITF_ENDIF, 0, NULL, NULL},  // RESERVED_LEN gt 0
        {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(PIXMTA, description)

NITF_CXX_ENDGUARD
