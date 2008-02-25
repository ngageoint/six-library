
#include <nitf/IOHandle.h>
#include <nitf/TRE.h>
#include <nitf/Record.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_A, 2, "Non-linear transform flag", "XFRM_FLAG",
            NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 10, "Scale Factor Relative to RO", "SCALE_FACTOR",
            NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 2, "Anamorphic Correction Indicator", "ANAMRPH_CORR",
            NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 2, "Scan Block Number", "SCANBLK_NUM",
            NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 12,
        "Output Product row number component of grid point index(1,1)",
              "OP_ROW_11",NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 12,
       "Output product column number component of grid point index(1,1)",
               "OP_COL_11",NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 12,
       "Output product row number component of grid point index(1,2)",
               "OP_ROW_12",NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 12,
        "Output product column number component of grid point index(1,2)",
              "OP_COL_12",NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 12,
        "Output product row number component of grid point index(2,1)",
              "OP_ROW_21",NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 12,
         "Output product column number component of grid point index(2,1)",
              "OP_COL_21",NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 12,
          "Output product row number component of grid point index(2,2)",
              "OP_ROW_22",NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 12,
           "Output product column number component of grid point index(2,2)",
              "OP_COL_22",NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 12,
        "Grid point (1,1) row number in full image coodinates",
           "FI_ROW_11",NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 12,
        "Grid point (1,1) column number in full image coodinates",
           "FI_COL_11",NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 12,
        "Grid point (1,2) row number in full image coodinates",
           "FI_ROW_12",NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 12,
        "Grid point (1,2) column number in full image coodinates",
           "FI_COL_12",NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 12,
         "Grid point (2,1) row number in full image coodinates",
           "FI_ROW_21",NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 12,
         "Grid point (2,1) column number in full image coodinates",
           "FI_COL_21",NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 12,
          "Grid point (2,2) row number in full image coodinates",
           "FI_ROW_22",NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 12,
         "Grid point (2,2) column number in full image coodinates",
           "FI_COL_22",NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},

    {NITF_BCS_A, 8,"Full image number of rows","FI_ROW",
                 NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 8, "Full image number of columns", "FI_COL",
                 NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(ICHIPB, description)

NITF_CXX_ENDGUARD
