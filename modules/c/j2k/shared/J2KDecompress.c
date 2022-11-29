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

#if _MSC_VER
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#pragma warning(push)
#pragma warning(disable: 5039) // '...': pointer or reference to potentially throwing function passed to '...' 
#include <windows.h>
#pragma warning(pop)
#undef min
#undef max

#pragma warning(disable: 5045) // Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
#endif // _MSC_VER

#include <import/nitf.h>
#include <import/j2k.h>

NITF_CXX_GUARD

NITFPRIV(nitf_DecompressionControl*) implOpen(nitf_ImageSubheader * subheader,
                                              nrt_HashTable * options, 
                                              nitf_Error * error);
NITFPRIV(NITF_BOOL) implStart(nitf_DecompressionControl* control,
                              nitf_IOInterface*  io,
                              uint64_t        offset,
                              uint64_t        fileLength,
                              nitf_BlockingInfo* blockInfo,
                              uint64_t*       blockMask,
                              nitf_Error*        error);
NITFPRIV(uint8_t*) implReadBlock(nitf_DecompressionControl *control,
                                    uint32_t blockNumber,
                                    uint64_t* blockSize,
                                    nitf_Error* error);
NITFPRIV(NITF_BOOL) implFreeBlock(nitf_DecompressionControl* control,
                            uint8_t* block,
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
    uint64_t offset;          /* File offset to data */
    uint64_t fileLength;      /* Length of compressed data in file */
}
ImplControl;

NITF_CXX_ENDGUARD


NITFAPI(const char**) J2KDecompress_init(nitf_Error *error)
{
    (void)error;
    return ident;
}

NITFAPI(void) C8_cleanup(void)
{
    /* TODO */
}

NITFPRIV(NITF_BOOL) implFreeBlock(nitf_DecompressionControl* control,
                            uint8_t* block,
                            nitf_Error* error)
{
    (void)control;
    (void)error;

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

NITFPRIV(uint8_t*) implReadBlock(nitf_DecompressionControl *control,
                                    uint32_t blockNumber,
                                    uint64_t* blockSize,
                                    nitf_Error* error)
{
    ImplControl *implControl = (ImplControl*)control;
    uint8_t *buf = NULL;
    uint64_t bufSize;
    j2k_Container* container = NULL;

    if (j2k_Reader_canReadTiles(implControl->reader, error))
    {
        /* use j2k_Reader_readTile */
        uint32_t tileX, tileY;

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
        uint32_t tileX, tileY, x0, x1, y0, y1, totalRows, totalCols;

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
    (void)subheader;
    (void)options;

    ImplControl* implControl = (ImplControl*)implMemAlloc(sizeof(ImplControl), error);
    if (!implControl)
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
                              uint64_t        offset_,
                              uint64_t        fileLength,
                              nitf_BlockingInfo* blockInfo,
                              uint64_t*       blockMask,
                              nitf_Error*        error)
{
    /* TODO: In order to support M8, I think we would update this */
    (void)blockMask;

    ImplControl* implControl = (ImplControl*)control;

    const nrt_Off offset = (nrt_Off)offset_;
    if (nitf_IOInterface_seek(io, offset, NITF_SEEK_SET, error) < 0)
        goto CATCH_ERROR;

    implControl->reader = j2k_Reader_openIO(io, error);
    if (!implControl->reader)
        goto CATCH_ERROR;

    implControl->offset     = (uint64_t)offset;
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
