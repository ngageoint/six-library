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


#include <import/nitf.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_A,  1,     "General Data",                 "GENERAL_DATA"},        /* 01 */
    {NITF_IF,     0,     "eq Y",                         "GENERAL_DATA"},
    {NITF_BCS_A,  25,    "Sensor Name",                  "SENSOR"},              /* 01a */
    {NITF_BCS_A,  32,    "Sensor URI",                   "SENSOR_URI"},          /* 01b */
    {NITF_BCS_A,  25,    "Platform Common Name",         "PLATFORM"},            /* 01c */
    {NITF_BCS_A,  32,    "Platform URI",                 "PLATFORM_URI"},        /* 01d */
    {NITF_BCS_A,  10,    "Operation Domain",             "OPERATION_DOMAIN"},    /* 01e */
    {NITF_BCS_N,  1,     "Content Level",                "CONTENT_LEVEL"},       /* 01f */
    {NITF_BCS_A,  5,     "Geodetic System",              "GEODETIC_SYSTEM"},     /* 01g */
    {NITF_BCS_A,  1,     "Geodetic Type",                "GEODETIC_TYPE"},       /* 01h */
    {NITF_BCS_A,  3,     "Elevation Datum",              "ELEVATION_DATUM"},     /* 01i */
    {NITF_BCS_A,  2,     "Length Unit",                  "LENGTH_UNIT"},         /* 01j */
    {NITF_BCS_A,  3,     "Angular Unit",                 "ANGULAR_UNIT"},        /* 01k */
    {NITF_BCS_N,  8,     "Start Date",                   "START_DATE"},          /* 01l */
    {NITF_BCS_N,  14,    "Start Time",                   "START_TIME"},          /* 01m */
    {NITF_BCS_N,  8,     "End Date",                     "END_DATE"},            /* 01n */
    {NITF_BCS_N,  14,    "End Time",                     "END_TIME"},            /* 01o */
    {NITF_BCS_N,  2,     "Generation Count",             "GENERATION_COUNT"},    /* 01p */
    {NITF_BCS_N,  8,     "Generation Date",              "GENERATION_DATE"},     /* 01q */
    {NITF_BCS_N,  10,    "Generation Time",              "GENERATION_TIME"},     /* 01r */
    {NITF_ENDIF,  0,     NULL,                           NULL},

    {NITF_BCS_A,  1,     "Sensor Array Data",            "SENSOR_ARRAY_DATA"},   /* 02 */
    {NITF_IF,     0,     "eq Y",                         "SENSOR_ARRAY_DATA"},
    {NITF_BCS_A,  20,    "Detection",                    "DETECTION"},           /* 02a */
    {NITF_BCS_N,  8,     "Row Detectors",                "ROW_DETECTORS"},       /* 02b */
    {NITF_BCS_N,  8,     "Column Detectors",             "COLUMN_DETECTORS"},    /* 02c */
    {NITF_BCS_N,  8,     "Row Metric",                   "ROW_METRIC"},          /* 02d */
    {NITF_BCS_N,  8,     "Column Metric",                "COLUMN_METRIC"},       /* 02e */
    {NITF_BCS_N,  8,     "Focal Length",                 "FOCAL_LENGTH"},        /* 02f */
    {NITF_BCS_N,  8,     "Row Field of View",            "ROW_FOV"},             /* 02g */
    {NITF_BCS_N,  8,     "Column Field of View",         "COLUMN_FOV"},          /* 02h */
    {NITF_BCS_A,  1,     "Calibrated",                   "CALIBRATED"},          /* 02i */
    {NITF_ENDIF,  0,     NULL,                           NULL},

    {NITF_BCS_A,  1,     "Sensor Calibration Data",      "SENSOR_CALIBRATION_DATA"}, /* 03 */
    {NITF_IF,     0,     "eq Y",                         "SENSOR_CALIBRATION_DATA"},
    {NITF_BCS_A,  2,     "Calibration Unit System",      "CALIBRATION_UNIT"},    /* 03a */
    {NITF_BCS_N,  9,     "Principal Point Offset X",     "PRINCIPAL_POINT_OFFSET_X"}, /* 03b */
    {NITF_BCS_N,  9,     "Principal Point Offset Y",     "PRINCIPAL_POINT_OFFSET_Y"}, /* 03c */
    {NITF_BCS_A,  12,    "Radial Distortion Coeff 1",    "RADIAL_DISTORT_1"},    /* 03d */
    {NITF_BCS_A,  12,    "Radial Distortion Coeff 2",    "RADIAL_DISTORT_2"},    /* 03e */
    {NITF_BCS_A,  12,    "Radial Distortion Coeff 3",    "RADIAL_DISTORT_3"},    /* 03f */
    {NITF_BCS_N,  9,     "Radial Distortion Fit Limit",  "RADIAL_DISTORT_LIMIT"},/* 03g */
    {NITF_BCS_A,  12,    "Decentering Distortion Coeff 1","DECENT_DISTORT_1"},   /* 03h */
    {NITF_BCS_A,  12,    "Decentering Distortion Coeff 2","DECENT_DISTORT_2"},   /* 03i */
    {NITF_BCS_A,  12,    "Affinity Distortion Coeff 1",  "AFFINITY_DISTORT_1"},  /* 03j */
    {NITF_BCS_A,  12,    "Affinity Distortion Coeff 2",  "AFFINITY_DISTORT_2"},  /* 03k */
    {NITF_BCS_N,  8,     "Calibration Date",             "CALIBRATION_DATE"},    /* 03l */
    {NITF_ENDIF,  0,     NULL,                           NULL},

    {NITF_BCS_A,  1,     "Image Formation Data",         "IMAGE_FORMATION_DATA"},/* 04 */
    {NITF_IF,     0,     "eq Y",                         "IMAGE_FORMATION_DATA"},
    {NITF_BCS_A,  15,    "Imaging Method",               "METHOD"},              /* 04a */
    {NITF_BCS_A,  3,     "Imaging Mode",                 "MODE"},                /* 04b */
    {NITF_BCS_N,  8,     "Row Count",                    "ROW_COUNT"},           /* 04c */
    {NITF_BCS_N,  8,     "Column Count",                 "COLUMN_COUNT"},        /* 04d */
    {NITF_BCS_N,  8,     "Row Detection Set",            "ROW_SET"},             /* 04e */
    {NITF_BCS_N,  8,     "Column Detection Set",         "COLUMN_SET"},          /* 04f */
    {NITF_BCS_N,  10,    "Row Detection Rate",           "ROW_RATE"},            /* 04g */
    {NITF_BCS_N,  10,    "Column Detection Rate",        "COLUMN_RATE"},         /* 04h */
    {NITF_BCS_N,  8,     "First Collected Pixel Row",    "FIRST_PIXEL_ROW"},     /* 04i */
    {NITF_BCS_N,  8,     "First Collected Pixel Column", "FIRST_PIXEL_COLUMN"},  /* 04j */
    {NITF_BCS_N,  1,     "Image Transform Parameter Count","TRANSFORM_PARAMS"},  /* 04k */
    {NITF_IF,     0,     ">= 1",                         "TRANSFORM_PARAMS"},
    {NITF_BCS_A,  12,    "Image Transform Parameter 1",  "TRANSFORM_PARAM_1"},   /* 04l */
    {NITF_ENDIF,  0,     NULL,                           NULL},
    {NITF_IF,     0,     ">= 2",                         "TRANSFORM_PARAMS"},
    {NITF_BCS_A,  12,    "Image Transform Parameter 2",  "TRANSFORM_PARAM_2"},   /* 04m */
    {NITF_ENDIF,  0,     NULL,                           NULL},
    {NITF_IF,     0,     ">= 3",                         "TRANSFORM_PARAMS"},
    {NITF_BCS_A,  12,    "Image Transform Parameter 3",  "TRANSFORM_PARAM_3"},   /* 04n */
    {NITF_ENDIF,  0,     NULL,                           NULL},
    {NITF_IF,     0,     ">= 4",                         "TRANSFORM_PARAMS"},
    {NITF_BCS_A,  12,    "Image Transform Parameter 4",  "TRANSFORM_PARAM_4"},   /* 04o */
    {NITF_ENDIF,  0,     NULL,                           NULL},
    {NITF_IF,     0,     ">= 5",                         "TRANSFORM_PARAMS"},
    {NITF_BCS_A,  12,    "Image Transform Parameter 5",  "TRANSFORM_PARAM_5"},   /* 04p */
    {NITF_ENDIF,  0,     NULL,                           NULL},
    {NITF_IF,     0,     ">= 6",                         "TRANSFORM_PARAMS"},
    {NITF_BCS_A,  12,    "Image Transform Parameter 6",  "TRANSFORM_PARAM_6"},   /* 04q */
    {NITF_ENDIF,  0,     NULL,                           NULL},
    {NITF_IF,     0,     ">= 7",                         "TRANSFORM_PARAMS"},
    {NITF_BCS_A,  12,    "Image Transform Parameter 7",  "TRANSFORM_PARAM_7"},   /* 04r */
    {NITF_ENDIF,  0,     NULL,                           NULL},
    {NITF_IF,     0,     ">= 8",                         "TRANSFORM_PARAMS"},
    {NITF_BCS_A,  12,    "Image Transform Parameter 8",  "TRANSFORM_PARAM_8"},   /* 04s */
    {NITF_ENDIF,  0,     NULL,                           NULL},
    {NITF_ENDIF,  0,     NULL,                           NULL},

    {NITF_BCS_N,  12,    "Reference Time",               "REFERENCE_TIME"},      /* 05a */
    {NITF_BCS_N,  8,     "Reference Pixel Row",          "REFERENCE_ROW"},       /* 05b */
    {NITF_BCS_N,  8,     "Reference Pixel Column",       "REFERENCE_COLUMN"},    /* 05c */

    {NITF_BCS_N,  11,    "Latitude or X",                "LATITUDE_OR_X"},       /* 06a */
    {NITF_BCS_N,  12,    "Longitude or Y",               "LONGITUDE_OR_Y"},      /* 06b */
    {NITF_BCS_N,  11,    "Altitude or Z",                "ALTITUDE_OR_Z"},       /* 06c */
    {NITF_BCS_N,  8,     "Sensor X Position Offset",     "SENSOR_X_OFFSET"},     /* 06d */
    {NITF_BCS_N,  8,     "Sensor Y Position Offset",     "SENSOR_Y_OFFSET"},     /* 06e */
    {NITF_BCS_N,  8,     "Sensor Z Position Offset",     "SENSOR_Z_OFFSET"},     /* 06f */

    {NITF_BCS_A,  1,     "Attitude Euler Angles",        "ATTITUDE_EULER_ANGLES"},/* 07 */
    {NITF_IF,     0,     "eq Y",                         "ATTITUDE_EULER_ANGLES"},
    {NITF_BCS_N,  1,     "Sensor Angle Model",           "SENSOR_ANGLE_MODEL"},  /* 07a */
    {NITF_BCS_N,  10,    "Sensor Angle 1",               "SENSOR_ANGLE_1"},      /* 07b */
    {NITF_BCS_N,  9,     "Sensor Angle 2",               "SENSOR_ANGLE_2"},      /* 07c */
    {NITF_BCS_N,  10,    "Sensor Angle 3",               "SENSOR_ANGLE_3"},      /* 07d */
    {NITF_BCS_A,  1,     "Platform Relative Angles",     "PLATFORM_RELATIVE"},   /* 07e */
    {NITF_BCS_N,  9,     "Platform Heading",             "PLATFORM_HEADING"},    /* 07f */
    {NITF_BCS_N,  9,     "Platform Pitch",               "PLATFORM_PITCH"},      /* 07g */
    {NITF_BCS_N,  10,    "Platform Roll",                "PLATFORM_ROLL"},       /* 07h */
    {NITF_ENDIF,  0,     NULL,                           NULL},

    {NITF_BCS_A,  1,     "Attitude Unit Vectors",        "ATTITUDE_UNIT_VECTORS"},/* 08 */
    {NITF_IF,     0,     "eq Y",                         "ATTITUDE_UNIT_VECTORS"},
    {NITF_BCS_N,  10,    "Image Coord X Unit Vector 1",  "ICX_NORTH_OR_X"},      /* 08a */
    {NITF_BCS_N,  10,    "Image Coord X Unit Vector 2",  "ICX_EAST_OR_Y"},       /* 08b */
    {NITF_BCS_N,  10,    "Image Coord X Unit Vector 3",  "ICX_DOWN_OR_Z"},       /* 08c */
    {NITF_BCS_N,  10,    "Image Coord Y Unit Vector 1",  "ICY_NORTH_OR_X"},      /* 08d */
    {NITF_BCS_N,  10,    "Image Coord Y Unit Vector 2",  "ICY_EAST_OR_Y"},       /* 08e */
    {NITF_BCS_N,  10,    "Image Coord Y Unit Vector 3",  "ICY_DOWN_OR_Z"},       /* 08f */
    {NITF_BCS_N,  10,    "Image Coord Z Unit Vector 1",  "ICZ_NORTH_OR_X"},      /* 08g */
    {NITF_BCS_N,  10,    "Image Coord Z Unit Vector 2",  "ICZ_EAST_OR_Y"},       /* 08h */
    {NITF_BCS_N,  10,    "Image Coord Z Unit Vector 3",  "ICZ_DOWN_OR_Z"},       /* 08i */
    {NITF_ENDIF,  0,     NULL,                           NULL},

    {NITF_BCS_A,  1,     "Attitude Quaternion",          "ATTITUDE_QUATERNION"}, /* 09 */
    {NITF_IF,     0,     "eq Y",                         "ATTITUDE_QUATERNION"},
    {NITF_BCS_N,  10,    "Attitude Quaternion Vector 1", "ATTITUDE_Q1"},         /* 09a */
    {NITF_BCS_N,  10,    "Attitude Quaternion Vector 2", "ATTITUDE_Q2"},         /* 09b */
    {NITF_BCS_N,  10,    "Attitude Quaternion Vector 3", "ATTITUDE_Q3"},         /* 09c */
    {NITF_BCS_N,  10,    "Attitude Scalar Component",    "ATTITUDE_Q4"},         /* 09d */
    {NITF_ENDIF,  0,     NULL,                           NULL},

    {NITF_BCS_A,  1,     "Sensor Velocity Data",         "SENSOR_VELOCITY_DATA"},/* 10 */
    {NITF_IF,     0,     "eq Y",                         "SENSOR_VELOCITY_DATA"},
    {NITF_BCS_N,  9,     "Sensor North Velocity",        "VELOCITY_NORTH_OR_X"}, /* 10a */
    {NITF_BCS_N,  9,     "Sensor East Velocity",         "VELOCITY_EAST_OR_Y"},  /* 10b */
    {NITF_BCS_N,  9,     "Sensor Down Velocity",         "VELOCITY_DOWN_OR_Z"},  /* 10c */
    {NITF_ENDIF,  0,     NULL,                           NULL},

    {NITF_BCS_N,  2,     "Point Set Data",               "POINT_SET_DATA"},      /* 11 */
    {NITF_LOOP,   0,     NULL,                           "POINT_SET_DATA"},
    {NITF_BCS_A,  25,    "Point Set Type",               "POINT_SET_TYPE"},      /* 11a */
    {NITF_BCS_N,  3,     "Point Count",                  "POINT_COUNT"},         /* 11b */
    {NITF_LOOP,   0,     NULL,                           "POINT_COUNT"},
    {NITF_BCS_N,  8,     "Point Row Location",           "P_ROW"},               /* 11c */
    {NITF_BCS_N,  8,     "Point Column Location",        "P_COLUMN"},            /* 11d */
    {NITF_BCS_N,  10,    "Point Latitude",               "P_LATITUDE"},          /* 11e */
    {NITF_BCS_N,  11,    "Point Longitude",              "P_LONGITUDE"},         /* 11f */
    {NITF_BCS_N,  6,     "Point Elevation",              "P_ELEVATION"},         /* 11g */
    {NITF_BCS_N,  8,     "Point Range",                  "P_RANGE"},             /* 11h */
    {NITF_ENDLOOP,0,    NULL,                           NULL},
    {NITF_ENDLOOP,0,    NULL,                           NULL},

    /** Time Stamped Data has different types, sizes, and counts depending on
        the value of TIME_STAMP_TYPE. There's not easy way to handle this right
        now so we'll use lots of NITF_IF statements.
     **/
    {NITF_BCS_N, 2,     "Time Stamped Data",            "TIME_STAMPED_DATA_SETS"},/* 12 */
    {NITF_LOOP,  0,     NULL,                           "TIME_STAMPED_DATA_SETS"},
    {NITF_BCS_A, 3,     "Time Stamp Type",              "TIME_STAMP_TYPE"},     /* 12a */
    {NITF_BCS_N, 4,     "Time Stamp Parameter Count",   "TIME_STAMP_COUNT"},    /* 12b */
    {NITF_LOOP,  0,     NULL,                           "TIME_STAMP_COUNT"},
    {NITF_BCS_N, 12,    "Time Stamp Time",              "TIME_STAMP_TIME"},     /* 12c */

    /** Start Time Stamped Data **/
    {NITF_IF,    0,     "eq 06a",                       "TIME_STAMP_TYPE"},     /* 12d */
    {NITF_BCS_N, 11,    "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 06b",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 12,    "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 06c",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 11,    "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 06d",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 8,     "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 06e",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 8,     "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 06f",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 8,     "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 07b",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 10,    "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 07c",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 9,     "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 07d",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 10,    "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 07f",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 9,     "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 07g",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 9,     "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 07h",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 10,    "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 08a",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 10,    "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 08b",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 10,    "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 08c",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 10,    "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 08d",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 10,    "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 08e",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 10,    "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 08f",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 10,    "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 08g",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 10,    "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 08h",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 10,    "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 08i",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 10,    "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 09a",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 10,    "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 09b",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 10,    "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 09c",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 10,    "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 09d",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 10,    "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 10a",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 9,     "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 10b",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 9,     "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 10c",                       "TIME_STAMP_TYPE"},
    {NITF_BCS_N, 9,     "Time Stamp Value",             "TIME_STAMP_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    /** End Time Stamped Data **/
    {NITF_ENDLOOP,0,    NULL,                           NULL},
    {NITF_ENDLOOP,0,    NULL,                           NULL},

    /** Pixel Referenced Data has different types, sizes, and counts depending on
        the value of PIXEL_REFERENCE_TYPE. There's not easy way to handle this right
        now so we'll use lots of NITF_IF statements.
     **/
    {NITF_BCS_N, 2,     "Pixel Reference Data",         "PIXEL_REFERENCED_DATA_SETS"},/* 13 */
    {NITF_LOOP,  0,     NULL,                           "PIXEL_REFERENCED_DATA_SETS"},
    {NITF_BCS_A, 3,     "Pixel Reference Type",         "PIXEL_REFERENCE_TYPE"},   /* 13a */
    {NITF_BCS_N, 4,     "Pixel Reference Parameter Count","PIXEL_REFERENCE_COUNT"},/* 13b */
    {NITF_LOOP,  0,     NULL,                           "PIXEL_REFERENCE_COUNT"},
    {NITF_BCS_N, 8,     "Pixel Reference Row",          "PIXEL_REFERENCE_ROW"},    /* 13c */
    {NITF_BCS_N, 8,     "Pixel Reference Column",       "PIXEL_REFERENCE_COLUMN"}, /* 13d */

    /** Start Pixel Referenced Data **/
    {NITF_IF,    0,     "eq 06a",                       "PIXEL_REFERENCE_TYPE"},   /* 13e */
    {NITF_BCS_N, 11,    "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 06b",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 12,    "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 06c",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 11,    "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 06d",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 8,     "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 06e",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 8,     "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 06f",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 8,     "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 07b",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 10,    "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 07c",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 9,     "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 07d",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 10,    "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 07f",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 9,     "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 07g",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 9,     "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 07h",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 10,    "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 08a",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 10,    "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 08b",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 10,    "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 08c",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 10,    "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 08d",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 10,    "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 08e",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 10,    "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 08f",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 10,    "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 08g",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 10,    "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 08h",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 10,    "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 08i",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 10,    "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 09a",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 10,    "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 09b",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 10,    "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 09c",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 10,    "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 09d",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 10,    "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 10a",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 9,     "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 10b",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 9,     "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    {NITF_IF,    0,     "eq 10c",                       "PIXEL_REFERENCE_TYPE"},
    {NITF_BCS_N, 9,     "Pixel Reference Value",        "PIXEL_REFERENCE_VALUE"},
    {NITF_ENDIF, 0,     NULL,                           NULL},
    /** End Pixel Referenced Data **/
    {NITF_ENDLOOP,0,    NULL,                           NULL},
    {NITF_ENDLOOP,0,    NULL,                           NULL},

    {NITF_BCS_N, 3,     "Uncertainty Data",             "UNCERTAINTY_DATA"},    /* 14 */
    {NITF_LOOP,  0,     NULL,                           "UNCERTAINTY_DATA"},
    {NITF_BCS_A, 11,    "Uncertainty First Index",      "UNCERTAINTY_FIRST_TYPE"}, /* 14a */
    {NITF_BCS_A, 11,    "Uncertainty Second Index",     "UNCERTAINTY_SECOND_TYPE"},/* 14b */
    {NITF_BCS_A, 10,    "Uncertainty Value",            "UNCERTAINTY_VALUE"},      /* 14c */
    {NITF_ENDLOOP,0,    NULL,                           NULL},

    {NITF_BCS_N, 3,     "Additional Parameters",        "ADDITIONAL_PARAMETER_DATA"},/* 15 */
    {NITF_LOOP,  0,     NULL,                           "ADDITIONAL_PARAMETER_DATA"},
    {NITF_BCS_A, 25,    "Parameter Name",               "PARAMETER_NAME"},      /* 15a */
    {NITF_BCS_N, 3,     "Parameter Field Size",         "PARAMETER_SIZE"},      /* 15b */
    {NITF_BCS_N, 4,     "Parameter Value Count",        "PARAMETER_COUNT"},     /* 15c */
    {NITF_LOOP,  0,     NULL,                           "PARAMETER_COUNT"},
    {NITF_BINARY,NITF_TRE_CONDITIONAL_LENGTH,"Parameter Value",
                        "PARAMETER_VALUE","PARAMETER_SIZE"},                    /* 15d */
    {NITF_ENDLOOP,0,    NULL,                           NULL},
    {NITF_ENDLOOP,0,    NULL,                           NULL},
    {NITF_END, 0,       NULL,                           NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(SENSRB, description)

NITF_CXX_ENDGUARD
