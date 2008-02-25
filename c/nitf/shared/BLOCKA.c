
#include <nitf/IOHandle.h>
#include <nitf/TRE.h>
#include <nitf/Record.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_N, 2, "Block Number", "BLOCK_INSTANCE",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 5, "No. of Gray Pixels", "N_GRAY",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 5, "Lines", "L_LINES",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 3, "Layover Angle", "LAYOVER_ANGLE",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 3, "Shadow Angle", "SHADOW_ANGLE",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 16, "reserved 1", "RESERVED-001",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 21, "FRLC Location", "FRLC_LOC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 21, "LRLC Location", "LRLC_LOC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 21, "LRFC Location", "LRFC_LOC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 21, "FRFC Location", "FRFC_LOC",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 5, "reserved 2", "RESERVED-002",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(BLOCKA, description)

NITF_CXX_ENDGUARD
