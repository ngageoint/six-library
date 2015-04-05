#ifndef __ZIP_TYPES_H__
#define __ZIP_TYPES_H__

#include "io/InputStream.h"
#include "io/OutputStream.h"
#include <import/sys.h>
#include <zlib.h>
//#define DEF_MEM_LEVEL 8

namespace zip
{
enum
{
    CD_SIGNATURE = 0x06054b50,
    EOCD_LEN = 22,
    MAX_COMMENT_LEN = 65535,
    MAX_EOCD_SEARCH = MAX_COMMENT_LEN + EOCD_LEN,
    ENTRY_SIGNATURE = 0x02014b50,
    ENTRY_LEN = 46,
    LFH_SIZE = 30
};
}

#endif
