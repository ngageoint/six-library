/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
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

#include <import/nitf.h>
#include <openjpeg.h>

/*!
 *  \file JasPerDecompress.c - Decompression plugin for jpeg2000
 *  using the jasper library,
 *
 *  \brief The jasper decompression plugin follows the decompression
 *  plugin
 *
 *  specification given in nitf/PluginRegistry.h and nitf/ImageIO.h
 *
 *  Compression type supported:
 *
 *   C8 - Jpeg 2000
 *
 *  This is the simplest version.It decompresses the entire image into a
 *  memory buffer the first time the image data is accessed. It does not
 *  present the compression tiling structure as the block organization and
 *  reports the image as one big block.
 */

/*  Prototypes  */
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
/*
 *  This is the identifier for the plugin.  The first
 *  value must be either "COMPRESSION," "DECOMPRESSION," or "TRE"
 *  to signify which area the plugin fills
 */
static char *ident[] =
    {
        "DECOMPRESSION",  /*  This is a decompression handler  */
        "C8",             /*  JPEG2000 is C8 in the NITF spec  */
        NULL              /*  Need to NULL terminate this      */
    };



/*  Function prototypes  */
static nitf_DecompressionInterface interfaceTable =
    {
        implOpen,
        implReadBlock,
        implFreeBlock,
        implClose,
        NULL
    };

typedef struct _ImplControl
{
    nitf_BlockingInfo blockInfo; /* Kept for convience */
    nitf_Uint8 *input;           /* The block is kept here after decoding */
    nitf_Uint64 offset;          /* File offset to data */
    nitf_Uint64 fileLength;      /* Length of compressed data in file */
    nitf_Uint32 nCols;           /* Number of columns in image */
    nitf_Uint32 nBytes;          /* Number of bytes/pixel (expanded) */
    nitf_Uint32 nBands;          /* Number of bands/components */
    nitf_Uint64 imageSkip;       /* Byte skip between image bands in input */
}
ImplControl;

NITF_CXX_ENDGUARD


/*!
 *  This function is required by the plugin manager.
 *  It is called at plugin manager load time.  In our
 *  case, we call the initialization function for jasper
 *  \param error An error to populate on failure.
 *  \return The identifier structure
 *
 */
NITFAPI(char**) OpenJPEGDecompress_init(nitf_Error *error)
{
    /* TODO */
    return ident;
}

/*!
 *  This function is required by the plugin manager, and is
 *  called at unload time.
 */
NITFAPI(void) C8_cleanup(void)
{
    /* TODO */
}

/*!
 *  Interface free block function
 *
 *  Since the block is maintained by the contol structure until it is freed,
 *  this function does nothing
 */

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
    if (p)
        NITF_FREE(p);
}

NITFPRIV(void) implClose(nitf_DecompressionControl** control)
{
    ImplControl *implControl = NULL;
    implControl = (ImplControl *) * control;
    if (implControl->input != NULL)
    {
        implMemFree((void*)implControl->input);
        implControl->input = NULL;
    }
    implMemFree((void *)(*control));
    *control = NULL;
}

NITFPRIV(nitf_DecompressionControl*) implOpen(nitf_IOInterface* io,
                                              nitf_Uint64        offset,
                                              nitf_Uint64        fileLength,
                                              nitf_BlockingInfo* blockInfo,
                                              nitf_Uint64*       blockMask,
                                              nitf_Error*        error)
{
    int check;
    ImplControl *implControl = NULL;
    nitf_Uint64 outputLen;
    nitf_Off found;
    nitf_Uint64 expected =
        (nitf_Uint64)(implControl->blockInfo.length) *
        (nitf_Uint64)(implControl->blockInfo.numBlocksPerRow) *
        (nitf_Uint64)(implControl->blockInfo.numBlocksPerCol);

    /*      Allocate the result */
    implControl = (ImplControl *) NITF_MALLOC(sizeof(ImplControl));
    if (implControl == NULL)
    {
        nitf_Error_init(error,
                        "Error creating control object",
                        NITF_CTXT,
                        NITF_ERR_DECOMPRESSION);
        return NULL;
    }

    implControl->input = NULL;

    /*      Seek the handle to the start of data */
    found = nitf_IOInterface_seek(io, offset, NITF_SEEK_SET, error);
    if (!NITF_IO_SUCCESS(found))
    {
        implClose((nitf_DecompressionControl**)&implControl);
        return NULL;
    }

    /*  Allocate read buffer */
    implControl->input = implMemAlloc(fileLength, error);
    if (!implControl->input)
    {
        implClose((nitf_DecompressionControl**)&implControl);
        return NULL;
    }

    /*  Read compressed data */
    check = nitf_IOInterface_read(io, (char*)implControl->input, fileLength,
                                  error);

    if (!NITF_IO_SUCCESS(check))
    {
        implClose((nitf_DecompressionControl**)&implControl);
        return 0;
    }

    implControl->offset     = offset;
    implControl->fileLength = fileLength;
    implControl->blockInfo  = *blockInfo;
    return((nitf_DecompressionControl*) implControl);
}
