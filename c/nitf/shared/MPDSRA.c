
#include <nitf/IOHandle.h>
#include <nitf/TRE.h>
#include <nitf/Record.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_A, 2, "Image Block Number", "blkno",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 2, "Commanded IPR", "cdipr",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 2, "No of Blocks in WDG", "nblkw",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 5, "No of Rows in Block", "nrblk",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 5, "No of Cols in Block", "ncblk",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 9, "Output Ref Pt X (ECF)", "orpx",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 9, "Output Ref Pt Y (ECF)", "orpy",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 9, "Output Ref Pt Z (ECF)", "orpz",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 5, "Row of ORP", "orpro",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 5, "Col of ORP", "orpco",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 7, "FP Normal Vector X", "fpnvx",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 7, "FP Normal Vector Y", "fpnvy",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 7, "FP Normal Vector Z", "fpnvz",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 9, "Collection Start Time", "arptm",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 14, "reserved 1", "resv1",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 9, "Aircraft Position N", "arppn",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 9, "Aircraft Position E", "arppe",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 9, "Aircraft Position D", "arppd",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 9, "Aircraft Velocity N", "arpvn",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 9, "Aircraft Velocity E", "arpve",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 9, "Aircraft Velocity D", "arpvd",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 8, "Aircraft Accel N", "arpan",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 8, "Aircraft Accel E", "arpae",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 8, "Aircraft Accel D", "arpad",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 13, "reserved 2", "resv2",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(MPDSRA, description)

NITF_CXX_ENDGUARD
