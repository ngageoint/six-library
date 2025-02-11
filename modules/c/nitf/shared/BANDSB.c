/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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
    {NITF_BCS_N,  5,  "Number of Bands",                                    "COUNT" },
    {NITF_BCS_A,  24, "Data Representation",                                "RADIOMETRIC_QUANTITY" },
    {NITF_BCS_A,  1,  "Data Representation Unit",                           "RADIOMETRIC_QUANTITY_UNIT" },
    {NITF_BINARY, 4,  "Cube Scale Factor",                                  "SCALE_FACTOR" },
    {NITF_BINARY, 4,  "Cube Additive Factor",                               "ADDITIVE_FACTOR" },
    {NITF_BCS_N,  7,  "Row Ground Sample Distance",                         "ROW_GSD" },
    {NITF_BCS_A,  1,  "Units of Row Ground Sample Distance",                "ROW_GSD_UNIT" },
    {NITF_BCS_N,  7,  "Column Ground Sample Distance",                      "COL_GSD" },
    {NITF_BCS_A,  1,  "Units of Column Ground Sample Distance",             "COL_GSD_UNIT" },
    {NITF_BCS_N,  7,  "Spatial Response Function (Rows)",                   "SPT_RESP_ROW" },
    {NITF_BCS_A,  1,  "Units of Spatial Response Function (Rows)",          "SPT_RESP_UNIT_ROW" },
    {NITF_BCS_N,  7,  "Spatial Response Function (Cols)",                   "SPT_RESP_COL" },
    {NITF_BCS_A,  1,  "Units of Spatial Response Function (Cols)",          "SPT_RESP_UNIT_COL" },
    {NITF_BINARY,48,  "Field reserved for future use",                      "DATA_FLD_1" },
    {NITF_BINARY, 4,  "Bit-wise Existence Mask Field",                      "EXISTENCE_MASK" },
    {NITF_IF, 0, "& 0x80000000", "EXISTENCE_MASK" },
    {NITF_BCS_A, 24,  "Adjustment Surface",                                 "RADIOMETRIC_ADJUSTMENT_SURFACE" },
    {NITF_BINARY, 4,  "Adjustment Altitude Above WGS84 Ellipsoid",          "ATMOSPHERIC_ADJUSTMENT_ALTITUDE" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x40000000", "EXISTENCE_MASK" },
    {NITF_BCS_N,  7,  "Diameter of the lens",                               "DIAMETER" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x20000000", "EXISTENCE_MASK" },
    {NITF_BINARY,32,  "Field reserved for future use",                      "DATA_FLD_2" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x01F80000", "EXISTENCE_MASK" },
    {NITF_BCS_A,  1,  "Wave Length Units",                                  "WAVE_LENGTH_UNIT" },
    {NITF_ENDIF, 0, NULL, NULL},

    {NITF_LOOP, 0, NULL, "COUNT" },

    {NITF_IF, 0, "& 0x10000000", "EXISTENCE_MASK" },
    {NITF_BCS_A, 50,  "Band n Identifier",                                  "BANDID" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x08000000", "EXISTENCE_MASK" },
    {NITF_BCS_N,  1,  "Bad Band Flag",                                      "BAD_BAND" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x04000000", "EXISTENCE_MASK" },
    {NITF_BCS_N,  3,  "NIIRS Value",                                        "NIIRS" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x02000000", "EXISTENCE_MASK" },
    {NITF_BCS_N,  5,  "Band n Focal length",                                "FOCAL_LEN" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x01000000", "EXISTENCE_MASK" },
    {NITF_BCS_N,  7,  "Band n Center Response Wavelength",                  "CWAVE" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x00800000", "EXISTENCE_MASK" },
    {NITF_BCS_N,  7,  "Band n Width",                                       "FWHM" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x00400000", "EXISTENCE_MASK" },
    {NITF_BCS_N,  7,  "Band n Width Uncertainty",                           "FWHM_UNC" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x00200000", "EXISTENCE_MASK" },
    {NITF_BCS_N,  7,  "Band n Nominal Wavelength",                          "NOM_WAVE" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x00100000", "EXISTENCE_MASK" },
    {NITF_BCS_N,  7,  "Band n Nominal Wavelength Uncertainty",              "NOM_WAVE_UNC" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x00080000", "EXISTENCE_MASK" },
    {NITF_BCS_N,  7,  "Band n Lower Wavelength Bound",                      "LBOUND" },
    {NITF_BCS_N,  7,  "Band n Upper Wavelength Bound",                      "UBOUND" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x00040000", "EXISTENCE_MASK" },
    {NITF_BINARY, 4,  "Individual Scale Factor",                            "SCALE_FACTOR" },
    {NITF_BINARY, 4,  "Individual Additive Factor",                         "ADDITIVE_FACTOR" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x00020000", "EXISTENCE_MASK" },
    {NITF_BCS_A, 16,  "Start Time",                                         "START_TIME" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x00010000", "EXISTENCE_MASK" },
    {NITF_BCS_N, 6,   "Integration Time",                                   "INT_TIME" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x00008000", "EXISTENCE_MASK" },
    {NITF_BCS_N, 6,   "Band n Calibration (Dark)",                          "CALDRK" },
    {NITF_BCS_N, 5,   "Band n Calibration (Increment)",                     "CALIBRATION_SENSITIVITY" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x00004000", "EXISTENCE_MASK" },
    {NITF_BCS_N, 7,   "Band n Spatial Response Interval (Row)",             "ROW_GSD" },
    {NITF_IF, 0, "& 0x00002000", "EXISTENCE_MASK" },
    {NITF_BCS_N, 7,   "Band n Spatial Response Interval Uncertainty (Row)", "ROW_GSD_UNC" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_BCS_A, 1,   "Unit of Row Spacing",                                "ROW_GSD_UNIT" },
    {NITF_BCS_N, 7,   "Band n Spatial Response Interval (Col)",             "COL_GSD" },
    {NITF_IF, 0, "& 0x00002000", "EXISTENCE_MASK" },
    {NITF_BCS_N, 7,   "Band n Spatial Response Interval Uncertainty (Col)", "COL_GSD_UNC" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_BCS_A, 1,   "Unit of Column Spacing",                             "COL_GSD_UNIT" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x00001000", "EXISTENCE_MASK" },
    {NITF_BCS_N, 5,   "Band n Background Noise",                            "BKNOISE" },
    {NITF_BCS_N, 5,   "Band n Scene Noise",                                 "SCNNOISE" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x00000800", "EXISTENCE_MASK" },
    {NITF_BCS_N, 7,   "Band n Spatial Response Function (Row)",             "SPT_RESP_FUNCTION_ROW" },
    {NITF_IF, 0, "& 0x00000400", "EXISTENCE_MASK" },
    {NITF_BCS_N, 7,   "Band n Spatial Response Function Uncertainty (Row)", "SPT_RESP_UNC_ROW" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_BCS_A, 1,   "Unit of Spatial Response (Row)",                     "SPT_RESP_UNIT_ROW" },
    {NITF_BCS_N, 7,   "Band n Spatial Response Function (Col)",             "SPT_RESP_FUNCTION_COL" },
    {NITF_IF, 0, "& 0x00000400", "EXISTENCE_MASK" },
    {NITF_BCS_N, 7,   "Band n Spatial Response Function Uncertainty (Col)", "SPT_RESP_UNC_COL" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_BCS_A, 1,   "Unit of Spatial Response (Col)",                     "SPT_RESP_UNIT_COL" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x00000200", "EXISTENCE_MASK" },
    {NITF_BINARY,16,  "Field reserved for future use",                      "DATA_FLD_3" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x00000100", "EXISTENCE_MASK" },
    {NITF_BINARY,24,  "Field reserved for future use",                      "DATA_FLD_4" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x00000080", "EXISTENCE_MASK" },
    {NITF_BINARY,32,  "Field reserved for future use",                      "DATA_FLD_5" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "& 0x00000040", "EXISTENCE_MASK" },
    {NITF_BINARY,48,  "Field reserved for future use",                      "DATA_FLD_6" },
    {NITF_ENDIF, 0, NULL, NULL},

    {NITF_ENDLOOP, 0, NULL, NULL },

    {NITF_IF, 0, "& 0x00000001", "EXISTENCE_MASK" },
    {NITF_BCS_N, 2,  "Number of Auxiliary Band Level Parameters (m)",       "NUM_AUX_B" },
    {NITF_BCS_N, 2,  "Number of Auxiliary Cube Level Parameters (k)",       "NUM_AUX_C" },

    {NITF_LOOP, 0, NULL, "NUM_AUX_B" },

    {NITF_BCS_A, 1,  "Band Auxiliary Parameter Value Format",               "BAPF" },
    {NITF_BCS_A, 7,  "Unit of Band Auxiliary Parameter",                    "UBAP" },

    {NITF_LOOP, 0, NULL, "COUNT" },

    {NITF_IF, 0, "eq I", "BAPF" },
    {NITF_BCS_N, 10, "Auxiliary Parameter Integer Value",                   "APN" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "eq R", "BAPF" },
    {NITF_BINARY, 4, "Auxiliary Parameter Real Value",                      "APR" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "eq A", "BAPF" },
    {NITF_BCS_N, 20, "Auxiliary Parameter ASCII Value",                     "APA" },
    {NITF_ENDIF, 0, NULL, NULL},

    {NITF_ENDLOOP, 0, NULL, NULL },
    {NITF_ENDLOOP, 0, NULL, NULL },

    {NITF_LOOP, 0, NULL, "NUM_AUX_C" },

    {NITF_BCS_A, 1,  "Cube Auxiliary Parameter Value Format",               "CAPF" },
    {NITF_BCS_A, 7,  "Unit of Cube Auxiliary Parameter",                    "UCAP" },
    {NITF_IF, 0, "eq I", "CAPF" },
    {NITF_BCS_N, 10, "Auxiliary Parameter Integer Value",                   "APN" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "eq R", "CAPF" },
    {NITF_BINARY, 4, "Auxiliary Parameter Real Value",                      "APR" },
    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_IF, 0, "eq A", "CAPF" },
    {NITF_BCS_N, 20, "Auxiliary Parameter ASCII Value",                     "APA" },
    {NITF_ENDIF, 0, NULL, NULL},

    {NITF_ENDLOOP, 0, NULL, NULL },
    {NITF_ENDLOOP, 0, NULL, NULL },

    {NITF_ENDIF, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(BANDSB, description)

NITF_CXX_ENDGUARD



