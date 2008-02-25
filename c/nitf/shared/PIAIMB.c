
#include <nitf/IOHandle.h>
#include <nitf/TRE.h>
#include <nitf/Record.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_A, 3, "Cloud Cover", "cloud",
     "(0\\d{2})|100|999", NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 1, "Standard Radiometric?", "stdrd",
     "[YN]", NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 12, "Sensor Mode", "smode",
     "WHISKBROOM|PUSHBROOM|FRAMING|SPOT|SWATH|TBD", NITF_NO_RANGE, NULL,
     NULL},
    {NITF_BCS_A, 18, "Sensor Name", "sname",
     NITF_VAL_BCS_A "{18}", NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 255, "Source", "srce",
     NITF_VAL_BCS_A "{255}", NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 2, "Compression Generation", "cmgen",
     "\\d{2}", NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 1, "Subjective Quality", "squal",
     "[PGEF]", NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 7, "PIA Mission Number", "misnm",
     NITF_VAL_BCS_A "{7}", NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 32, "Camera Specs", "cspec",
     NITF_VAL_BCS_A "{32}", NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 2, "Project ID Code", "pjtid",
     NITF_VAL_BCS_A "{2}", NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 1, "Generation", "gener",
     NITF_VAL_BCS_A "{1}", NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 1, "Exploitation Support", "expls",
     "[YN]", NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 2, "Other Conditions", "othrc",
     NITF_VAL_BCS_A "{2}", NITF_NO_RANGE, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(PIAIMB, description)

NITF_CXX_ENDGUARD
