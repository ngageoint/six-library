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
    {NITF_BCS_N, 4, "VERNUM", "VERNUM" },
    {NITF_BCS_A, 25, "CLCTN_NAME", "CLCTN_NAME" },
    {NITF_BCS_A, 255, "CLCTN_DESCRIPT", "CLCTN_DESCRIPT" },
    {NITF_BCS_A, 8, "CLCTN_STDATE", "CLCTN_STDATE" },
    {NITF_BCS_A, 8, "CLCTN_SPDATE", "CLCTN_SPDATE" },
    {NITF_BCS_A, 11, "CLCTN_LOC", "CLCTN_LOC" },
    {NITF_BCS_A, 15, "SITE", "SITE" },
    {NITF_BCS_A, 2, "COUNTRY", "COUNTRY" },
    {NITF_BCS_A, 20, "SPONSOR", "SPONSOR" },
    {NITF_BCS_A, 100, "PERSONNEL", "PERSONNEL" },
    {NITF_BCS_N, 1, "NUM_SITES", "NUM_SITES" },
    {NITF_LOOP, 0, NULL, "NUM_SITES"},
    {NITF_BCS_A, 20, "SCLCTN_NAME", "SCLCTN_NAME" },
    {NITF_BCS_A, 255, "SDESCRIPTION", "SDESCRIPTION" },
    {NITF_BCS_A, 3, "SITE_NUM", "SITE_NUM" },
    {NITF_BCS_A, 3, "SCN_NUM", "SCN_NUM" },
    {NITF_BCS_A, 8, "SCLCTN_STDATE", "SCLCTN_STDATE" },
    {NITF_BCS_A, 8, "SCLCTN_SPDATE", "SCLCTN_SPDATE" },
    {NITF_BCS_A, 11, "SCN_CNTR", "SCN_CNTR" },
    {NITF_BCS_A, 5, "ALTITUDE", "ALTITUDE" },
    {NITF_BCS_A, 50, "SCN_CONTENT", "SCN_CONTENT" },
    {NITF_BCS_A, 50, "BGRND_TYPE", "BGRND_TYPE" },
    {NITF_BCS_A, 1, "SITE_COV", "SITE_COV" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_A, 3, "SCLCTN_Z_OFF", "SCLCTN_Z_OFF" },
    {NITF_BCS_A, 7, "SECURITY", "SECURITY" },
    {NITF_BCS_A, 15, "SCG", "SCG" },
    {NITF_BCS_A, 2, "FLIGHT_NUM", "FLIGHT_NUM" },
    {NITF_BCS_A, 2, "PASS_NUM", "PASS_NUM" },
    {NITF_BCS_A, 20, "WX_STATION", "WX_STATION" },
    {NITF_BCS_A, 15, "WX_OVERVIEW", "WX_OVERVIEW" },
    {NITF_BCS_A, 30, "WX_FILE", "WX_FILE" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(CLCTNB, description)

NITF_CXX_ENDGUARD
