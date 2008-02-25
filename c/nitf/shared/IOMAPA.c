#include <nitf/IOHandle.h>
#include <nitf/TRE.h>
#include <nitf/Record.h>

NITF_CXX_GUARD

static nitf_TREDescription descrip_00006[] = {
    {NITF_BCS_N, 3, "Band Identifier", "BAND_NUMBER",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 1, "Mapping Method to Apply", "MAP_SELECT",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 2, "Scale Factor 2", "S2",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

static nitf_TREDescription descrip_08202[] = {
    {NITF_BCS_N, 3, "Band Identifier", "BAND_NUMBER",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 1, "Mapping Method to Apply", "MAP_SELECT",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 2, "I/O TABLE USED", "TABLE_ID",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 2, "Scale Factor 1", "S1",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 2, "Scale Factor 2", "S2",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_LOOP, 0, NITF_CONST_N, "4096"},
    {NITF_BCS_N, 2, "Output Mapping Value", "OUTPUT_MAP_VALUE",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

static nitf_TREDescription descrip_00016[] = {
    {NITF_BCS_N, 3, "Band Identifier", "BAND_NUMBER",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 1, "Mapping Method to Apply", "MAP_SELECT",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 2, "I/O TABLE USED", "TABLE_ID",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 2, "Scale Factor 1", "S1",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 2, "Scale Factor 2", "S2",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 3, "R Scaling Factor � Whole Part", "R_WHOLE",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 3, "R Scaling Factor � Fractional Part",
     "R_FRACTION", NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE,
     NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

static nitf_TREDescription descrip_00091[] = {
    {NITF_BCS_N, 3, "Band Identifier", "BAND_NUMBER",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 1, "Mapping Method to Apply", "MAP_SELECT",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 1, "I/O TABLE USED", "TABLE_ID",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 2, "Scale Factor 1", "S1",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 2, "Scale Factor 2", "S2",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 1, "Number of Segments", "NO_OF_SEGMENTS",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 4, "Segment Boundary 1", "XOB_1",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 4, "Segment Boundary 2", "XOB_2",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_LOOP, 0, NULL, "NO_OF_SEGMENTS"},
    {NITF_BINARY, 4, "B0 Coefficient", "OUT_B0",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BINARY, 4, "B1 Coefficient", "OUT_B1",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BINARY, 4, "B2 Coefficient", "OUT_B2",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BINARY, 4, "B3 Coefficient", "OUT_B3",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BINARY, 4, "B4 Coefficient", "OUT_B4",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BINARY, 4, "B5 Coefficient", "OUT_B5",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

/* Define the available descriptions and the default one */
static nitf_TREDescriptionInfo descriptions[] = {
    { "IOMAPA_6", descrip_00006, 6 },
    { "IOMAPA_8202", descrip_08202, 8202 },
    { "IOMAPA_16", descrip_00016, 16 },
    { "IOMAPA_91", descrip_00091, 91 },
    { NULL, NULL, NITF_TRE_DESC_NO_LENGTH }
};
static nitf_TREDescriptionSet descriptionSet = { 0, descriptions };

NITF_DECLARE_PLUGIN(IOMAPA)

NITF_CXX_ENDGUARD
