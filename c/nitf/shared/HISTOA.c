#include <nitf/IOHandle.h>
#include <nitf/TRE.h>
#include <nitf/Record.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_A, 20, "System Type", "SYSTYPE",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 12, "Prior Compression", "PC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 4, "Prior Enhancements", "PE",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 1, "System Specific Remap", "REMAP_FLAG",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 2, "Data Mapping ID from the ESD", "LUTID",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 2, "Number of Processing Events", "NEVENTS",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_LOOP, 0, NULL, "NEVENTS"},
    {NITF_BCS_A, 14, "Processing Date And Time", "PDATE",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 10, "Processing Site", "PSITE",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 10, "Softcopy Processing Application", "PAS",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 1, "Number of Image Processing Comments",
     "NIPCOM", NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE,
     NULL, NULL},
    {NITF_LOOP, 0, NULL, "NIPCOM"},
    {NITF_BCS_A, 80, "Image Processing Comment", "IPCOM",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_N, 2, "Input Bit Depth (actual)", "IBPP",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 3, "Input Pixel Value Type", "IPVTYPE",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 10, "Input Bandwidth Compression", "INBWC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 1, "Display-Ready Flag", "DISP_FLAG",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 1, "Image Rotation", "ROT_FLAG",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},

    {NITF_IF, 0, "== 1", "ROT_FLAG" },

        {NITF_BCS_A, 8, "Angle Rotation", "ROT_ANGLE",
         NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},

    {NITF_ENDIF, 0, NULL, NULL},

    {NITF_BCS_A, 1, "Image Projection", "ASYM_FLAG",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},

    {NITF_IF, 0, "== 1", "ASYM_FLAG" },

        {NITF_BCS_A, 7, "Mag in Line (row) Direction", "ZOOMROW",
         NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},

        {NITF_BCS_A, 7, "Mag in Element (column) Direction",
         "ZOOMCOL", NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE,
         NULL, NULL},

    {NITF_ENDIF, 0, NULL, NULL},

    {NITF_BCS_A, 1, "Asymmetric Correction", "PROJ_FLAG",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},

    {NITF_BCS_N, 1, "Sharpening", "SHARP_FLAG",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},

    {NITF_IF, 0, "== 1", "SHARP_FLAG" },

        {NITF_BCS_N, 2, "Sharpening Family Number", "SHARPFAM",
         NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
        {NITF_BCS_N, 2, "Sharpening Member Number", "SHARPMEM",
         NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},

    {NITF_ENDIF, 0, NULL, NULL},

    {NITF_BCS_N, 1, "Symmetrical Magnification", "MAG_FLAG",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},

    {NITF_IF, 0, "== 1", "MAG_FLAG" },

        {NITF_BCS_A, 7, "Level of Relative Magnification",
         "MAG_LEVEL", NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE,
         NULL, NULL},

    {NITF_ENDIF, 0, NULL, NULL},


    {NITF_BCS_N, 1, "Dynamic Range Adjustment (DRA)", "DRA_FLAG",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},

    {NITF_IF, 0, "== 1", "DRA_FLAG" },

        {NITF_BCS_A, 7, "DRA Multiplier", "DRA_MULT",
         NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},

        {NITF_BCS_N, 5, "DRA Subtractor", "DRA_SUB",
         NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},

    {NITF_ENDIF, 0, NULL, NULL},

    {NITF_BCS_N, 1, "Tonal Transfer Curve (TTC)", "TTC_FLAG",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},


    {NITF_IF, 0, "== 1", "TTC_FLAG" },

        {NITF_BCS_N, 2, "TTC Family Number", "TTCFAM",
         NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
        
        {NITF_BCS_N, 2, "TTC Member Number", "TTCMEM",
         NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},

    {NITF_ENDIF, 0, NULL, NULL},


    {NITF_BCS_N, 1, "Device LUT", "DEVLUT_FLAG",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},


    {NITF_BCS_N, 2, "Output Bit Depth (actual)", "OBPP",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 3, "Output Pixel Value Type", "OPVTYPE",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 10, "Output Bandwidth Compression", "OUTBWC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(HISTOA, description)

NITF_CXX_ENDGUARD
