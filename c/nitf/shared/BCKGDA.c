#include <nitf/IOHandle.h>
#include <nitf/TRE.h>
#include <nitf/Record.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_N, 8, "Background Width", "BGWIDTH",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 8, "Background Height", "BGHEIGHT",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 8, "Background Red", "BGRED",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 8, "Background Green", "BGGREEN",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 8, "Background Blue", "BGBLUE",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 8, "Pixel Size", "PIXSIZE",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 8, "Pixel Units", "PIXUNITS",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(BCKGDA, description)

NITF_CXX_ENDGUARD
