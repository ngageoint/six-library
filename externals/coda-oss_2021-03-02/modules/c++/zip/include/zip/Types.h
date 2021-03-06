/* =========================================================================
 * This file is part of zip-c++
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * zip-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this program; If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

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

