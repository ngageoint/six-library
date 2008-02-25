#include <nitf/IOHandle.h>
#include <nitf/TRE.h>
#include <nitf/Record.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BINARY, 4, "First Image Frame Offset", "FRAME_1_OFFSET",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BINARY, 4, "Number of Blocks", "NUMBER_OF_FRAMES",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_LOOP, 0, "- 1", "NUMBER_OF_FRAMES"},
    {NITF_BINARY, 4, "Number of Blocks", "FRAME_OFFSET",
     NITF_VAL_BCS_A_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(NBLOCA, description)

NITF_CXX_ENDGUARD
