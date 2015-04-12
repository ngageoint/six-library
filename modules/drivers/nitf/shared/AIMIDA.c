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


static nitf_TREDescription descrip_00069[] = {
    {NITF_BCS_A, 7, "Aircraft T.O. Date", "MISSION_DATE" },
    {NITF_BCS_A, 4, "Mission ID", "MISSION_NO" },
    {NITF_BCS_A, 2, "Flight Number", "FLIGHT_NO" },
    {NITF_BCS_A, 3, "Image Operation Number", "OP_NUM" },
    {NITF_BCS_A, 2, "Start Segment ID", "START_SEGMENT" },
    {NITF_BCS_A, 2, "Reprocess Number", "REPRO_NUM" },
    {NITF_BCS_A, 3, "Replay", "REPLAY" },
    {NITF_BCS_A, 1, "Reserved 1", "RESVD001" },
    {NITF_BCS_A, 2, "Starting Tile Column", "START_COLUMN" },
    {NITF_BCS_A, 5, "Starting Tile Row", "START_ROW" },
    {NITF_BCS_A, 2, "Ending Segment", "END_SEGMENT" },
    {NITF_BCS_A, 2, "Ending Tile Column", "END_COLUMN" },
    {NITF_BCS_A, 5, "Ending Tile Row", "END_ROW" },
    {NITF_BCS_A, 2, "Country Code", "COUNTRY" },
    {NITF_BCS_A, 4, "Reserved 2", "RESVD002" },
    {NITF_BCS_A, 11, "Location lat/long", "LOCATION" },
    {NITF_BCS_A, 5, "Time of First Line", "TIME" },
    {NITF_BCS_A, 7, "Date of First Line", "CREATION_DATE" },
    {NITF_END, 0, NULL, NULL}
};

   /* from draft AIMIDA vers 0.9a */
static nitf_TREDescription descrip_00073[] = {
    {NITF_BCS_A, 8, "Aircraft T.O. Date", "MISSION_DATE" },
    {NITF_BCS_A, 4, "Mission ID", "MISSION_NO" },
    {NITF_BCS_A, 2, "Flight No.", "FLIGHT_NO" },
    {NITF_BCS_A, 3, "Image Operation No.", "OP_NUM" },
    {NITF_BCS_A, 2, "Reserved 1", "RESVD001" },
    {NITF_BCS_A, 2, "Reprocess No.", "REPRO_NUM" },
    {NITF_BCS_A, 3, "Retransmission No.", "REPLAY" },
    {NITF_BCS_A, 1, "Reserved 2", "RESVD002" },
    {NITF_BCS_A, 3, "Starting Tile Column", "START_COLUMN" },
    {NITF_BCS_A, 5, "Starting Tile Row", "START_ROW" },
    {NITF_BCS_A, 2, "Reserved 3", "RESVD003" },
    {NITF_BCS_A, 3, "Ending Tile Column", "END_COLUMN" },
    {NITF_BCS_A, 5, "Ending Tile Row", "END_ROW" },
    {NITF_BCS_A, 2, "Country Code", "COUNTRY" },
    {NITF_BCS_A, 4, "Reserved 4", "RESVD004" },
    {NITF_BCS_A, 11, "Location lat/long", "LOCATION" },
    {NITF_BCS_A, 5, "Time of First Line", "TIME" },
    {NITF_BCS_A, 8, "Date of First Line", "CREATION_DATE" },
    {NITF_END, 0, NULL, NULL}
};

static nitf_TREDescription descrip_00089[] = {
    {NITF_BCS_A, 14, "Acquisition Date/Time", "MISSION_DATE_TIME" },
    {NITF_BCS_A, 4, "Mission Number", "MISSION_NO" },
    {NITF_BCS_A, 10, "Mission ID (ATO)", "MISSION_ID" },
    {NITF_BCS_A, 2, "Flight No.", "FLIGHT_NO" },
    {NITF_BCS_N, 3, "Image Operation No.", "OP_NUM" },
    {NITF_BCS_A, 2, "Current Segment ID", "CURRENT_SEGMENT" },
    {NITF_BCS_N, 2, "Reprocess No.", "REPRO_NUM" },
    {NITF_BCS_A, 3, "Retransmission No.", "REPLAY" },
    {NITF_BCS_A, 1, "reserved 1", "RESVD001" },
    {NITF_BCS_N, 3, "Starting Tile Column", "START_COLUMN" },
    {NITF_BCS_N, 5, "Starting Tile Row", "START_ROW" },
    {NITF_BCS_A, 2, "Ending Segment", "END_SEGMENT" },
    {NITF_BCS_N, 3, "Ending Tile Column", "END_COLUMN" },
    {NITF_BCS_N, 5, "Ending Tile Row", "END_ROW" },
    {NITF_BCS_A, 2, "Country Code", "COUNTRY" },
    {NITF_BCS_A, 4, "reserved 2", "RESVD002" },
    {NITF_BCS_A, 11, "Location lat/long", "LOCATION" },
    {NITF_BCS_A, 13, "reserved 3", "RESVD003" },
    {NITF_END, 0, NULL, NULL}
};


/* Define the available descriptions and the default one */
static nitf_TREDescriptionInfo descriptions[] = {
    { "AIMIDA_69", descrip_00069, 69 },
    { "AIMIDA_73", descrip_00073, 73 },
    { "AIMIDA_89", descrip_00089, 89 },
    { NULL, NULL, NITF_TRE_DESC_NO_LENGTH }
};
static nitf_TREDescriptionSet descriptionSet = { 0, descriptions };

NITF_DECLARE_PLUGIN(AIMIDA)

NITF_CXX_ENDGUARD
