/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

NITF_CXX_GUARD

NITFPRIV(nitf_DecompressionControl*) implOpen(nitf_ImageSubheader * subheader,
                                              nrt_HashTable * options, 
                                              nitf_Error * error);
NITFPRIV(NITF_BOOL) implStart(nitf_DecompressionControl* control,
                              nitf_IOInterface*  io,
                              nitf_Uint64        offset,
                              nitf_Uint64        fileLength,
                              nitf_BlockingInfo* blockInfo,
                              nitf_Uint64*       blockMask,
                              nitf_Error*        error);
NITFPRIV(nitf_Uint8*) implReadBlock(nitf_DecompressionControl *control,
                                    nitf_Uint32 blockNumber,
                                    nitf_Uint64* blockSize,
                                    nitf_Error* error);
NITFPRIV(int) implFreeBlock(nitf_DecompressionControl* control,
                            nitf_Uint8* block,
                            nitf_Error* error);

NITFPRIV(void) implClose(nitf_DecompressionControl** control);

NITFPRIV(void) implMemFree(void* p);

static const char *ident[] =
{
    NITF_PLUGIN_DECOMPRESSION_KEY, "C8", NULL
};

static nitf_DecompressionInterface interfaceTable =
{
    implOpen, implStart, implReadBlock, implFreeBlock, implClose, NULL
};

typedef struct _ImplControl
{
    nitf_BlockingInfo blockInfo; /* Kept for convenience */
    j2k_Reader *reader;          /* j2k Reader */
    nitf_Uint64 offset;          /* File offset to data */
    nitf_Uint64 fileLength;      /* Length of compressed data in file */
}
ImplControl;

NITF_CXX_ENDGUARD


NITFAPI(const char**) J2KDecompress_init(nitf_Error *error)
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
                                    nitf_Uint64* blockSize,
                                    nitf_Error* error)
{
    ImplControl *implControl = (ImplControl*)control;
    nrt_Uint8 *buf = NULL;
    nrt_Uint64 bufSize;
    j2k_Container* container = NULL;

    if (j2k_Reader_canReadTiles(implControl->reader, error))
    {
        /* use j2k_Reader_readTile */
        nitf_Uint32 tileX, tileY;

        tileY = blockNumber / implControl->blockInfo.numBlocksPerRow;
        tileX = blockNumber % implControl->blockInfo.numBlocksPerRow;

        if (0 == (bufSize = j2k_Reader_readTile(implControl->reader, tileX,
                                                tileY, &buf, error)))
        {
            implMemFree(buf);
            return NULL;
        }
    }
    else
    {
        /* use j2k_Reader_readRegion using the block info */
        nitf_Uint32 tileX, tileY, x0, x1, y0, y1, totalRows, totalCols;

        tileY = blockNumber / implControl->blockInfo.numBlocksPerRow;
        tileX = blockNumber % implControl->blockInfo.numBlocksPerRow;

        x0 = tileX * implControl->blockInfo.numColsPerBlock;
        x1 = x0 + implControl->blockInfo.numColsPerBlock;
        y0 = tileY * implControl->blockInfo.numRowsPerBlock;
        y1 = y0 + implControl->blockInfo.numRowsPerBlock;

        /* check for last tiles */
        container = j2k_Reader_getContainer(implControl->reader, error);
        if (container == NULL)
        {
            return NULL;
        }
        totalRows = j2k_Container_getHeight(container, error);
        totalCols = j2k_Container_getWidth(container, error);

        if (x1 > totalCols)
            x1 = totalCols;
        if (y1 > totalRows)
            y1 = totalRows;

        if (0 == (bufSize = j2k_Reader_readRegion(implControl->reader, x0, y0,
                                                  x1, y1, &buf, error)))
        {
            implMemFree(buf);
            return NULL;
        }
    }
    *blockSize = bufSize;
    return buf;
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
    if (control && *control)
    {
        ImplControl *implControl = (ImplControl*)*control;
        if (implControl->reader)
        {
            j2k_Reader_destruct(&implControl->reader);
        }
        implMemFree(implControl);
        *control = NULL;
    }
}

NITFPRIV(nitf_DecompressionControl*) implOpen(nitf_ImageSubheader * subheader,
                                              nrt_HashTable * options, 
                                              nitf_Error * error)
{
    ImplControl *implControl = NULL;
    (void)subheader;
    (void)options;

    if (!(implControl = (ImplControl*)implMemAlloc(sizeof(ImplControl), error)))
        goto CATCH_ERROR;

    return((nitf_DecompressionControl*) implControl);

    CATCH_ERROR:
    {
        implMemFree(implControl);
        return NULL;
    }
}

NITFPRIV(NITF_BOOL) implStart(nitf_DecompressionControl* control,
                              nitf_IOInterface*  io,
                              nitf_Uint64        offset,
                              nitf_Uint64        fileLength,
                              nitf_BlockingInfo* blockInfo,
                              nitf_Uint64*       blockMask,
                              nitf_Error*        error)
{
    ImplControl *implControl = NULL;

    /* TODO: In order to support M8, I think we would update this */
    (void)blockMask;

    implControl = (ImplControl*)control;

    if (nitf_IOInterface_seek(io, offset, NITF_SEEK_SET, error) < 0)
        goto CATCH_ERROR;

    if (!(implControl->reader = j2k_Reader_openIO(io, error)))
        goto CATCH_ERROR;

    implControl->offset     = offset;
    implControl->fileLength = fileLength;
    implControl->blockInfo  = *blockInfo;
    return NITF_SUCCESS;

    CATCH_ERROR:
    {
        implMemFree(implControl);
        return NITF_FAILURE;
    }
}

#endif
