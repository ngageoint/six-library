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

#if _MSC_VER
#pragma warning(disable: 4820) // '...' : '...' bytes padding added after data member '...'
#pragma warning(disable: 4668) // '...' is not defined as a preprocessor macro, replacing with '...' for '...'
#pragma warning(disable: 5039) // '...': pointer or reference to potentially throwing function passed to '...' function under -EHc. Undefined behavior may occur if this function throws an exception.
#endif

#include <import/nitf.h>

NITF_CXX_GUARD

// MIL-PRF-89034, Table 38 (page 80)
static nitf_TREDescription description[] = {
    {NITF_BCS_A, 4, "Segment Model ID", "SISEGID" },
    {NITF_BCS_N, 8, "Segment Absolute CE 90%", "SNACE" },
    {NITF_BCS_N, 8, "Segment Absolute LE 90%", "SNALE" },
    {NITF_BCS_N, 8, "Segment Mensuration 1 pixel CE 90%", "SNMCE" },
    {NITF_BCS_N, 8, "Segment Mensuration 1 pixel LE 90%", "SNMLE" },
    {NITF_BCS_A, 1, "Exceeds Absolute CE Requirement", "EXACERF" },
    {NITF_BCS_A, 1, "Exceeds Absolute LE Requirement", "EXALERF" },
    {NITF_BCS_N, 3, "Number of remaining segments", "NBRSEG" },
    {NITF_LOOP, 0, NULL, "NBRSEG"},
    {NITF_BCS_A, 4, "Stereo image segment ID", "RASEGID" },
    {NITF_BCS_N, 8, "Segment Relative CEP 50% (0-1 nm)", "SNRCE0-1" },
    {NITF_BCS_N, 8, "Segment Relative LEP 50% (0-1 nm)", "SNRLE0-1" },
    {NITF_BCS_N, 8, "Segment Relative CEP 50% (1-5 nm)", "SNRCE1-5" },
    {NITF_BCS_N, 8, "Segment Relative LEP 50% (1-5 nm)", "SNRLE1-5" },
    {NITF_BCS_N, 8, "Segment Relative CEP 50% (5-15 nm)", "SNRCE5-15" },
    {NITF_BCS_N, 8, "Segment Relative LEP 50% (5-15 nm)", "SNRLE5-15" },
    {NITF_BCS_N, 8, "Segment Relative CEP 50% (15-30 nm)", "SNRCE15-30" },
    {NITF_BCS_N, 8, "Segment Relative LEP 50% (15-30 nm)", "SNRLE15-30" },
    {NITF_BCS_N, 8, "Segment Relative CEP 50% (>30 nm)", "SNRCE>30" },
    {NITF_BCS_N, 8, "Segment Relative LEP 50% (>30 nm)", "SNRLE>30" },
    {NITF_BCS_A, 1, "Exceeds Relative CEP Requirement (0-1 nm)", "EXRCE0-1" },
    {NITF_BCS_A, 1, "Exceeds Relative LEP Requirement (0-1 nm)", "EXRLE0-1" },
    {NITF_BCS_A, 1, "Exceeds Relative CEP Requirement (1-5 nm)", "EXRCE1-5" },
    {NITF_BCS_A, 1, "Exceeds Relative LEP Requirement (1-5 nm)", "EXRLE1-5" },
    {NITF_BCS_A, 1, "Exceeds Relative CEP Requirement (5-15 nm)", "EXRCE5-15" },
    {NITF_BCS_A, 1, "Exceeds Relative LEP Requirement (5-15 nm)", "EXRLE5-15" },
    {NITF_BCS_A, 1, "Exceeds Relative CEP Requirement (15-30 nm)", "EXRCE15-30" },
    {NITF_BCS_A, 1, "Exceeds Relative LEP Requirement (15-30 nm)", "EXRLE15-30" },
    {NITF_BCS_A, 1, "Exceeds Relative CEP Requirement (>30 nm)", "EXRCE>30" },
    {NITF_BCS_A, 1, "Exceeds Relative LEP Requirement (>30 nm)", "EXRLE>30" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(PTPRAA, description)

NITF_CXX_ENDGUARD
