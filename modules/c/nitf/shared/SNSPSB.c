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
    {NITF_BCS_N, 2, "Number of sets of sensor parameters", "NUMSNS" }, 
    {NITF_LOOP, 0, NULL, "NUMSNS"},
    {NITF_BCS_N, 2, "Number of Bounding Polygons", "NUM_BP" },
    {NITF_LOOP, 0, NULL, "NUM_BP"},
    {NITF_BCS_N, 2, "Number of Points in the Bounding Polygon", "NUM_PTS" },
    {NITF_LOOP, 0, NULL, "NUM_PTS"},
    {NITF_BCS_N, 15, "Longitude/Easting", "LON" },
    {NITF_BCS_N, 15, "Latitude/Northing", "LAT" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_N, 2, "Number of Bands", "NUM_BND" },
    {NITF_LOOP, 0, NULL, "NUM_BND"},
    {NITF_BCS_A, 5, "Original Scene Band Identification", "BID" },
    {NITF_BCS_N, 5, "Signal Lower Limit", "WS1" },
    {NITF_BCS_N, 5, "Signal Upper Limit", "WS2" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_A, 3, "Resolutions and ground sample distances units", "UNIRES" },
    {NITF_BCS_N, 6, "Resolution in columns", "REX" },
    {NITF_BCS_N, 6, "Resolution in rows", "REY" },
    {NITF_BCS_N, 6, "Ground Sample Distance in columns", "GSX" },
    {NITF_BCS_N, 6, "Ground Sample Distance in rows", "GSY" },
    {NITF_BCS_A, 12, "Location of pixel for GSX and GSY", "GSL" },
    {NITF_BCS_A, 8, "Vector or Mission Name", "PLTFM" },
    {NITF_BCS_A, 8, "Sensor or Instrument Name", "INS" },
    {NITF_BCS_A, 4, "Spectral Mode", "MOD" },
    {NITF_BCS_A, 5, "Processing Level", "PRL" },
    {NITF_BCS_A, 18, "Acquisition Date and Time", "ACT" },
    {NITF_BCS_A, 3, "Unit of the Scene Orientation Angle" , "UNINOA" },
    {NITF_BCS_N, 7, "Scene Orientation Angle", "NOA" },
    {NITF_BCS_A, 3, "Unit of Incidence Angle", "UNIANG" },
    {NITF_BCS_N, 7, "Incidence Angle at Original Scene Center", "ANG" },
    {NITF_BCS_A, 3, "Unit of Altitude", "UNIALT" },
    {NITF_BCS_N, 9, "Altitude of Sensor", "ALT" },
    {NITF_BCS_N, 10, "WGS84 Longitude of Original Scene Centre", "LONSCC" },
    {NITF_BCS_N, 10, "WGS84 Latitude of Original Scene Centre", "LATSCC" },
    {NITF_BCS_A, 3, "Unit of Solar Angles", "UNISAE" },
    {NITF_BCS_N, 7, "Solar Azimuth", "SAZ" },
    {NITF_BCS_N, 7, "Solar Elevation", "SEL" },
    {NITF_BCS_A, 3, "Unit of Attitude Angles", "UNIRPY" },
    {NITF_BCS_N, 7, "Roll of the Sensor", "ROL" },
    {NITF_BCS_N, 7, "Pitch of the Sensor", "PIT" },
    {NITF_BCS_N, 7, "Yaw of the Sensor", "YAW" },
    {NITF_BCS_A, 3, "Unit of Pixel Time", "UNIPXT" },
    {NITF_BCS_N, 14, "Pixel Time", "PIXT" },
    {NITF_BCS_A, 7, "Unit of Attitude Speed", "UNISPE" },
    {NITF_BCS_N, 22, "Roll Speed", "ROS" },
    {NITF_BCS_N, 22, "Pitch Speed", "PIS" },
    {NITF_BCS_N, 22, "Yaw Speed", "YAS" },
    {NITF_BCS_N, 3, "Number of Auxiliary Parameters", "NUM_AUX" },
    {NITF_LOOP, 0, NULL, "NUM_AUX"},
    {NITF_BCS_A, 20, "Auxiliary Parameter ID", "API" },
    {NITF_BCS_A, 1, "Auxiliary Parameter Value Format", "APF" },
    {NITF_BCS_A, 7, "Unit of Auxiliary Parameter", "UNIAPX" },
    {NITF_BCS_N, 10, "Auxiliary Parameter Integer Value", "APN" },
    {NITF_BCS_N, 20, "Auxiliary Parameter Real Value", "APR" },
    {NITF_BCS_A, 20, "Auxiliary Parameter Characters String Value", "APA" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(SNSPSB, description)

NITF_CXX_ENDGUARD
