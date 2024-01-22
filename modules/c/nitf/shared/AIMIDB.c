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

#if _MSC_VER
#pragma warning(disable: 4820) // '...' : '...' bytes padding added after data member '...'
#pragma warning(disable: 4668) // '...' is not defined as a preprocessor macro, replacing with '...' for '...'
#pragma warning(disable: 5039) // '...': pointer or reference to potentially throwing function passed to '...' function under -EHc. Undefined behavior may occur if this function throws an exception.
#endif

#include <import/nitf.h>

NITF_CXX_GUARD

static nitf_TREDescription AIMIDB_description[] = {
    {NITF_BCS_A, 14, "Acquisition Date/Time", "ACQUISITION_DATE" },
    {NITF_BCS_A, 4, "Mission Number", "MISSION_NO" },
    {NITF_BCS_A, 10, "Mission ID (ATO)", "MISSION_IDENTIFICATION" },
    {NITF_BCS_A, 2, "Flight No.", "FLIGHT_NO" },
    {NITF_BCS_N, 3, "Image Operation No.", "OP_NUM" },
    {NITF_BCS_A, 2, "Current Segment ID", "CURRENT_SEGMENT" },
    {NITF_BCS_N, 2, "Reprocess No.", "REPRO_NUM" },
    {NITF_BCS_A, 3, "Retransmission No.", "REPLAY" },
    {NITF_BCS_A, 1, "reserved 1", "RESERVED_001" },
    {NITF_BCS_N, 3, "Start Tile Column", "START_TILE_COLUMN" },
    {NITF_BCS_N, 5, "Start Tile Row", "START_TILE_ROW" },
    {NITF_BCS_A, 2, "End Segment", "END_SEGMENT" },
    {NITF_BCS_N, 3, "End Tile Column", "END_TILE_COLUMN" },
    {NITF_BCS_N, 5, "End Tile Row", "END_TILE_ROW" },
    {NITF_BCS_A, 2, "Country Code", "COUNTRY" },
    {NITF_BCS_A, 4, "reserved 2", "RESERVED002" },
    {NITF_BCS_A, 11, "Location lat/long", "LOCATION" },
    {NITF_BCS_A, 13, "reserved 3", "RESERVED003" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN_SIMPLE(AIMIDB)

NITF_CXX_ENDGUARD
