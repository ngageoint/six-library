#include <nitf/TRE.h>
#include <nitf/Record.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_A, 15, "Target UTM", "TGTUTM",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 15, "Target ID", "PIATGAID",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 2, "Country Code", "PIACTRY",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 5, "Category Code", "PIACAT",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 15, "Target Geographic Coordinates", "TGTGEO",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 3, "Target Coordinate Datum", "DATUM",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 38, "Target Name", "TGTNAME",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 3, "Percent Coverage", "PERCOVER",
     NITF_VAL_BCS_N_PLUS, NITF_U32_RANGE, "0", "100" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(PIATGA, description)

NITF_CXX_ENDGUARD
