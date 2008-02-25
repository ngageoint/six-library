
#include <nitf/IOHandle.h>
#include <nitf/TRE.h>
#include <nitf/Record.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_A, 25, "Aircraft Location", "ACFT_LOC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 6, "Aircraft Location Accuracy", "ACFT_LOC_ACCY",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 6, "Aircraft Altitude", "ACFT_ALT",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 25, "Reference Point Location", "RP_LOC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 6, "Reference Point Location Accuracy", "RP_LOC_ACCY",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 6, "Reference Point Elevation", "RP_ELV",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 7, "Range Offset", "OF_PC_R",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 7, "Azimuth Offset", "OF_PC_A",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 7, "Cosine of Grazing Angle", "COSGRZ",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 7, "Estimated Slant Range", "RGCRP",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 1, "Right/Left Map", "RLMAP",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 5, "RP Row Number", "RP_ROW",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 5, "RP Column Number", "RP_COL",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 10, "Range Unit Vector North","C_R_NC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 10, "Range Unit Vector East", "C_R_EC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 10, "Range Unit Vector Down", "C_R_DC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 9, "Azimuth Unit Vect North", "C_AZ_NC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 9, "Azimuth Unit Vect East", "C_AZ_EC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 9, "Azimuth Unit Vect Down", "C_AZ_DC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 9, "Altitude North", "C_AL_NC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 9, "Altitude East", "C_AL_EC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 9, "Altitude Down", "C_AL_DC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 3, "Total Number of Tiles in Column Direction",
                                                      "TOTAL_TILES_COLS",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 5, "Total Number of Tiles in Row Direction",
                                                      "TOTAL_TILES_ROWS",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(MENSRB, description)

NITF_CXX_ENDGUARD
