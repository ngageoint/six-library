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

typedef struct _ImplControl
{
    nitf_BlockingInfo blockInfo; /* Kept for convience */
    j2k_Container *container;    /* j2k Container */
    nitf_Uint64 offset;          /* File offset to data */
    nitf_Uint64 fileLength;      /* Length of compressed data in file */
}
ImplControl;

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
    ImplControl *implControl = (ImplControl*)control;

    if (j2k_Container_canReadTiles(implControl->container, error))
    {
        /* TODO use j2k_Container_readTile */
        nrt_Uint8 *buf = NULL;
        nitf_Uint32 tileX, tileY;

        tileY = blockNumber / implControl->blockInfo.numBlocksPerRow;
        tileX = blockNumber % implControl->blockInfo.numBlocksPerRow;

        if (!j2k_Container_readTile(implControl->container, tileX, tileY, &buf,
                                   error))
        {
            implMemFree(buf);
            return NULL;
        }
        return buf;
    }
    else
    {
        /* TODO use j2k_Container_readRegion using the block info */
    }

    return NULL;
}

NITFPRIV(void*) implMemAlloc(size_t size, nitf_Error* error)
{
    void * p = NITF_MALLOC(size);
    if (!p)
    {
        nitf_Error_init(error,
                        NITF_STRERROR( NITF_ERRNO ),
                        NITF_CTXT,
                        NITF_ERR_MEMORY);
    }
    else
    {
        memset(p, 0, size);
    }
    return p;
}

NITFPRIV(void) implMemFree(void* p)
{
    if (p) NITF_FREE(p);
}

NITFPRIV(void) implClose(nitf_DecompressionControl** control)
{
    ImplControl **implControl = (ImplControl**)control;

    if (implControl && *implControl)
    {
        if ((*implControl)->container)
            j2k_Container_destruct(&(*implControl)->container);
        implMemFree(*implControl);
    }
    *implControl = NULL;
}

NITFPRIV(nitf_DecompressionControl*) implOpen(nitf_IOInterface* io,
                                              nitf_Uint64        offset,
                                              nitf_Uint64        fileLength,
                                              nitf_BlockingInfo* blockInfo,
                                              nitf_Uint64*       blockMask,
                                              nitf_Error*        error)
{
    ImplControl *implControl = NULL;

    if (!(implControl = (ImplControl*)implMemAlloc(sizeof(ImplControl), error)))
        goto CATCH_ERROR;

    if (nitf_IOInterface_seek(io, offset, NITF_SEEK_SET, error) < 0)
        goto CATCH_ERROR;

    if (!(implControl->container = j2k_Container_openIO(io, error)))
        goto CATCH_ERROR;

    implControl->offset     = offset;
    implControl->fileLength = fileLength;
    implControl->blockInfo  = *blockInfo;
    return((nitf_DecompressionControl*) implControl);

    CATCH_ERROR:
    {
        implMemFree(implControl);
        return NULL;
    }

}

#endif
