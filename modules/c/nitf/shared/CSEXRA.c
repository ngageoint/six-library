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
    {NITF_BCS_A, 6, "Sensor", "SENSOR" },
    {NITF_BCS_N, 12, "Time of the First Line of Image", "TIME_FIRST_LINE_IMAGE" },
    {NITF_BCS_N, 12, "Image Duration Time", "TIME_IMAGE_DURATION" },
    {NITF_BCS_N, 5, "Maximum Mean Ground Sample Distance", "MAX_GSD" },
    {NITF_BCS_A, 5, "Along Scan GSD", "ALONG_SCAN_GSD" },
    {NITF_BCS_A, 5, "Cross-Scan GSD", "CROSS_SCAN_GSD" },
    {NITF_BCS_A, 5, "Geometric Mean GSD", "GEO_MEAN_GSD" },
    {NITF_BCS_A, 5, "Along Scan Vertical GSD", "A_S_VERT_GSD" },
    {NITF_BCS_A, 5, "Cross-Scan Vertical GSD", "C_S_VERT_GSD" },
    {NITF_BCS_A, 5, "Geometric Mean Vertical GSD", "GEO_MEAN_VERT_GSD" },
    {NITF_BCS_A, 5, "GSD Beta Angle", "GSD_BETA_ANGLE" },
    {NITF_BCS_N, 5, "Dynamic range of pixels in image", "DYNAMIC_RANGE" },
    {NITF_BCS_N, 7, "Number of Lines", "NUM_LINES" },
    {NITF_BCS_N, 5, "Number of Samples", "NUM_SAMPLES" },
    {NITF_BCS_N, 7, "Nominal Angle to True North", "ANGLE_TO_NORTH" },
    {NITF_BCS_N, 6, "Nominal Obliquity angle", "OBLIQUITY_ANGLE" },
    {NITF_BCS_N, 7, "Azimuth of Obliquity", "AZ_OF_OBLIQUITY" },
    {NITF_BCS_N, 1, "Ground Cover", "GRD_COVER" },
    {NITF_BCS_N, 1, "Snow Depth Category", "SNOW_DEPTH_CAT" },
    {NITF_BCS_N, 7, "Sun Azimuth Angle", "SUN_AZIMUTH" },
    {NITF_BCS_N, 7, "Sun Elevation Angle", "SUN_ELEVATION" },
    {NITF_BCS_A, 3, "Predicted NIIRS", "PREDICTED_NIIRS" },
    {NITF_BCS_N, 3, "Circular Error", "CIRCL_ERR" },
    {NITF_BCS_N, 3, "Linear Error", "LINEAR_ERR" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(CSEXRA, description)

NITF_CXX_ENDGUARD
