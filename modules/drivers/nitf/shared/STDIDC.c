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
    {NITF_BCS_A, 14, "ACQ DATE", "ACQUISITION_DATE" },
    {NITF_BCS_A, 14, "MISSION", "MISSION" },
    {NITF_BCS_A, 2, "PASS", "PASS" },
    {NITF_BCS_A, 3, "OP NUM", "OP_NUM" },
    {NITF_BCS_A, 2, "START SEGMENT", "START_SEGMENT" },
    {NITF_BCS_A, 2, "REPRO NUM", "REPRO_NUM" },
    {NITF_BCS_A, 3, "REPLAY REGEN", "REPLAY_REGEN" },
    {NITF_BCS_A, 1, "BLANK FILL", "BLANK_FILL" },
    {NITF_BCS_A, 3, "START COLUMN", "START_COLUMN" },
    {NITF_BCS_A, 5, "START ROW", "START_ROW" },
    {NITF_BCS_A, 2, "END SEGMENT", "END_SEGMENT" },
    {NITF_BCS_A, 3, "END COLUMN", "END_COLUMN" },
    {NITF_BCS_A, 5, "END ROW", "END_ROW" },
    {NITF_BCS_A, 2, "COUNTRY", "COUNTRY" },
    {NITF_BCS_A, 4, "W A C", "WAC" },
    {NITF_BCS_A, 11, "LOCATION", "LOCATION" },
    {NITF_BCS_A, 5, "Reserved 1", "RESERV01" },
    {NITF_BCS_A, 8, "Reserved 2", "RESERV02" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(STDIDC, description)

NITF_CXX_ENDGUARD
