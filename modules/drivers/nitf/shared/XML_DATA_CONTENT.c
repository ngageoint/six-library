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

static nitf_TREDescription descrip_0005[] = {
    {NITF_BCS_N, 5, "Cyclic Redundancy Check", "DESCRC" },
    {NITF_END, 0, NULL, NULL}
};

static nitf_TREDescription descrip_0283[] = {
    {NITF_BCS_N, 5, "Cyclic Redundancy Check", "DESCRC" },
    {NITF_BCS_A, 8, "XML File Type", "DESSHFT" },
    {NITF_BCS_A, 20, "Date and Time", "DESSHDT" },
    {NITF_BCS_A, 40, "Responsible Party  Organization Identifier", "DESSHRP" },
    {NITF_BCS_A, 60, "Specification Identifier.", "DESSHSI" },
    {NITF_BCS_A, 10, "Specification Version", "DESSHSV" },
    {NITF_BCS_A, 20, "Specification Date", "DESSHSD" },
    {NITF_BCS_A, 120, "Target Namespace", "DESSHTN" },
    {NITF_END, 0, NULL, NULL}
};

static nitf_TREDescription descrip_0773[] = {
    {NITF_BCS_N, 5, "Cyclic Redundancy Check", "DESCRC" },
    {NITF_BCS_A, 8, "XML File Type", "DESSHFT" },
    {NITF_BCS_A, 20, "Date and Time", "DESSHDT" },
    {NITF_BCS_A, 40, "Responsible Party  Organization Identifier", "DESSHRP" },
    {NITF_BCS_A, 60, "Specification Identifier.", "DESSHSI" },
    {NITF_BCS_A, 10, "Specification Version", "DESSHSV" },
    {NITF_BCS_A, 20, "Specification Date", "DESSHSD" },
    {NITF_BCS_A, 120, "Target Namespace", "DESSHTN" },
    {NITF_BCS_A, 125, "Location  Polygon", "DESSHLPG" },
    {NITF_BCS_A, 25, "Location  Point", "DESSHLPT" },
    {NITF_BCS_A, 20, "Location  Identifier.", "DESSHLI" },
    {NITF_BCS_A, 120, "Location Identifier Namespace URI", "DESSHLIN" },
    {NITF_BCS_A, 200, "Abstract", "DESSHABS" },
    {NITF_END, 0, NULL, NULL}
};

/* Define the available descriptions and the default one */
static nitf_TREDescriptionInfo descriptions[] = {
    { "XML_DATA_CONTENT_005", descrip_0005, 5 },
    { "XML_DATA_CONTENT_283", descrip_0283, 283 },
    { "XML_DATA_CONTENT_773", descrip_0773, 773 },
    { NULL, NULL, NITF_TRE_DESC_NO_LENGTH }
};
static nitf_TREDescriptionSet descriptionSet = { 0, descriptions };

NITF_DECLARE_PLUGIN(XML_DATA_CONTENT)


NITF_CXX_ENDGUARD
