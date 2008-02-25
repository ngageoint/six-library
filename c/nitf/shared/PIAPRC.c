
#include <nitf/IOHandle.h>
#include <nitf/TRE.h>
#include <nitf/Record.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_A, 64, "Access ID", "accid",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 32, "FM Control Number", "fmctl",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 1, "Subjective Detail", "sdet",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 2, "Product Code", "pcode",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 6, "Producer Subelement", "psube",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 20, "Product ID Number", "pidnm",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 10, "Product Short Name", "pname",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 2, "Producer Code", "maker",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 14, "Product Create Time", "ctime",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_A, 40, "Map ID", "mapid",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 2, "SECTITLE Repetitions", "strep",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_LOOP, 0, NULL, "strep"},
    {NITF_BCS_A, 48, "SECTITLE", "SECTITLE",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_N, 2, "REQORG Repetitions", "rorep",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_LOOP, 0, NULL, "rorep"},
    {NITF_BCS_A, 64, "Requesting Organization", "REQORG",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_N, 2, "KEYWORD Repetitions", "kwrep",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_LOOP, 0, NULL, "kwrep"},
    {NITF_BCS_A, 255, "KEYWORD", "KEYWORD",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_N, 2, "ASSRPT Repetitions", "arrep",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_LOOP, 0, NULL, "arrep"},
    {NITF_BCS_A, 20, "ASSRPT", "ASSRPT",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_N, 2, "ATEXT Repetitions", "atrep",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_LOOP, 0, NULL, "atrep"},
    {NITF_BCS_A, 255, "ATEXT", "ATEXT",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(PIAPRC, description)

NITF_CXX_ENDGUARD
