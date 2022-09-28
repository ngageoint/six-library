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

#if _MSC_VER
#pragma warning(disable: 4820) // '...' : '...' bytes padding added after data member '...'
#pragma warning(disable: 4668) // '...' is not defined as a preprocessor macro, replacing with '...' for '...'
#pragma warning(disable: 5039) // '...': pointer or reference to potentially throwing function passed to '...' function under -EHc. Undefined behavior may occur if this function throws an exception.
#endif

#include <import/nitf.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
        {NITF_BCS_A,   36, "UUID Assigned to the Current Image Plane", "IMAGE_UUID"},
        {NITF_BCS_N,   3,  "Number of GLAS/GFM DES Associated with this Image", "NUM_ASSOC_DES"},
        {NITF_IF,      0,  "> 0", "NUM_ASSOC_DES"},
        {NITF_LOOP,    0,  NULL, "NUM_ASSOC_DES"},
        {NITF_BCS_A,  36,  "UUID of Associated DES", "ASSOC_DES_ID"},
        {NITF_ENDLOOP, 0,  NULL, NULL},  // NUM_ASSOC_DES
        {NITF_ENDIF,   0,  NULL, NULL},  // NUM_ASSOC_DES > 0
        {NITF_BCS_A,   6,  "Platform Identifier", "PLATFORM_ID"},
        {NITF_BCS_A,   6,  "Payload Identifier", "PAYLOAD_ID"},
        {NITF_BCS_A,   6,  "Sensor Identifier", "SENSOR_ID"},
        {NITF_BCS_A,   1,  "Sensor Type", "SENSOR_TYPE"},
        {NITF_BCS_A,  12,  "X Coordinate of Ground Reference Point", "GROUND_REF_POINT_X"},
        {NITF_BCS_A,  12,  "Y Coordinate of Ground Reference Point", "GROUND_REF_POINT_Y"},
        {NITF_BCS_A,  12,  "Z Coordinate of Ground Reference Point", "GROUND_REF_POINT_Z"},
        {NITF_IF,      0,  "eq S", "SENSOR_TYPE"},
        {NITF_BCS_N,   8,  "Day of First Line", "DAY_FIRST_LINE_IMAGE"},
        {NITF_BCS_N,  15,  "Time of First Line", "TIME_FIRST_LINE"},
        {NITF_BCS_N,  16,  "Time of Image Duration", "TIME_IMAGE_DURATION"},
        {NITF_ENDIF,   0,  NULL, NULL},  // SENSOR_TYPE eq S
        {NITF_IF,      0,  "eq F", "SENSOR_TYPE"},
        {NITF_BCS_N,   1,  "Location of Frame Time Stamp", "TIME_STAMP_LOC"},
        {NITF_IF,      0,  "== 0", "TIME_STAMP_LOC"},
        {NITF_BCS_N,   9,  "Reference Frame Identifier", "REFERENCE_FRAME_NUM"},
        {NITF_BCS_A,  24,  "Base Timestamp", "BASE_TIMESTAMP"},
        {NITF_BINARY,  8,  "Delta Time Duration (UINT64)", "DT_MULTIPLIER"},
        {NITF_BINARY,  1,  "Byte Size of the DT Values (UINT8)", "DT_SIZE"},
        {NITF_BINARY,  4,  "Numer of Frames int the Current Temporal Block (UINT32)", "NUMBER_FRAMES"},
        {NITF_BINARY,  4,  "Number of DT Values (UINT32)", "NUMBER_DT"},

        {NITF_IF,      0,  "eq 1", "DT_SIZE"},  // UINT8
        {NITF_LOOP,    0,  NULL, "NUMBER_DT"},
        {NITF_BINARY,  1,  "DT Value", "DT"},
        {NITF_ENDLOOP, 0,  NULL, NULL},  // NUMBER_DT
        {NITF_ENDIF,   0,  NULL, NULL},  // DT_SIZE eq 1

        {NITF_IF,      0,  "eq 2", "DT_SIZE"},  // UINT16
        {NITF_LOOP,    0,  NULL, "NUMBER_DT"},
        {NITF_BINARY,  2,  "DT Value", "DT"},
        {NITF_ENDLOOP, 0,  NULL, NULL},  // NUMBER_DT
        {NITF_ENDIF,   0,  NULL, NULL},  // DT_SIZE eq 2

        {NITF_IF,      0,  "eq 3", "DT_SIZE"},  // UINT24
        {NITF_LOOP,    0,  NULL, "NUMBER_DT"},
        {NITF_BINARY,  3,  "DT Value", "DT"},
        {NITF_ENDLOOP, 0,  NULL, NULL},  // NUMBER_DT
        {NITF_ENDIF,   0,  NULL, NULL},  // DT_SIZE eq 3

        {NITF_IF,      0,  "eq 4", "DT_SIZE"},  // UINT32
        {NITF_LOOP,    0,  NULL, "NUMBER_DT"},
        {NITF_BINARY,  4,  "DT Value", "DT"},
        {NITF_ENDLOOP, 0,  NULL, NULL},  // NUMBER_DT
        {NITF_ENDIF,   0,  NULL, NULL},  // DT_SIZE eq 4

        {NITF_IF,      0,  "eq 5", "DT_SIZE"},  // UINT40
        {NITF_LOOP,    0,  NULL, "NUMBER_DT"},
        {NITF_BINARY,  5,  "DT Value", "DT"},
        {NITF_ENDLOOP, 0,  NULL, NULL},  // NUMBER_DT
        {NITF_ENDIF,   0,  NULL, NULL},  // DT_SIZE eq 5

        {NITF_IF,      0,  "eq 6", "DT_SIZE"},  // UINT48
        {NITF_LOOP,    0,  NULL, "NUMBER_DT"},
        {NITF_BINARY,  6,  "DT Value", "DT"},
        {NITF_ENDLOOP, 0,  NULL, NULL},  // NUMBER_DT
        {NITF_ENDIF,   0,  NULL, NULL},  // DT_SIZE eq 6

        {NITF_IF,      0,  "eq 7", "DT_SIZE"},  // UINT56
        {NITF_LOOP,    0,  NULL, "NUMBER_DT"},
        {NITF_BINARY,  7,  "DT Value", "DT"},
        {NITF_ENDLOOP, 0,  NULL, NULL},  // NUMBER_DT
        {NITF_ENDIF,   0,  NULL, NULL},  // DT_SIZE eq 7

        {NITF_IF,      0,  "eq 8", "DT_SIZE"},  // UINT64
        {NITF_LOOP,    0,  NULL, "NUMBER_DT"},
        {NITF_BINARY,  8,  "DT Value", "DT"},
        {NITF_ENDLOOP, 0,  NULL, NULL},  // NUMBER_DT
        {NITF_ENDIF,   0,  NULL, NULL},  // DT_SIZE eq 8

        {NITF_ENDIF,   0,  NULL, NULL},  // TIME_STAMP_LOC eq 0
        {NITF_ENDIF,   0,  NULL, NULL},  // SENSOR_TYPE eq F

        {NITF_BCS_A,   12,  "Max Mean GSD", "MAX_GSD"},
        {NITF_BCS_A,   12,  "Measured Along-Scan GSD", "ALONG_SCAN_GSD"},
        {NITF_BCS_A,   12,  "Measured Cross-Scan GSD", "CROSS_SCAN_GSD"},
        {NITF_BCS_A,   12,  "Measured Geometric Mean GSD", "GEO_MEAN_GSD"},
        {NITF_BCS_A,   12,  "Measured Along-Scan Vertical GSD", "A_S_VERT_GSD"},
        {NITF_BCS_A,   12,  "Measured Cross-Scan Vertical GSD", "C_S_VERT_GSD"},
        {NITF_BCS_A,   12,  "Measured Geometric Mean Vertical GSD", "GEO_MEAN_VERT_GSD"},
        {NITF_BCS_A,   5,  "Angle Between Along-Scan and Cross-Scan Directions", "GSD_BETA_ANGLE"},
        {NITF_BCS_A,   5,  "Dynamic Range of Pixels", "DYNAMIC_RANGE"},
        {NITF_BCS_N,   7,  "Number of Lines in Entire Image Plane", "NUM_LINES"},
        {NITF_BCS_N,   5,  "Number of Samples per Line in Entire Image Plane", "NUM_SAMPLES"},
        {NITF_BCS_A,   7,  "Nominal Angle to North", "ANGLE_TO_NORTH"},
        {NITF_BCS_A,   6,  "Nominal Obliquity Angle", "OBLIQUITY_ANGLE"},
        {NITF_BCS_A,   7,  "Azimuth of Obliquity", "AZ_OF_OBLIQUITY"},
        {NITF_BCS_N,   1,  "Atmospheric Refraction Flag", "ATM_REFR_FLAG"},
        {NITF_BCS_N,   1,  "Velocity Aberration Flag", "VEL_ABER_FLAG"},
        {NITF_BCS_N,   1,  "Ground Cover Flag", "GRD_COVER"},
        {NITF_BCS_N,   1,  "Snow Depth Category", "SNOW_DEPTH_CATEGORY"},
        {NITF_BCS_A,   7,  "Sun Azimuth Angle", "SUN_AZIMUTH"},
        {NITF_BCS_A,   7,  "Sun Elevation Angle", "SUN_ELEVATION"},
        {NITF_BCS_A,   3,  "NIIRS Value for the Mono Collection at Principal Target", "PREDICTED_NIIRS"},
        {NITF_BCS_A,   5,  "Circular Error", "CIRCL_ERR"},
        {NITF_BCS_A,   5,  "Linear Error", "LINEAR_ERR"},
        {NITF_BCS_A,   3,  "Cloud Cover", "CLOUD_COVER"},
        {NITF_BCS_A,   1,  "Rolling Shutter Flag", "ROLLING_SHUTTER_FLAG"},
        {NITF_BCS_A,   1,  "Time Un-Modeled Error Flag", "UE_TIME_FLAG"},
        {NITF_BCS_N,   5,  "Length of Reserved Portion", "RESERVED_LEN"},
        {NITF_BINARY,  NITF_TRE_CONDITIONAL_LENGTH,
                           "Reserved Data Portion", "RESERVED", "RESERVED_LEN"},

        {NITF_END,     0,  NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(CSEXRB, description)

NITF_CXX_ENDGUARD
