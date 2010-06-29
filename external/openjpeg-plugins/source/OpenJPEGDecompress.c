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

NITF_CXX_GUARD

/*  Prototypes  */
NITFPRIV(nitf_DecompressionControl*) implOpen(nitf_IOInterface* io,
        nitf_Uint64 offset,
        nitf_Uint64 fileLength,
        nitf_BlockingInfo* blockInfo,
        nitf_Uint64* blockMask,
        nitf_Error* error);

NITFPRIV(nitf_Uint8*) implReadBlock(nitf_DecompressionControl *control,
        nitf_Uint32 blockNumber,
        nitf_Error* error);
NITFPRIV(int) implFreeBlock(nitf_DecompressionControl* control,
        nitf_Uint8* block,
        nitf_Error* error);

NITFPRIV(void) implClose(nitf_DecompressionControl** control);

NITFPRIV(void) implMemFree(void* p);

NITFPRIV(OPJ_UINT32) implStreamRead(void* buf, OPJ_UINT32 bytes, void *data);
NITFPRIV(bool) implStreamSeek(OPJ_SIZE_T bytes, void *data);
NITFPRIV(OPJ_SIZE_T) implStreamSkip(OPJ_SIZE_T bytes, void *data);
NITFPRIV(void) implErrorCallback(const char *msg, void *data);
NITFPRIV(void) implWarningCallback(const char *msg, void *data);
NITFPRIV(void) implInfoCallback(const char *msg, void *data);

static char *ident[] = { "DECOMPRESSION", /*  This is a decompression handler  */
"C8", /*  JPEG2000 is C8 in the NITF spec  */
NULL /*  Need to NULL terminate      */
};

/*  Function prototypes  */
static nitf_DecompressionInterface interfaceTable = { implOpen, implReadBlock,
                                                      implFreeBlock, implClose,
                                                      NULL };

typedef struct _ImplControl
{
    nitf_BlockingInfo blockInfo; /* Kept for convenience */
    nitf_IOInterface *io;
    opj_stream_t *stream;
    nitf_Uint64 offset; /* File offset to data */
    nitf_Uint64 fileLength; /* Length of compressed data in file */
    nitf_Uint32 nCols; /* Number of columns in image */
    nitf_Uint32 nBytes; /* Number of bytes/pixel (expanded) */
    nitf_Uint64 imageSkip; /* Byte skip between image bands in input */
    nitf_Error error;
} ImplControl;

NITF_CXX_ENDGUARD

NITFAPI(char**) OpenJPEGDecompress_init(nitf_Error *error)
{
    return ident;
}

NITFAPI(void) C8_cleanup(void)
{
}

NITFPRIV(int) implFreeBlock(nitf_DecompressionControl* control,
        nitf_Uint8* block,
        nitf_Error* error)
{
    if (block)
    {
        implMemFree((void*)block);
    }
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
    if (p)
    {
        NITF_FREE(p);
    }
}

NITFPRIV(nitf_Uint8*) implReadBlock(nitf_DecompressionControl *control,
        nitf_Uint32 blockNumber,
        nitf_Error* error)
{

    nitf_Uint32 blockRow, blockCol;
    nitf_Uint32 blockWidth, blockHeight;

    ImplControl* implControl = NULL;
    opj_codec_t* codec = NULL;
    opj_image_t* image = NULL;
    opj_dparameters_t parameters;
    nitf_Uint8* buf = NULL;

    implControl = (ImplControl*)control;

    buf = (nitf_Uint8*)implMemAlloc(implControl->blockInfo.length, error);
    if(!buf)
    {
        return NULL;
    }
    blockRow = blockNumber / implControl->blockInfo.numBlocksPerRow;
    blockCol = blockNumber % implControl->blockInfo.numBlocksPerRow;
    blockWidth = implControl->blockInfo.numColsPerBlock;
    blockHeight = implControl->blockInfo.numRowsPerBlock;

    /*printf("Block number: %d\n", blockNumber);*/

    /* create the decoder */
    codec = opj_create_decompress(CODEC_J2K);

    /* add some message callback handlers */
    opj_set_error_handler(codec, (opj_msg_callback)implErrorCallback, NULL);
    opj_set_warning_handler(codec, (opj_msg_callback)implWarningCallback, NULL);
    opj_set_info_handler(codec, (opj_msg_callback)implInfoCallback, NULL);

    opj_set_default_decoder_parameters(&parameters);

    /* setup the decoder */
    if (!opj_setup_decoder(codec, &parameters))
    {
        opj_destroy_codec(codec);
        implMemFree((void *)buf);
        nitf_Error_init(error,
                "Error setting up openjpeg decoder",
                NITF_CTXT,
                NITF_ERR_DECOMPRESSION);
        return NULL;
    }

    {
        OPJ_INT32 x0, y0;
        OPJ_UINT32 tileWidth, tileHeight, tilesX, tilesY;
        bool keepGoing = 1;

        /* TODO reset the stream to 0 each time?... */
        nitf_IOInterface_seek(implControl->io, implControl->offset, NITF_SEEK_SET, error);

        /* TODO do we really read the header every time ? */
        if(!opj_read_header(codec, &image, &x0, &y0, &tileWidth, &tileHeight,
                        &tilesX, &tilesY, implControl->stream))
        {
            opj_destroy_codec(codec);
            implMemFree((void *)buf);
            nitf_Error_init(error,
                    "Error reading header",
                    NITF_CTXT,
                    NITF_ERR_DECOMPRESSION);
            return NULL;
        }

        /* only decode what we want */
        if (!opj_set_decode_area(codec, blockCol * blockWidth,
                        blockRow * blockHeight,
                        (blockCol + 1) * blockWidth,
                        (blockRow + 1) * blockHeight))
        {
            opj_destroy_codec(codec);
            opj_image_destroy(image);
            implMemFree((void *)buf);
            nitf_Error_init(error,
                    "Error decoding area",
                    NITF_CTXT,
                    NITF_ERR_DECOMPRESSION);
            return NULL;
        }

        /*printf("decoding area: %d, %d  %d, %d\n", blockCol * blockWidth,
                        blockRow * blockHeight,
                        (blockCol + 1) * blockWidth,
                        (blockRow + 1) * blockHeight);*/

        while(keepGoing)
        {
            OPJ_UINT32 tileIndex, nComponents, reqSize;
            OPJ_INT32 tileX0, tileY0, tileX1, tileY1;

            if (!opj_read_tile_header(codec, &tileIndex, &reqSize,
                            &tileX0, &tileY0, &tileX1, &tileY1,
                            &nComponents, &keepGoing, implControl->stream))
            {
                opj_destroy_codec(codec);
                opj_image_destroy(image);
                implMemFree((void *)buf);
                nitf_Error_init(error,
                        "Error reading tile header",
                        NITF_CTXT,
                        NITF_ERR_DECOMPRESSION);
                return NULL;
            }

            if (keepGoing)
            {
                if (!opj_decode_tile_data(codec, tileIndex, buf,
                                reqSize, implControl->stream))
                {
                    opj_destroy_codec(codec);
                    opj_image_destroy(image);
                    implMemFree((void *)buf);
                    nitf_Error_init(error,
                            "Error decoding tile",
                            NITF_CTXT,
                            NITF_ERR_DECOMPRESSION);
                    return NULL;
                }
            }
        }
    }

    opj_end_decompress(codec, implControl->stream);
    opj_destroy_codec(codec);
    opj_image_destroy(image);

    return buf;
}

NITFPRIV(void) implClose(nitf_DecompressionControl** control)
{
    ImplControl *implControl = NULL;
    implControl = (ImplControl *) * control;
    if (implControl->stream)
    {
        opj_stream_destroy(implControl->stream);
        implControl->stream = NULL;
    }
    implMemFree((void *)(*control));
    *control = NULL;
}

NITFPRIV(nitf_DecompressionControl*) implOpen(nitf_IOInterface* io,
        nitf_Uint64 offset,
        nitf_Uint64 fileLength,
        nitf_BlockingInfo* blockInfo,
        nitf_Uint64* blockMask,
        nitf_Error* error)
{
    ImplControl *implControl = NULL;
    nitf_Uint64 outputLen;
    nitf_Off found;

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

    implControl->io = io;
    implControl->stream = NULL;

    /*      Seek the handle to the start of data */
    found = nitf_IOInterface_seek(io, offset, NITF_SEEK_SET, error);
    if (!NITF_IO_SUCCESS(found))
    {
        implClose((nitf_DecompressionControl**)&implControl);
        return NULL;
    }

    /* setup the stream */
    implControl->stream = opj_stream_create(1024, 1);
    if (!implControl->stream)
    {
        implClose((nitf_DecompressionControl**)&implControl);
        nitf_Error_init(error,
                "Error creating openjpeg stream",
                NITF_CTXT,
                NITF_ERR_DECOMPRESSION);
        return NULL;
    }
    opj_stream_set_user_data(implControl->stream, implControl);
    opj_stream_set_read_function(implControl->stream, (opj_stream_read_fn)implStreamRead);
    opj_stream_set_seek_function(implControl->stream, (opj_stream_seek_fn)implStreamSeek);
    opj_stream_set_skip_function(implControl->stream, (opj_stream_skip_fn)implStreamSkip);

    implControl->offset = offset;
    implControl->fileLength = fileLength;
    implControl->blockInfo = *blockInfo;
    return((nitf_DecompressionControl*) implControl);
}

NITFPRIV(OPJ_UINT32) implStreamRead(void* buf, OPJ_UINT32 bytes, void *data)
{
    ImplControl *ctrl = (ImplControl*)data;
    nitf_Uint64 offset, bytesLeft;
    OPJ_UINT32 toRead;

    offset = nitf_IOInterface_tell(ctrl->io, &ctrl->error);
    assert(offset >= ctrl->offset); /* probably not a good idea, but need it */

    bytesLeft = ctrl->fileLength - (offset - ctrl->offset);
    toRead = bytesLeft < bytes ? (OPJ_UINT32)bytesLeft : bytes;
    /*printf("left: %lld, toRead: %d\n", bytesLeft, toRead);*/
    if (toRead <= 0 || !nitf_IOInterface_read(
            ctrl->io, (char*)buf, toRead, &ctrl->error))
    {
        return -1;
    }
    return toRead;
}

NITFPRIV(bool) implStreamSeek(OPJ_SIZE_T bytes, void *data)
{
    ImplControl *ctrl = (ImplControl*)data;
    if (!nitf_IOInterface_seek(ctrl->io, ctrl->offset + bytes,
                               NITF_SEEK_SET, &ctrl->error))
    {
        return 0;
    }
    return 1;
}

NITFPRIV(OPJ_SIZE_T) implStreamSkip(OPJ_SIZE_T bytes, void *data)
{
    ImplControl *ctrl = (ImplControl*)data;
    if (!nitf_IOInterface_seek(ctrl->io, bytes, NITF_SEEK_CUR, &ctrl->error))
    {
        return -1;
    }
    return bytes;
}

NITFPRIV(void) implErrorCallback(const char *msg, void *data)
{
    fprintf(stderr, "Error: %s\n", msg);
}

NITFPRIV(void) implWarningCallback(const char *msg, void *data)
{
    fprintf(stdout, "Warning: %s\n", msg);
}

NITFPRIV(void) implInfoCallback(const char *msg, void *data)
{
    fprintf(stdout, "Info: %s\n", msg);
}
