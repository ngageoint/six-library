/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_J2K_H

#include <import/nitf.h>
#include <import/j2k.h>


NITFPRIV(nitf_DecompressionControl*) implOpen(nitf_IOInterface*  io,
                                              nitf_Uint64        offset,
                                              nitf_Uint64        fileLength,
                                              nitf_BlockingInfo* blockInfo,
                                              nitf_Uint64*       blockMask,
                                              nitf_Error*        error);

NITFPRIV(nitf_Uint8*) implReadBlock(nitf_DecompressionControl *control,
                                    nitf_Uint32 blockNumber,
                                    nitf_Error* error);
NITFPRIV(int) implFreeBlock(nitf_DecompressionControl* control,
                            nitf_Uint8* block,
                            nitf_Error* error);

NITFPRIV(void) implClose(nitf_DecompressionControl** control);

NITFPRIV(void) implMemFree(void* p);


NITF_CXX_GUARD

static char *ident[] =
{
    "DECOMPRESSION", "C8", NULL
};

static nitf_DecompressionInterface interfaceTable =
{
    implOpen, implReadBlock, implFreeBlock, implClose, NULL
};

NITF_CXX_ENDGUARD


NITFAPI(char**) J2KDecompress_init(nitf_Error *error)
{
    return ident;
}

NITFAPI(void) C8_cleanup(void)
{
    /* TODO */
}


NITFPRIV(int) implFreeBlock(nitf_DecompressionControl* control,
                            nitf_Uint8* block,
                            nitf_Error* error)
{
    if (block)
        implMemFree((void*)block);
    return 1;
}

NITFAPI(void*) C8_construct(char *compressionType,
                            nitf_Error* error)
{
    if (strcmp(compressionType, "C8") != 0)
    {
        nitf_Error_init(error,
                        "Unsupported compression type",
                        NITF_CTXT,
                        NITF_ERR_DECOMPRESSION);

        return NULL;
    }
    return((void *) &interfaceTable);
}

NITFPRIV(nitf_Uint8*) implReadBlock(nitf_DecompressionControl *control,
                                    nitf_Uint32 blockNumber,
                                    nitf_Error* error)
{
    /* TODO */
    return NULL;
}

NITFPRIV(void*) implMemAlloc(size_t size, nitf_Error* error)
{
    void * p = NITF_MALLOC(size);
    memset(p, 0, size);
    if (!p)
    {
        nitf_Error_init(error,
                        NITF_STRERROR( NITF_ERRNO ),
                        NITF_CTXT,
                        NITF_ERR_MEMORY);
    }
    return p;
}

NITFPRIV(void) implMemFree(void* p)
{
    if (p) NITF_FREE(p);
}



NITFPRIV(void) implClose(nitf_DecompressionControl** control)
{
    /* TODO */
}

NITFPRIV(nitf_DecompressionControl*) implOpen(nitf_IOInterface* io,
                                              nitf_Uint64        offset,
                                              nitf_Uint64        fileLength,
                                              nitf_BlockingInfo* blockInfo,
                                              nitf_Uint64*       blockMask,
                                              nitf_Error*        error)
{
    /* TODO */
}

#endif
