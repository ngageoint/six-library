#include <nitf/IOHandle.h>
#include <nitf/TRE.h>
#include <nitf/Record.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_N, 1, "Success", "SUCCESS",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 7, "Error - Bias", "ERR_BIAS",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 7, "Error - Random", "ERR_RAND",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 6, "Line Offset", "LINE_OFF",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 5, "Sample Offset", "SAMP_OFF",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 8, "Geodetic Latitude Offset", "LAT_OFF",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 9, "Geodetic Longitude Offset", "LONG_OFF",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 5, "Geodetic Height Offset", "HEIGHT_OFF",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 6, "Line Scale", "LINE_SCALE",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 5, "Sample Scale", "SAMP_SCALE",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 8, "Geodetic Latitude Scale", "LAT_SCALE",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 9, "Geodetic Longitude Scale", "LONG_SCALE",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 5, "Geodetic Height Scale", "HEIGHT_SCALE",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_LOOP, 0, NITF_CONST_N, "20"},
    {NITF_BCS_A, 12, "Line Numerator Coefficient",
     "LINE_NUM_COEFF", NITF_VAL_BCS_A_PLUS,
     NITF_NO_RANGE, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_LOOP, 0, NITF_CONST_N, "20"},
    {NITF_BCS_A, 12, "Line Denominator Coefficient",
     "LINE_DEN_COEFF", NITF_VAL_BCS_A_PLUS,
     NITF_NO_RANGE, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_LOOP, 0, NITF_CONST_N, "20"},
    {NITF_BCS_A, 12, "Sample Numerator Coefficient",
     "SAMP_NUM_COEFF", NITF_VAL_BCS_A_PLUS,
     NITF_NO_RANGE, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_LOOP, 0, NITF_CONST_N, "20"},
    {NITF_BCS_A, 12, "Sample Denominator Coefficient",
     "SAMP_DEN_COEFF", NITF_VAL_BCS_A_PLUS,
     NITF_NO_RANGE, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(RPC00B, description)

NITF_CXX_ENDGUARD
