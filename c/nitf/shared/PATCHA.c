#include <nitf/TRE.h>
#include <nitf/Record.h>

NITF_CXX_GUARD


static nitf_TREDescription descrip_00074[] = {
    {NITF_BCS_A, 4, "Patch Number", "PAT_NO",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 1, "Last Patch of Search Scene", "LAST_PAT_FLAG",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 7, "Patch Start Line", "LNSTRT",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 7, "Patch End Line", "LNSTOP",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 5, "Number of Azimuth Lines", "AZL",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 5, "Number of Valid Azimuth Lines", "NVL",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 3, "First Valid Line", "FVL",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 5, "Number of Pixels per Line", "NPIXEL",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 5, "First Valid Pixel Index", "FVPIX",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 3, "Spot Frame Number", "FRAME",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 8, "GMT", "GMT",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 7, "Scene Heading", "SHEAD",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 6, "Ground Sweep Angle", "GSWEEP",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 8, "Patch Shear Factor", "SHEAR",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

static nitf_TREDescription descrip_00115[] = {
    {NITF_BCS_A, 4, "Patch Number", "PAT_NO",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 1, "Last Patch of Search Scene", "LAST_PAT_FLAG",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 7, "Patch Start Line", "LNSTRT",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 7, "Patch End Line", "LNSTOP",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 5, "Number of Azimuth Lines", "AZL",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 5, "Number of Valid Azimuth Lines", "NVL",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 3, "First Valid Line", "FVL",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 5, "Number of Pixels per Line", "NPIXEL",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 5, "First Valid Pixel Index", "FVPIX",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 3, "Spot Frame Number", "FRAME",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 8, "UTC", "UTC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 7, "Scene Heading", "SHEAD",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 7, "Local Gravity", "GRAVITY",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 5, "Ins Platform Velocity, North", "INS_V_NC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 5, "Ins Platform Velocity, East", "INS_V_EC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 5, "Ins Platform Velocity, Down", "INS_V_DC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 8, "Geodetic Latitude Offset", "OFFLAT",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 8, "Geodetic Longitude Offset", "OFFLONG",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 3, "Track Heading", "TRACK",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 6, "Ground Sweep Angle", "GSWEEP",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 8, "Patch Shear Factor", "SHEAR",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

/* Define the available descriptions and the default one */
static nitf_TREDescriptionInfo descriptions[] = {
    { "PATCHA_115", descrip_00115, 115 },
    { "PATCHA_74", descrip_00074, 74 },
    { NULL, NULL, NITF_TRE_DESC_NO_LENGTH }
};

static nitf_TREDescriptionSet descriptionSet = { 0, descriptions };

NITF_DECLARE_PLUGIN(PATCHA)

NITF_CXX_ENDGUARD
