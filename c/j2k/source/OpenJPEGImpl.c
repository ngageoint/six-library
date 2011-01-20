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

#ifdef HAVE_OPENJPEG_H

#include <import/nrt.h>
#include "j2k/Reader.h"
#include "j2k/Container.h"
#include <openjpeg.h>


/******************************************************************************/
/* TYPES & DECLARATIONS                                                       */
/******************************************************************************/
typedef struct _IOControl
{
    nrt_IOInterface *io;
    nrt_Off offset;
    nrt_Off length;
    int isRead;
    nrt_Error error;
} IOControl;


typedef struct _OpenJPEGReaderImpl
{
    opj_dparameters_t parameters;
    nrt_Off ioOffset;
    nrt_IOInterface *io;
    int ownIO;
    j2k_Container *container;
} OpenJPEGReaderImpl;

typedef struct _OpenJPEGContainerImpl
{
    nrt_Int32 x0;
    nrt_Int32 y0;
    nrt_Uint32 tileWidth;
    nrt_Uint32 tileHeight;
    nrt_Uint32 xTiles;
    nrt_Uint32 yTiles;
    nrt_Uint32 width;
    nrt_Uint32 height;
    nrt_Uint32 nComponents;
    nrt_Uint32 componentBytes;
    nrt_Uint32 inputComponentBits;
    int imageType;
    int isSigned;
} OpenJPEGContainerImpl;

NRTPRIV(OPJ_UINT32) implStreamRead(void* buf, OPJ_UINT32 bytes, void *data);
NRTPRIV(bool)       implStreamSeek(OPJ_SIZE_T bytes, void *data);
NRTPRIV(OPJ_SIZE_T) implStreamSkip(OPJ_SIZE_T bytes, void *data);



NRTPRIV( nrt_Uint32) OpenJPEGContainer_getTilesX(J2K_USER_DATA *, nrt_Error *);
NRTPRIV( nrt_Uint32) OpenJPEGContainer_getTilesY(J2K_USER_DATA *, nrt_Error *);
NRTPRIV( nrt_Uint32) OpenJPEGContainer_getTileWidth(J2K_USER_DATA *, nrt_Error *);
NRTPRIV( nrt_Uint32) OpenJPEGContainer_getTileHeight(J2K_USER_DATA *, nrt_Error *);
NRTPRIV( nrt_Uint32) OpenJPEGContainer_getWidth(J2K_USER_DATA *, nrt_Error *);
NRTPRIV( nrt_Uint32) OpenJPEGContainer_getHeight(J2K_USER_DATA *, nrt_Error *);
NRTPRIV( nrt_Uint32) OpenJPEGContainer_getNumComponents(J2K_USER_DATA *, nrt_Error *);
NRTPRIV( nrt_Uint32) OpenJPEGContainer_getComponentBytes(J2K_USER_DATA *, nrt_Error *);
NRTPRIV(void)        OpenJPEGContainer_destruct(J2K_USER_DATA *);

static j2k_IContainer ContainerInterface = { &OpenJPEGContainer_getTilesX,
                                             &OpenJPEGContainer_getTilesY,
                                             &OpenJPEGContainer_getTileWidth,
                                             &OpenJPEGContainer_getTileHeight,
                                             &OpenJPEGContainer_getWidth,
                                             &OpenJPEGContainer_getHeight,
                                             &OpenJPEGContainer_getNumComponents,
                                             &OpenJPEGContainer_getComponentBytes,
                                             &OpenJPEGContainer_destruct};

NRTPRIV( NRT_BOOL  )     OpenJPEGReader_canReadTiles(J2K_USER_DATA *,  nrt_Error *);
NRTPRIV( nrt_Uint64)     OpenJPEGReader_readTile(J2K_USER_DATA *, nrt_Uint32,
                                                 nrt_Uint32, nrt_Uint8 **,
                                                 nrt_Error *);
NRTPRIV( nrt_Uint64)     OpenJPEGReader_readRegion(J2K_USER_DATA *, nrt_Uint32,
                                                   nrt_Uint32, nrt_Uint32,
                                                   nrt_Uint32, nrt_Uint8 **,
                                                   nrt_Error *);
NRTPRIV( j2k_Container*) OpenJPEGReader_getContainer(J2K_USER_DATA *, nrt_Error *);
NRTPRIV(void)            OpenJPEGReader_destruct(J2K_USER_DATA *);

static j2k_IReader ReaderInterface = {&OpenJPEGReader_canReadTiles,
                                      &OpenJPEGReader_readTile,
                                      &OpenJPEGReader_readRegion,
                                      &OpenJPEGReader_getContainer,
                                      &OpenJPEGReader_destruct };


NRTPRIV(void) OpenJPEG_cleanup(opj_stream_t **, opj_codec_t **, opj_image_t **);

/******************************************************************************/
/* IO                                                                         */
/******************************************************************************/

NRTAPI(opj_stream_t*)
OpenJPEG_createIO(nrt_IOInterface *io, nrt_Off length, nrt_Error *error)
{
    opj_stream_t *stream = NULL;
    IOControl *ioControl = NULL;

    stream = opj_stream_create(1024, 1);
    if (!stream)
    {
        nrt_Error_init(error, "Error creating openjpeg stream", NRT_CTXT,
                       NRT_ERR_MEMORY);
    }
    else
    {
        /*      Allocate the result */
        ioControl = (IOControl *) NRT_MALLOC(sizeof(IOControl));
        if (ioControl == NULL)
        {
            nrt_Error_init(error, "Error creating control object", NRT_CTXT,
                           NRT_ERR_MEMORY);
            return NULL;
        }
        ioControl->io = io;
        ioControl->offset = nrt_IOInterface_tell(io, error);
        if (length > 0)
            ioControl->length = length;
        else
            ioControl->length = nrt_IOInterface_getSize(io, error)
                    - ioControl->offset;

        opj_stream_set_user_data(stream, ioControl);
        opj_stream_set_read_function(stream,
                                     (opj_stream_read_fn) implStreamRead);
        opj_stream_set_seek_function(stream,
                                     (opj_stream_seek_fn) implStreamSeek);
        opj_stream_set_skip_function(stream,
                                     (opj_stream_skip_fn) implStreamSkip);
        /* TODO write */
    }
    return stream;
}

NRTPRIV(OPJ_UINT32) implStreamRead(void* buf, OPJ_UINT32 bytes, void *data)
{
    IOControl *ctrl = (IOControl*)data;
    nrt_Off offset, bytesLeft, alreadyRead, len;
    OPJ_UINT32 toRead;

    offset = nrt_IOInterface_tell(ctrl->io, &ctrl->error);
    assert(offset >= ctrl->offset); /* probably not a good idea, but need it */

    alreadyRead = offset - ctrl->offset;
    bytesLeft = alreadyRead >= ctrl->length ? 0 : ctrl->length - alreadyRead;
    toRead = bytesLeft < (nrt_Off)bytes ? (OPJ_UINT32)bytesLeft : bytes;
    if (toRead <= 0 || !nrt_IOInterface_read(
                    ctrl->io, (char*)buf, toRead, &ctrl->error))
    {
        return 0;
    }
    return toRead;
}

NRTPRIV(bool) implStreamSeek(OPJ_SIZE_T bytes, void *data)
{
    IOControl *ctrl = (IOControl*)data;
    if (!nrt_IOInterface_seek(ctrl->io, ctrl->offset + bytes,
                    NRT_SEEK_SET, &ctrl->error))
    {
        return 0;
    }
    return 1;
}

NRTPRIV(OPJ_SIZE_T) implStreamSkip(OPJ_SIZE_T bytes, void *data)
{
    IOControl *ctrl = (IOControl*)data;
    if (bytes < 0)
    {
        return 0;
    }
    if (!nrt_IOInterface_seek(ctrl->io, bytes, NRT_SEEK_CUR, &ctrl->error))
    {
        return -1;
    }
    return bytes;
}

NRTPRIV(void)
OpenJPEG_cleanup(opj_stream_t **stream, opj_codec_t **codec,
                  opj_image_t **image)
{
    if (stream && *stream)
    {
        opj_stream_destroy(*stream);
        *stream = NULL;
    }
    if (codec && *codec)
    {
        opj_destroy_codec(*codec);
        *codec = NULL;
    }
    if (image && *image)
    {
        opj_image_destroy(*image);
        *image = NULL;
    }
}

/******************************************************************************/
/* UTILITIES                                                                  */
/******************************************************************************/

NRTPRIV( NRT_BOOL)
OpenJPEG_setup(OpenJPEGReaderImpl *impl, opj_stream_t **stream,
               opj_codec_t **codec, nrt_Error *error)
{
    if (nrt_IOInterface_seek(impl->io, impl->ioOffset, NRT_SEEK_SET, error) < 0)
    {
        goto CATCH_ERROR;
    }

    if (!(*stream = OpenJPEG_createIO(impl->io, 0, error)))
    {
        goto CATCH_ERROR;
    }

    if (!(*codec = opj_create_decompress(CODEC_J2K)))
    {
        goto CATCH_ERROR;
    }

    opj_set_default_decoder_parameters(&impl->parameters);

    if (!opj_setup_decoder(*codec, &impl->parameters))
    {
        nrt_Error_init(error, "Error setting up openjpeg decoder", NRT_CTXT,
                       NRT_ERR_UNK);
        goto CATCH_ERROR;
    }

    return NRT_SUCCESS;

    CATCH_ERROR:
    {
        OpenJPEG_cleanup(stream, codec, NULL);
        return NRT_FAILURE;
    }
}

NRTPRIV( NRT_BOOL)
OpenJPEG_readHeader(OpenJPEGReaderImpl *impl, nrt_Error *error)
{
    OpenJPEGContainerImpl *container = NULL;
    opj_stream_t *stream = NULL;
    opj_image_t *image = NULL;
    opj_codec_t *codec = NULL;
    NRT_BOOL rc = NRT_SUCCESS;

    container = (OpenJPEGContainerImpl*)impl->container->data;

    if (!OpenJPEG_setup(impl, &stream, &codec, error))
    {
        goto CATCH_ERROR;
    }

    if (!opj_read_header(codec, &image, &container->x0, &container->y0,
                         &container->tileWidth, &container->tileHeight,
                         &container->xTiles, &container->yTiles, stream))
    {
        nrt_Error_init(error, "Error reading header", NRT_CTXT, NRT_ERR_UNK);
        goto CATCH_ERROR;
    }

    /* sanity checking */
    if (!image)
    {
        nrt_Error_init(error, "NULL image after reading header", NRT_CTXT,
                       NRT_ERR_UNK);
        goto CATCH_ERROR;
    }

    if (image->x1 - image->x0 <= 0 || image->y1 - image->y0 <= 0)
    {
        nrt_Error_init(error, "Invalid image offsets", NRT_CTXT, NRT_ERR_UNK);
        goto CATCH_ERROR;
    }
    if (image->numcomps == 0)
    {
        nrt_Error_init(error, "No image components found", NRT_CTXT,
                       NRT_ERR_UNK);
        goto CATCH_ERROR;
    }

    container->inputComponentBits = image->comps[0].prec;

    if (container->inputComponentBits > 16)
        container->componentBytes = 4;
    else if (container->inputComponentBits > 8)
        container->componentBytes = 2;
    else
        container->componentBytes = 1;

    container->width = image->x1 - image->x0;
    container->height = image->y1 - image->y0;
    container->nComponents = image->numcomps;

    goto CLEANUP;

    CATCH_ERROR:
    {
        rc = NRT_FAILURE;
    }

    CLEANUP:
    {
        OpenJPEG_cleanup(&stream, &codec, &image);
    }
    return rc;
}

/******************************************************************************/
/* CONTAINER                                                                  */
/******************************************************************************/

NRTPRIV( nrt_Uint32)
OpenJPEGContainer_getTilesX(J2K_USER_DATA *data, nrt_Error *error)
{
    OpenJPEGContainerImpl *impl = (OpenJPEGContainerImpl*) data;
    return impl->xTiles;
}

NRTPRIV( nrt_Uint32)
OpenJPEGContainer_getTilesY(J2K_USER_DATA *data, nrt_Error *error)
{
    OpenJPEGContainerImpl *impl = (OpenJPEGContainerImpl*) data;
    return impl->yTiles;
}

NRTPRIV( nrt_Uint32)
OpenJPEGContainer_getTileWidth(J2K_USER_DATA *data, nrt_Error *error)
{
    OpenJPEGContainerImpl *impl = (OpenJPEGContainerImpl*) data;
    return impl->tileWidth;
}

NRTPRIV( nrt_Uint32)
OpenJPEGContainer_getTileHeight(J2K_USER_DATA *data, nrt_Error *error)
{
    OpenJPEGContainerImpl *impl = (OpenJPEGContainerImpl*) data;
    return impl->tileHeight;
}

NRTPRIV( nrt_Uint32)
OpenJPEGContainer_getWidth(J2K_USER_DATA *data, nrt_Error *error)
{
    OpenJPEGContainerImpl *impl = (OpenJPEGContainerImpl*) data;
    return impl->width;
}

NRTPRIV( nrt_Uint32)
OpenJPEGContainer_getHeight(J2K_USER_DATA *data, nrt_Error *error)
{
    OpenJPEGContainerImpl *impl = (OpenJPEGContainerImpl*) data;
    return impl->height;
}

NRTPRIV( nrt_Uint32)
OpenJPEGContainer_getNumComponents(J2K_USER_DATA *data, nrt_Error *error)
{
    OpenJPEGContainerImpl *impl = (OpenJPEGContainerImpl*) data;
    return impl->nComponents;
}

NRTPRIV( nrt_Uint32)
OpenJPEGContainer_getComponentBytes(J2K_USER_DATA *data, nrt_Error *error)
{
    OpenJPEGContainerImpl *impl = (OpenJPEGContainerImpl*) data;
    return impl->componentBytes;
}

NRTPRIV(void)
OpenJPEGContainer_destruct(J2K_USER_DATA * data)
{
    if (data)
    {
        OpenJPEGContainerImpl *impl = (OpenJPEGContainerImpl*) data;
        NRT_FREE(data);
    }
}

/******************************************************************************/
/* READER                                                                     */
/******************************************************************************/

NRTPRIV( NRT_BOOL)
OpenJPEGReader_canReadTiles(J2K_USER_DATA *data, nrt_Error *error)
{
    return NRT_SUCCESS;
}

NRTPRIV( nrt_Uint64)
OpenJPEGReader_readTile(J2K_USER_DATA *data, nrt_Uint32 tileX, nrt_Uint32 tileY,
                  nrt_Uint8 **buf, nrt_Error *error)
{
    OpenJPEGReaderImpl *impl = (OpenJPEGReaderImpl*) data;
    OpenJPEGContainerImpl *container = NULL;

    opj_stream_t *stream = NULL;
    opj_image_t *image = NULL;
    opj_codec_t *codec = NULL;
    nrt_Uint32 bufSize;

    if (!OpenJPEG_setup(impl, &stream, &codec, error))
    {
        goto CATCH_ERROR;
    }

    container = (OpenJPEGContainerImpl*)impl->container->data;

    /* unfortunately, we need to read the header every time ... */
    if (!opj_read_header(codec, &image, &container->x0, &container->y0,
                         &container->tileWidth, &container->tileHeight,
                         &container->xTiles, &container->yTiles, stream))
    {
        nrt_Error_init(error, "Error reading header", NRT_CTXT, NRT_ERR_UNK);
        goto CATCH_ERROR;
    }

    /* only decode what we want */
    if (!opj_set_decode_area(codec, container->tileWidth * tileX,
                             container->tileHeight * tileY,
                             container->tileWidth * (tileX + 1),
                             container->tileHeight * (tileY + 1)))
    {
        nrt_Error_init(error, "Error decoding area", NRT_CTXT, NRT_ERR_UNK);
        goto CATCH_ERROR;
    }

    {
        int keepGoing;
        OPJ_UINT32 tileIndex, nComponents;
        OPJ_INT32 tileX0, tileY0, tileX1, tileY1;

        if (!opj_read_tile_header(codec, &tileIndex, &bufSize, &tileX0,
                                  &tileY0, &tileX1, &tileY1, &nComponents,
                                  &keepGoing, stream))
        {
            nrt_Error_init(error, "Error reading tile header", NRT_CTXT,
                           NRT_ERR_UNK);
            goto CATCH_ERROR;
        }

        if (keepGoing)
        {

            if (buf && !*buf)
            {
                *buf = (nrt_Uint8*)NRT_MALLOC(bufSize);
                if (!*buf)
                {
                    nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                                   NRT_ERR_MEMORY);
                    goto CATCH_ERROR;
                }
            }

            if (!opj_decode_tile_data(codec, tileIndex, *buf, bufSize, stream))
            {
                nrt_Error_init(error, "Error decoding tile", NRT_CTXT,
                               NRT_ERR_UNK);
                goto CATCH_ERROR;
            }
        }
    }

    goto CLEANUP;

    CATCH_ERROR:
    {
        bufSize = 0;
    }

    CLEANUP:
    {
        OpenJPEG_cleanup(&stream, &codec, &image);
    }
    return (nrt_Uint64)bufSize;
}

NRTPRIV( nrt_Uint64)
OpenJPEGReader_readRegion(J2K_USER_DATA *data, nrt_Uint32 x0, nrt_Uint32 y0,
                          nrt_Uint32 x1, nrt_Uint32 y1, nrt_Uint8 **buf,
                          nrt_Error *error)
{
    OpenJPEGReaderImpl *impl = (OpenJPEGReaderImpl*) data;
    OpenJPEGContainerImpl *container = NULL;

    opj_stream_t *stream = NULL;
    opj_image_t *image = NULL;
    opj_codec_t *codec = NULL;
    nrt_Uint64 bufSize;
    nrt_Uint64 offset = 0;

    if (!OpenJPEG_setup(impl, &stream, &codec, error))
    {
        goto CATCH_ERROR;
    }

    container = (OpenJPEGContainerImpl*)impl->container->data;

    /* unfortunately, we need to read the header every time ... */
    if (!opj_read_header(codec, &image, &container->x0, &container->y0,
                         &container->tileWidth, &container->tileHeight,
                         &container->xTiles, &container->yTiles, stream))
    {
        nrt_Error_init(error, "Error reading header", NRT_CTXT, NRT_ERR_UNK);
        goto CATCH_ERROR;
    }

    if (x1 == 0)
        x1 = container->width;
    if (y1 == 0)
        y1 = container->height;

    /* only decode what we want */
    if (!opj_set_decode_area(codec, x0, y0, x1, y1))
    {
        nrt_Error_init(error, "Error decoding area", NRT_CTXT, NRT_ERR_UNK);
        goto CATCH_ERROR;
    }

    bufSize = (nrt_Uint64)(x1 - x0) * (y1 - y0) * container->componentBytes
            * container->nComponents;
    if (buf && !*buf)
    {
        *buf = (nrt_Uint8*)NRT_MALLOC(bufSize);
        if (!*buf)
        {
            nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                           NRT_ERR_MEMORY);
            goto CATCH_ERROR;
        }
    }

    {
        int keepGoing;
        OPJ_UINT32 tileIndex, nComponents, reqSize;
        OPJ_INT32 tileX0, tileY0, tileX1, tileY1;

        do
        {
            if (!opj_read_tile_header(codec, &tileIndex, &reqSize, &tileX0,
                                      &tileY0, &tileX1, &tileY1, &nComponents,
                                      &keepGoing, stream))
            {
                nrt_Error_init(error, "Error reading tile header", NRT_CTXT,
                               NRT_ERR_UNK);
                goto CATCH_ERROR;
            }

            if (keepGoing)
            {
                if (!opj_decode_tile_data(codec, tileIndex, (*buf + offset),
                                          reqSize, stream))
                {
                    nrt_Error_init(error, "Error decoding tile", NRT_CTXT,
                                   NRT_ERR_UNK);
                    goto CATCH_ERROR;
                }
                offset += reqSize;
            }
        }
        while (keepGoing);
    }

    goto CLEANUP;

    CATCH_ERROR:
    {
        bufSize = 0;
    }

    CLEANUP:
    {
        OpenJPEG_cleanup(&stream, &codec, &image);
    }
    return bufSize;
}

NRTPRIV( j2k_Container*)
OpenJPEGReader_getContainer(J2K_USER_DATA *data, nrt_Error *error)
{
    OpenJPEGReaderImpl *impl = (OpenJPEGReaderImpl*) data;
    return impl->container;
}

NRTPRIV(void)
OpenJPEGReader_destruct(J2K_USER_DATA * data)
{
    if (data)
    {
        OpenJPEGReaderImpl *impl = (OpenJPEGReaderImpl*) data;
        if (impl->io && impl->ownIO)
        {
            nrt_IOInterface_destruct(&impl->io);
            impl->io = NULL;
        }
        NRT_FREE(data);
    }
}

/******************************************************************************/
/******************************************************************************/
/* PUBLIC FUNCTIONS                                                           */
/******************************************************************************/
/******************************************************************************/

NRTAPI(j2k_Reader*) j2k_Reader_open(const char *fname, nrt_Error *error)
{
    j2k_Reader *reader = NULL;
    nrt_IOHandle handle;
    nrt_IOInterface *io = NULL;

    if (!fname)
    {
        nrt_Error_init(error, "NULL filename", NRT_CTXT,
                       NRT_ERR_INVALID_OBJECT);
        goto CATCH_ERROR;
    }

    handle = nrt_IOHandle_create(fname, NRT_ACCESS_READONLY,
                                 NRT_OPEN_EXISTING, error);
    if (NRT_INVALID_HANDLE(handle))
    {
        nrt_Error_init(error, "Invalid IO handle", NRT_CTXT,
                               NRT_ERR_INVALID_OBJECT);
        goto CATCH_ERROR;
    }

    if (!(io = nrt_IOHandleAdaptor_construct(handle, error)))
        goto CATCH_ERROR;

    if (!(reader = j2k_Reader_openIO(io, error)))
        goto CATCH_ERROR;

    ((OpenJPEGReaderImpl*) reader->data)->ownIO = 1;

    return reader;

    CATCH_ERROR:
    {
        if (io)
            nrt_IOInterface_destruct(&io);
        if (reader)
            j2k_Reader_destruct(&reader);
        return NULL;
    }
}

NRTAPI(j2k_Reader*) j2k_Reader_openIO(nrt_IOInterface *io, nrt_Error *error)
{
    OpenJPEGReaderImpl *impl = NULL;
    j2k_Reader *reader = NULL;
    OpenJPEGContainerImpl *container = NULL;

    /* create the Reader interface */
    impl = (OpenJPEGReaderImpl *) NRT_MALLOC(sizeof(OpenJPEGReaderImpl));
    if (!impl)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(impl, 0, sizeof(OpenJPEGReaderImpl));

    reader = (j2k_Reader *) NRT_MALLOC(sizeof(j2k_Reader));
    if (!reader)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(reader, 0, sizeof(j2k_Reader));
    reader->data = impl;
    reader->iface = &ReaderInterface;

    /* create the Container interface */
    container = (OpenJPEGContainerImpl *) NRT_MALLOC(sizeof(OpenJPEGContainerImpl));
    if (!container)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(container, 0, sizeof(OpenJPEGContainerImpl));

    impl->container = (j2k_Container*) NRT_MALLOC(sizeof(j2k_Container));
    if (!impl->container)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(impl->container, 0, sizeof(j2k_Container));
    impl->container->iface = &ContainerInterface;
    impl->container->data = container;

    /* initialize the interfaces */
    impl->io = io;
    impl->ioOffset = nrt_IOInterface_tell(io, error);

    if (!OpenJPEG_readHeader(impl, error))
    {
        goto CATCH_ERROR;
    }

    return reader;

    CATCH_ERROR:
    {
        if (reader)
        {
            j2k_Reader_destruct(&reader);
        }
        else if (impl)
        {
            OpenJPEGReader_destruct((J2K_USER_DATA*) impl);
        }
        return NULL;
    }
}

NRTAPI(j2k_Container*) j2k_Container_construct(nrt_Uint32 width,
        nrt_Uint32 height,
        nrt_Uint32 bands,
        nrt_Uint32 actualBitsPerPixel,
        nrt_Uint32 tileWidth,
        nrt_Uint32 tileHeight,
        int imageType,
        int isSigned,
        nrt_Error *error)
{
    j2k_Container *container = NULL;
    OpenJPEGContainerImpl *impl = NULL;

    container = (j2k_Container*) NRT_MALLOC(sizeof(j2k_Container));
    if (!container)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(container, 0, sizeof(j2k_Container));

    /* create the Container interface */
    impl = (OpenJPEGContainerImpl *) NRT_MALLOC(sizeof(OpenJPEGContainerImpl));
    if (!impl)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(impl, 0, sizeof(OpenJPEGContainerImpl));
    container->data = impl;
    container->iface = &ContainerInterface;

    impl->width = width;
    impl->height = height;
    impl->nComponents = bands;
    impl->componentBytes = (actualBitsPerPixel - 1) / 8 + 1;
    impl->inputComponentBits = actualBitsPerPixel;
    impl->tileWidth = tileWidth;
    impl->tileHeight = tileHeight;
    impl->imageType = imageType;
    impl->isSigned = isSigned;
    impl->xTiles = width / tileWidth + (width % tileWidth == 0 ? 0 : 1);
    impl->yTiles = height / tileHeight + (height % tileHeight == 0 ? 0 : 1);

    return container;

    CATCH_ERROR:
    {
        if (container)
        {
            j2k_Container_destruct(&container);
        }
        return NULL;
    }
}

#endif
