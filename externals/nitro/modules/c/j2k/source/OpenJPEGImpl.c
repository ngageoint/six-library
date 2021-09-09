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

#include "j2k/Config.h"

#ifdef _MSC_VER
#pragma warning(disable: 4706) // assignment within conditional expression
#endif // _MSC_VER

#ifdef HAVE_OPENJPEG_H

#include <string.h>

#include "j2k/Container.h"
#include "j2k/Reader.h"
#include "j2k/Writer.h"

#include <openjpeg.h>


/******************************************************************************/
/* TYPES & DECLARATIONS                                                       */
/******************************************************************************/

#define OPENJPEG_STREAM_SIZE 1024

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
    IOControl userData;
} OpenJPEGReaderImpl;

typedef struct _OpenJPEGWriterImpl
{
    j2k_Container *container;
    opj_codec_t *codec;
    opj_image_t *image;
    char *compressedBuf;
    nrt_IOInterface *compressed;
    opj_stream_t *stream;
    IOControl userData;
} OpenJPEGWriterImpl;

typedef struct _OpenJPEGError
{
    nrt_Error *error;
    void *context;
} OpenJPEGError;

J2KPRIV(OPJ_SIZE_T) implStreamRead(void* buf, OPJ_SIZE_T bytes, void *data);
J2KPRIV(OPJ_BOOL)   implStreamSeek(OPJ_OFF_T bytes, void *data);
J2KPRIV(OPJ_OFF_T)  implStreamSkip(OPJ_OFF_T bytes, void *data);
J2KPRIV(OPJ_SIZE_T) implStreamWrite(void *buf, OPJ_SIZE_T bytes, void *data);


J2KPRIV( NRT_BOOL  )     OpenJPEGReader_canReadTiles(J2K_USER_DATA *,  nrt_Error *);
J2KPRIV( uint64_t)     OpenJPEGReader_readTile(J2K_USER_DATA *, uint32_t,
                                                 uint32_t, uint8_t **,
                                                 nrt_Error *);
J2KPRIV( uint64_t)     OpenJPEGReader_readRegion(J2K_USER_DATA *, uint32_t,
                                                   uint32_t, uint32_t,
                                                   uint32_t, uint8_t **,
                                                   nrt_Error *);
J2KPRIV( j2k_Container*) OpenJPEGReader_getContainer(J2K_USER_DATA *, nrt_Error *);
J2KPRIV(void)            OpenJPEGReader_destruct(J2K_USER_DATA *);

static j2k_IReader ReaderInterface = {&OpenJPEGReader_canReadTiles,
                                      &OpenJPEGReader_readTile,
                                      &OpenJPEGReader_readRegion,
                                      &OpenJPEGReader_getContainer,
                                      &OpenJPEGReader_destruct };

J2KPRIV( NRT_BOOL)       OpenJPEGWriter_setTile(J2K_USER_DATA *,
                                                uint32_t, uint32_t,
                                                const uint8_t *, uint32_t,
                                                nrt_Error *);
J2KPRIV( NRT_BOOL)       OpenJPEGWriter_write(J2K_USER_DATA *, nrt_IOInterface *,
                                              nrt_Error *);
J2KPRIV( j2k_Container*) OpenJPEGWriter_getContainer(J2K_USER_DATA *, nrt_Error *);
J2KPRIV(void)            OpenJPEGWriter_destruct(J2K_USER_DATA *);

static j2k_IWriter WriterInterface = {&OpenJPEGWriter_setTile,
                                      &OpenJPEGWriter_write,
                                      &OpenJPEGWriter_getContainer,
                                      &OpenJPEGWriter_destruct };


J2KPRIV(void) OpenJPEG_cleanup(opj_stream_t **, opj_codec_t **, opj_image_t **);
J2KPRIV( J2K_BOOL) OpenJPEG_initImage(OpenJPEGWriterImpl *, j2k_WriterOptions *,
                                      nrt_Error *);

J2KPRIV(void) OpenJPEG_errorHandler(const char* msg, void* data)
{
    nrt_Error* error = (nrt_Error*)data;
    /* Initialize the first message, otherwise the message will be
       overridden up the stack */
    if(strlen(error->message) == 0)
    {
        nrt_Error_init(error, msg, NRT_CTXT, NRT_ERR_INVALID_OBJECT);
    }
}

/******************************************************************************/
/* IO                                                                         */
/******************************************************************************/

J2KAPI(opj_stream_t*)
OpenJPEG_createIO(nrt_IOInterface* io,
                  IOControl* ioControl,
                  nrt_Off length,
                  int isInput,
                  nrt_Error *error)
{
    opj_stream_t *stream = NULL;

    stream = opj_stream_create(OPENJPEG_STREAM_SIZE, isInput);
    if (!stream)
    {
        nrt_Error_init(error, "Error creating openjpeg stream", NRT_CTXT,
                       NRT_ERR_MEMORY);
    }
    else
    {
        ioControl->io = io;
        ioControl->offset = nrt_IOInterface_tell(io, error);
        if (length > 0)
        {
            ioControl->length = length;
        }
        else
        {
            /*
             * TODO: Technically we should just report the number of bytes
             *       remaining in this image segment, not in the whole NITF,
             *       though if we somehow read beyond this image segment,
             *       presumably we'd get OpenJPEG errors because the bytes
             *       we'd be reading wouldn't be J2K tiles
             */
            ioControl->length =
                    nrt_IOInterface_getSize(io, error) - ioControl->offset;
        }

        opj_stream_set_user_data(stream, ioControl, NULL);
        opj_stream_set_user_data_length(stream, ioControl->length);
        opj_stream_set_read_function(stream, implStreamRead);
        opj_stream_set_seek_function(stream, implStreamSeek);
        opj_stream_set_skip_function(stream, implStreamSkip);
        opj_stream_set_write_function(stream, implStreamWrite);
    }
    return stream;
}

J2KPRIV(OPJ_SIZE_T) implStreamRead(void* buf, OPJ_SIZE_T bytes, void *data)
{
    IOControl *ctrl = (IOControl*)data;
    nrt_Off offset, alreadyRead;
    OPJ_SIZE_T bytesLeft;
    OPJ_SIZE_T toRead;

    offset = nrt_IOInterface_tell(ctrl->io, &ctrl->error);
    assert(offset >= ctrl->offset); /* probably not a good idea, but need it */

    alreadyRead = offset - ctrl->offset;
    bytesLeft = alreadyRead >= ctrl->length ?
            0 : (OPJ_SIZE_T)(ctrl->length - alreadyRead);
    toRead = bytesLeft < bytes ? bytesLeft : bytes;
    if (toRead <= 0 || !nrt_IOInterface_read(
                    ctrl->io, (char*)buf, toRead, &ctrl->error))
    {
        return (OPJ_SIZE_T) -1;
    }
    return toRead;
}

J2KPRIV(OPJ_BOOL) implStreamSeek(OPJ_OFF_T bytes, void *data)
{
    IOControl *ctrl = (IOControl*)data;
    if (!NRT_IO_SUCCESS(nrt_IOInterface_seek(ctrl->io,
                                             ctrl->offset + bytes,
                                             NRT_SEEK_SET,
                                             &ctrl->error)))
    {
        return 0;
    }
    return 1;
}

J2KPRIV(OPJ_OFF_T) implStreamSkip(OPJ_OFF_T bytes, void *data)
{
    IOControl *ctrl = (IOControl*)data;
    if (bytes < 0)
    {
        return 0;
    }
    if (!NRT_IO_SUCCESS(nrt_IOInterface_seek(ctrl->io,
                        bytes,
                        NRT_SEEK_CUR,
                        &ctrl->error)))
    {
        return -1;
    }
    return bytes;
}

J2KPRIV(OPJ_SIZE_T) implStreamWrite(void *buf, OPJ_SIZE_T bytes, void *data)
{
    IOControl *ctrl = (IOControl*)data;
    if (bytes == 0)
    {
        return 0;
    }
    if (!nrt_IOInterface_write(ctrl->io, (const char*)buf,
                               bytes, &ctrl->error))
    {
        return (OPJ_SIZE_T)-1;
    }
    return bytes;
}

J2KPRIV(void)
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

J2KPRIV( NRT_BOOL)
OpenJPEG_setup(OpenJPEGReaderImpl *impl, opj_stream_t **stream,
               opj_codec_t **codec, nrt_Error *error)
{
    if (!NRT_IO_SUCCESS(nrt_IOInterface_seek(impl->io,
                                             impl->ioOffset,
                                             NRT_SEEK_SET,
                                             error)))
    {
        goto CATCH_ERROR;
    }

    if (!(*stream = OpenJPEG_createIO(impl->io, &impl->userData, 0, 1, error)))
    {
        goto CATCH_ERROR;
    }

    if (!(*codec = opj_create_decompress(OPJ_CODEC_J2K)))
    {
        nrt_Error_init(error, "Error creating OpenJPEG codec", NRT_CTXT,
                       NRT_ERR_INVALID_OBJECT);
        goto CATCH_ERROR;
    }

    memset(error->message, 0, NRT_MAX_EMESSAGE);
    if(!opj_set_error_handler(*codec,
                              OpenJPEG_errorHandler,
                              error))
    {
        nrt_Error_init(error, "Unable to set OpenJPEG error handler", NRT_CTXT,
                       NRT_ERR_UNK);
        goto CATCH_ERROR;
    }

    opj_set_default_decoder_parameters(&impl->parameters);

    if (!opj_setup_decoder(*codec, &impl->parameters))
    {
        /*nrt_Error_init(error, "Error setting up openjpeg decoder", NRT_CTXT,
          NRT_ERR_UNK);*/
        goto CATCH_ERROR;
    }

    return NRT_SUCCESS;

    CATCH_ERROR:
    {
        OpenJPEG_cleanup(stream, codec, NULL);
        return NRT_FAILURE;
    }
}

J2KPRIV( NRT_BOOL)
OpenJPEG_readHeader(OpenJPEGReaderImpl *impl, nrt_Error *error)
{
    opj_stream_t *stream = NULL;
    opj_image_t *image = NULL;
    opj_codec_t *codec = NULL;
    opj_codestream_info_v2_t* codeStreamInfo = NULL;
    NRT_BOOL rc = NRT_SUCCESS;
    OPJ_UINT32 tileWidth, tileHeight;
    OPJ_UINT32 imageWidth, imageHeight;

    if (!OpenJPEG_setup(impl, &stream, &codec, error))
    {
        goto CATCH_ERROR;
    }

    if (!opj_read_header(stream, codec, &image))
    {
        /*nrt_Error_init(error, "Error reading header", NRT_CTXT, NRT_ERR_UNK);*/
        goto CATCH_ERROR;
    }

    codeStreamInfo = opj_get_cstr_info(codec);
    if (!codeStreamInfo)
    {
        /*nrt_Error_init(error, "Error reading code stream", NRT_CTXT, NRT_ERR_UNK);*/
        goto CATCH_ERROR;
    }
    tileWidth = codeStreamInfo->tdx;
    tileHeight = codeStreamInfo->tdy;

    /* sanity checking */
    if (!image)
    {
        nrt_Error_init(error, "NULL image after reading header", NRT_CTXT,
                       NRT_ERR_UNK);
        goto CATCH_ERROR;
    }

    if (image->x0 >= image->x1 || image->y0 >= image->y1)
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

    /* TODO: We need special handling that's not implemented in readTile() to
     *       accommodate partial tiles with more than one band. */
    imageWidth = image->x1 - image->x0;
    imageHeight = image->y1 - image->y0;
    if (image->numcomps > 1 &&
        (imageWidth % tileWidth != 0 || imageHeight % tileHeight != 0))
    {
        nrt_Error_init(error, "No image components found", NRT_CTXT,
                       NRT_ERR_UNK);
        goto CATCH_ERROR;
    }

    if (!impl->container)
    {
        /* initialize the container */
        uint32_t idx;
        j2k_Component **components = NULL;
        int imageType;

        if (!(components = (j2k_Component**)J2K_MALLOC(
                sizeof(j2k_Component*) * image->numcomps)))
        {
            nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                           NRT_ERR_MEMORY);
            goto CATCH_ERROR;
        }

        for(idx = 0; idx < image->numcomps; ++idx)
        {
            opj_image_comp_t cmp = image->comps[idx];
            if (!(components[idx] = j2k_Component_construct(cmp.w, cmp.h,
                                                            cmp.prec, cmp.sgnd,
                                                            cmp.x0, cmp.y0,
                                                            cmp.dx, cmp.dy,
                                                            error)))
            {
                goto CATCH_ERROR;
            }
        }

        switch(image->color_space)
        {
        case OPJ_CLRSPC_SRGB:
            imageType = J2K_TYPE_RGB;
            break;
        case OPJ_CLRSPC_GRAY:
            imageType = J2K_TYPE_MONO;
            break;

        case OPJ_CLRSPC_CMYK:
        case OPJ_CLRSPC_SYCC:
        case OPJ_CLRSPC_EYCC:
            imageType = J2K_TYPE_UNKNOWN;
            break;

        case OPJ_CLRSPC_UNKNOWN:
        case OPJ_CLRSPC_UNSPECIFIED:
        default:
            imageType = J2K_TYPE_UNKNOWN;
        }

        if (!(impl->container = j2k_Container_construct(image->x1 - image->x0,
                                                        image->y1 - image->y0,
                                                        image->numcomps,
                                                        components,
                                                        tileWidth, tileHeight,
                                                        imageType, error)))
        {
            goto CATCH_ERROR;
        }
    }

    goto CLEANUP;

    CATCH_ERROR:
    {
        rc = NRT_FAILURE;
    }

    CLEANUP:
    {
        if (codeStreamInfo)
        {
            opj_destroy_cstr_info(&codeStreamInfo);
        }
        OpenJPEG_cleanup(&stream, &codec, &image);
    }
    return rc;
}

J2KPRIV( NRT_BOOL) OpenJPEG_initImage(OpenJPEGWriterImpl *impl,
                                      j2k_WriterOptions *writerOps,
                                      nrt_Error *error)
{
    NRT_BOOL rc = NRT_SUCCESS;
    uint32_t i, nComponents, height, width, tileHeight, tileWidth;
    uint32_t nBytes;
    j2k_Component *component = NULL;
    size_t uncompressedSize;
    int imageType;
    opj_cparameters_t encoderParams;
    opj_image_cmptparm_t *cmptParams;
    OPJ_COLOR_SPACE colorSpace;

    nComponents = j2k_Container_getNumComponents(impl->container, error);
    width = j2k_Container_getWidth(impl->container, error);
    height = j2k_Container_getHeight(impl->container, error);
    tileWidth = j2k_Container_getTileWidth(impl->container, error);
    tileHeight = j2k_Container_getTileHeight(impl->container, error);
    imageType = j2k_Container_getImageType(impl->container, error);

    /* Set up the encoder parameters.  This defaults to lossless. */
    /* TODO allow overrides somehow? */
    opj_set_default_encoder_parameters(&encoderParams);

    /* For now we are enforcing lossless compression.  If we have a better
     * way to allow overrides in the future, uncomment out the tcp_rates logic
     * below (tcp_rates[0] == 0 via opj_set_default_encoder_parameters()).
     * Also consider setting encoderParams.irreversible = 1; to use the
     * lossy DWT 9-7 instead of the reversible 5-3.
     */

    /*if (writerOps && writerOps->compressionRatio > 0.0001)
        encoderParams.tcp_rates[0] = 1.0 / writerOps->compressionRatio;
    else
        encoderParams.tcp_rates[0] = 4.0;
    */

    /* TODO: These two lines should not be necessary when using lossless
     *       encoding but appear to be needed (at least in OpenJPEG 2.0) -
     *       otherwise we get a seg fault.
     *       The sample opj_compress.c is doing the same thing with a comment
     *       indicating that it's a bug. */
    ++encoderParams.tcp_numlayers;
    encoderParams.cp_disto_alloc = 1;

    if (writerOps && writerOps->numResolutions > 0)
    {
        encoderParams.numresolution = writerOps->numResolutions;
        /* Note, if this isn't set right (see below) it will error out */
    }
    else
    {
        /*
           OpenJPEG defaults this to 6, but that causes the compressor
           to fail if the tile sizes are less than 2^6.  So we adjust this
           down if necessary.
        */
        const double logTwo = log(2);
        const OPJ_UINT32 minX = (OPJ_UINT32)floor(log(tileWidth) / logTwo);
        const OPJ_UINT32 minY = (OPJ_UINT32)floor(log(tileHeight) / logTwo);
        const OPJ_UINT32 minXY = (minX < minY) ? minX : minY;
        if (minXY < (OPJ_UINT32)encoderParams.numresolution)
        {
            encoderParams.numresolution = minXY;
        }
    }

    /* Turn on tiling */
    encoderParams.tile_size_on = 1;
    encoderParams.cp_tx0 = 0;
    encoderParams.cp_ty0 = 0;
    encoderParams.cp_tdx = tileWidth;
    encoderParams.cp_tdy = tileHeight;

    if (!(cmptParams = (opj_image_cmptparm_t*)J2K_MALLOC(sizeof(
            opj_image_cmptparm_t) * nComponents)))
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(cmptParams, 0, sizeof(opj_image_cmptparm_t) * nComponents);

    for(i = 0; i < nComponents; ++i)
    {
        component = j2k_Container_getComponent(impl->container, i, error);
        cmptParams[i].w = j2k_Component_getWidth(component, error);
        cmptParams[i].h = j2k_Component_getHeight(component, error);
        cmptParams[i].prec = j2k_Component_getPrecision(component, error);
        cmptParams[i].x0 = j2k_Component_getOffsetX(component, error);
        cmptParams[i].y0 = j2k_Component_getOffsetY(component, error);
        cmptParams[i].dx = j2k_Component_getSeparationX(component, error);
        cmptParams[i].dy = j2k_Component_getSeparationY(component, error);
        cmptParams[i].sgnd = j2k_Component_isSigned(component, error);
    }

    nBytes = (j2k_Container_getPrecision(impl->container, error) - 1) / 8 + 1;
    uncompressedSize = ((size_t)width) * height * nComponents * nBytes;

    /* this is not ideal, but there is really no other way */
    if (!(impl->compressedBuf = (char*)J2K_MALLOC(uncompressedSize)))
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    if (!(impl->compressed = nrt_BufferAdapter_construct(impl->compressedBuf,
                                                         uncompressedSize, 1,
                                                         error)))
    {
        goto CATCH_ERROR;
    }

    if (!(impl->stream = OpenJPEG_createIO(impl->compressed, &impl->userData,
                                           0, 0, error)))
    {
        goto CATCH_ERROR;
    }


    switch(imageType)
    {
    case J2K_TYPE_RGB:
        colorSpace = OPJ_CLRSPC_SRGB;
        break;
    default:
        colorSpace = OPJ_CLRSPC_GRAY;
    }

    if (!(impl->codec = opj_create_compress(OPJ_CODEC_J2K)))
    {
        nrt_Error_init(error, "Error creating OpenJPEG codec", NRT_CTXT,
                       NRT_ERR_INVALID_OBJECT);
        goto CATCH_ERROR;
    }
    if (!(impl->image = opj_image_tile_create(nComponents, cmptParams,
                                              colorSpace)))
    {
        nrt_Error_init(error, "Error creating OpenJPEG image", NRT_CTXT,
                       NRT_ERR_INVALID_OBJECT);
        goto CATCH_ERROR;
    }

    /* for some reason we must also explicitly specify these in the image... */
    impl->image->numcomps = nComponents;
    impl->image->x0 = 0;
    impl->image->y0 = 0;
    impl->image->x1 = width;
    impl->image->y1 = height;
    impl->image->color_space = colorSpace;

    memset(error->message, 0, NRT_MAX_EMESSAGE);
    if(!opj_set_error_handler(impl->codec,
                              OpenJPEG_errorHandler,
                              error))
    {
        nrt_Error_init(error, "Unable to set OpenJPEG error handler", NRT_CTXT,
                       NRT_ERR_UNK);
        goto CATCH_ERROR;
    }

    if (!opj_setup_encoder(impl->codec, &encoderParams, impl->image))
    {
        /*nrt_Error_init(error, "Error setting up OpenJPEG decoder", NRT_CTXT,
          NRT_ERR_INVALID_OBJECT);*/
        goto CATCH_ERROR;
    }

    if (!opj_start_compress(impl->codec, impl->image, impl->stream))
    {
        /*nrt_Error_init(error, "Error starting OpenJPEG compression", NRT_CTXT,
          NRT_ERR_INVALID_OBJECT);*/
        goto CATCH_ERROR;
    }

    goto CLEANUP;

    CATCH_ERROR:
    {
        rc = NRT_FAILURE;
    }

    CLEANUP:
    {
        if (cmptParams)
            J2K_FREE(cmptParams);
    }

    return rc;
}


/******************************************************************************/
/* READER                                                                     */
/******************************************************************************/

J2KPRIV( NRT_BOOL)
OpenJPEGReader_canReadTiles(J2K_USER_DATA *data, nrt_Error *error)
{
    (void)data;
    (void)error;
    return NRT_SUCCESS;
}

J2KPRIV( uint64_t)
OpenJPEGReader_readTile(J2K_USER_DATA *data, uint32_t tileX, uint32_t tileY,
                  uint8_t **buf, nrt_Error *error)
{
    OpenJPEGReaderImpl *impl = (OpenJPEGReaderImpl*) data;

    opj_stream_t *stream = NULL;
    opj_image_t *image = NULL;
    opj_codec_t *codec = NULL;
    uint32_t bufSize;
    const OPJ_UINT32 tileWidth = j2k_Container_getTileWidth(impl->container, error);
    const OPJ_UINT32 tileHeight = j2k_Container_getTileHeight(impl->container, error);
    size_t numBitsPerPixel = 0;
    size_t numBytesPerPixel = 0;
    uint64_t fullBufSize = 0;

    if (!OpenJPEG_setup(impl, &stream, &codec, error))
    {
        goto CATCH_ERROR;
    }

    /* unfortunately, we need to read the header every time ... */
    if (!opj_read_header(stream, codec, &image))
    {
        /*nrt_Error_init(error, "Error reading header", NRT_CTXT, NRT_ERR_UNK);*/
        goto CATCH_ERROR;
    }

    /* only decode what we want */
    if (!opj_set_decode_area(codec, image, tileWidth * tileX, tileHeight * tileY,
                             tileWidth * (tileX + 1), tileHeight * (tileY + 1)))
    {
        /*nrt_Error_init(error, "Error decoding area", NRT_CTXT, NRT_ERR_UNK);*/
        goto CATCH_ERROR;
    }

    {
        int keepGoing;
        OPJ_UINT32 tileIndex, nComponents;
        OPJ_INT32 tileX0, tileY0, tileX1, tileY1;

        if (!opj_read_tile_header(codec, stream, &tileIndex, &bufSize, &tileX0,
                                  &tileY0, &tileX1, &tileY1, &nComponents,
                                  &keepGoing))
        {
            /*nrt_Error_init(error, "Error reading tile header", NRT_CTXT,
              NRT_ERR_UNK);*/
            goto CATCH_ERROR;
        }

        if (keepGoing)
        {
            /* TODO: The way blockIO->cntl->blockOffsetInc is currently
             *       implemented in ImageIO.c corresponds with how a
             *       non-compressed partial block would be laid out in a
             *       NITF - the actual extra columns would have been read.
             *       Not sure how the J2K data is laid out on disk but
             *       OpenJPEG is hiding this from us if the extra columns are
             *       present there.  So whenever we get a partial tile that
             *       isn't at the full width, we need to add in these extra
             *       columns of 0's ourselves.  Potentially we could update
             *       ImageIO.c to not require this instead.  Note that we
             *       don't need to pad out the extra rows for a partial block
             *       that isn't the full height because ImageIO will never try
             *       to memcpy these in - we only need to get the stride to
             *       work out correctly.
             */
            const OPJ_UINT32 thisTileWidth = tileX1 - tileX0;
            const OPJ_UINT32 thisTileHeight = tileY1 - tileY0;
            if (thisTileWidth < tileWidth)
            {
                /* TODO: The current approach below only works for single band
                 *       imagery.  For RGB data, I believe it is stored as all
                 *       red, then all green, then all blue, so we would need
                 *       a temp buffer rather than reusing the current buffer.
                 */
                if (nComponents != 1)
                {
                    nrt_Error_init(
                        error,
                        "Partial tile width not implemented for multi-band",
                        NRT_CTXT, NRT_ERR_UNK);
                    goto CATCH_ERROR;
                }

                numBitsPerPixel =
                    j2k_Container_getPrecision(impl->container, error);
                numBytesPerPixel =
                    (numBitsPerPixel / 8) + (numBitsPerPixel % 8 != 0);
                fullBufSize = ((uint64_t)tileWidth) * thisTileHeight * numBytesPerPixel;
            }
            else
            {
                fullBufSize = bufSize;
            }

            if (buf && !*buf)
            {
                *buf = (uint8_t*)J2K_MALLOC(fullBufSize);
                if (!*buf)
                {
                    nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                                   NRT_ERR_MEMORY);
                    goto CATCH_ERROR;
                }
            }

            if ((buf != NULL) && !opj_decode_tile_data(codec, tileIndex, *buf, bufSize, stream))
            {
                /*nrt_Error_init(error, "Error decoding tile", NRT_CTXT,
                  NRT_ERR_UNK);*/
                goto CATCH_ERROR;
            }

            if (thisTileWidth < tileWidth)
            {
                /* We have a tile that isn't as wide as it "should" be
                 * Need to add in the extra columns ourselves.  By marching
                 * through the rows backwards, we can do this in place.
                 */
                const size_t srcStride = thisTileWidth * numBytesPerPixel;
                const size_t destStride = tileWidth * numBytesPerPixel;
                const size_t numLeftoverBytes = destStride - srcStride;
                OPJ_UINT32 lastRow = thisTileHeight - 1;
                size_t srcOffset = lastRow * srcStride;
                size_t destOffset = lastRow * destStride;
                OPJ_UINT32 ii;
                uint8_t* bufPtr = buf != NULL ? *buf : NULL;

                for (ii = 0;
                     ii < thisTileHeight;
                     ++ii, srcOffset -= srcStride, destOffset -= destStride)
                {
                    uint8_t* const dest = bufPtr + destOffset;
                    memmove(dest, bufPtr + srcOffset, srcStride);
                    memset(dest + srcStride, 0, numLeftoverBytes);
                }
            }
        }
    }

    goto CLEANUP;

    CATCH_ERROR:
    {
        fullBufSize = 0;
    }

    CLEANUP:
    {
        OpenJPEG_cleanup(&stream, &codec, &image);
    }
    return fullBufSize;
}

J2KPRIV( uint64_t)
OpenJPEGReader_readRegion(J2K_USER_DATA *data, uint32_t x0, uint32_t y0,
                          uint32_t x1, uint32_t y1, uint8_t **buf,
                          nrt_Error *error)
{
    OpenJPEGReaderImpl *impl = (OpenJPEGReaderImpl*) data;

    opj_stream_t *stream = NULL;
    opj_image_t *image = NULL;
    opj_codec_t *codec = NULL;
    uint64_t bufSize;
    uint64_t offset = 0;
    uint32_t componentBytes, nComponents;

    if (!OpenJPEG_setup(impl, &stream, &codec, error))
    {
        goto CATCH_ERROR;
    }

    /* unfortunately, we need to read the header every time ... */
    if (!opj_read_header(stream, codec, &image))
    {
        /*nrt_Error_init(error, "Error reading header", NRT_CTXT, NRT_ERR_UNK);*/
        goto CATCH_ERROR;
    }

    if (x1 == 0)
        x1 = j2k_Container_getWidth(impl->container, error);
    if (y1 == 0)
        y1 = j2k_Container_getHeight(impl->container, error);

    /* only decode what we want */
    if (!opj_set_decode_area(codec, image, x0, y0, x1, y1))
    {
        /*nrt_Error_init(error, "Error decoding area", NRT_CTXT, NRT_ERR_UNK);*/
        goto CATCH_ERROR;
    }

    nComponents = j2k_Container_getNumComponents(impl->container, error);
    componentBytes = (j2k_Container_getPrecision(impl->container, error) - 1) / 8 + 1;
    bufSize = (uint64_t)(x1 - x0) * (y1 - y0) * componentBytes * nComponents;
    if (buf && !*buf)
    {
        *buf = (uint8_t*)J2K_MALLOC(bufSize);
        if (!*buf)
        {
            nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                           NRT_ERR_MEMORY);
            goto CATCH_ERROR;
        }
    }

    {
        int keepGoing;
        OPJ_UINT32 tileIndex, reqSize;
        OPJ_INT32 tileX0, tileY0, tileX1, tileY1;

        do
        {
            if (!opj_read_tile_header(codec, stream, &tileIndex, &reqSize, &tileX0,
                                      &tileY0, &tileX1, &tileY1, &nComponents,
                                      &keepGoing))
            {
                /*nrt_Error_init(error, "Error reading tile header", NRT_CTXT,
                  NRT_ERR_UNK);*/
                goto CATCH_ERROR;
            }

            if (keepGoing)
            {
                if ((buf != NULL) && !opj_decode_tile_data(codec, tileIndex, (*buf + offset),
                                          reqSize, stream))
                {
                    /*nrt_Error_init(error, "Error decoding tile", NRT_CTXT,
                      NRT_ERR_UNK);*/
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

J2KPRIV( j2k_Container*)
OpenJPEGReader_getContainer(J2K_USER_DATA *data, nrt_Error *error)
{
    (void)error;

    OpenJPEGReaderImpl *impl = (OpenJPEGReaderImpl*) data;
    return impl->container;
}

J2KPRIV(void)
OpenJPEGReader_destruct(J2K_USER_DATA * data)
{
    if (data)
    {
        OpenJPEGReaderImpl* const impl = (OpenJPEGReaderImpl*) data;
        if (impl->io && impl->ownIO)
        {
            nrt_IOInterface_destruct(&impl->io);
            impl->io = NULL;
        }
        if(impl->container)
        {
            j2k_Container_destruct(&impl->container);
            impl->container = NULL;
        }
        J2K_FREE(data);
    }
}

/******************************************************************************/
/* WRITER                                                                     */
/******************************************************************************/

J2KPRIV( NRT_BOOL)
OpenJPEGWriter_setTile(J2K_USER_DATA *data, uint32_t tileX, uint32_t tileY,
                       const uint8_t *buf, uint32_t tileSize,
                       nrt_Error *error)
{
    OpenJPEGWriterImpl *impl = (OpenJPEGWriterImpl*) data;
    NRT_BOOL rc = NRT_SUCCESS;
    uint32_t xTiles, yTiles, tileIndex, width, height, tileWidth, tileHeight;
    uint32_t thisTileWidth, thisTileHeight, thisTileSize, nComponents, nBytes;
    uint8_t* newTileBuf = NULL;

    xTiles = j2k_Container_getTilesX(impl->container, error);
    yTiles = j2k_Container_getTilesY(impl->container, error);
    width  = j2k_Container_getWidth(impl->container, error);
    height = j2k_Container_getHeight(impl->container, error);
    tileWidth  = j2k_Container_getTileWidth(impl->container, error);
    tileHeight = j2k_Container_getTileHeight(impl->container, error);
    nComponents = j2k_Container_getNumComponents(impl->container, error);
    nBytes = (j2k_Container_getPrecision(impl->container, error) - 1) / 8 + 1;
    tileIndex = tileY * xTiles + tileX;

    memset(error->message, 0, NRT_MAX_EMESSAGE);
    if(!opj_set_error_handler(impl->codec,
                              OpenJPEG_errorHandler,
                              error))
    {
        nrt_Error_init(error, "Unable to set OpenJPEG error handler", NRT_CTXT,
                       NRT_ERR_UNK);
        goto CATCH_ERROR;
    }

    /* Check for edge case where we may have partial tile */
    thisTileWidth = tileWidth;
    thisTileHeight = tileHeight;
    if (tileX == xTiles - 1 && width % tileWidth != 0)
        thisTileWidth = width % tileWidth;
    if (tileY == yTiles - 1 && height % tileHeight != 0)
        thisTileHeight = height % tileHeight;

    thisTileSize = thisTileWidth * thisTileHeight * nComponents * nBytes;
    if(thisTileSize != tileSize)
        tileSize = thisTileSize;

    if(thisTileWidth < tileWidth)
    {
        /* TODO: The current approach below only works for single band
         *       imagery.  For RGB data, I believe it is stored as all
         *       red, then all green, then all blue, so we would need
         *       a temp buffer rather than reusing the current buffer.
         */
        if (nComponents != 1)
        {
            nrt_Error_init(
                error,
                "Partial tile width not implemented for multi-band",
                NRT_CTXT, NRT_ERR_UNK);
            goto CATCH_ERROR;
        }

        /* We have a tile that is wider than it "should" be
         * Need to create smaller buffer to pass to write function
         */
        {
            OPJ_UINT32 ii;
            size_t srcOffset = 0;
            size_t destOffset = 0;
            const size_t srcStride = ((size_t)tileWidth) * nBytes;
            const size_t destStride = ((size_t)thisTileWidth) * nBytes;

            newTileBuf = (uint8_t*) J2K_MALLOC(thisTileSize);
            if(!newTileBuf)
            {
                nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
                goto CATCH_ERROR;
            }

            for(ii = 0; ii < thisTileHeight; ++ii, srcOffset += srcStride,
                    destOffset += destStride)
                memcpy(newTileBuf + destOffset, buf + srcOffset, destStride);

            buf = newTileBuf;
        }
    }

    if (!opj_write_tile(impl->codec,
                        tileIndex,
                        (OPJ_BYTE* )buf,
                        tileSize,
                        impl->stream))
    {
        nrt_Error ioError;
        const nrt_Off currentPos = nrt_IOInterface_tell(impl->compressed, &ioError);
        const nrt_Off ioSize = nrt_IOInterface_getSize(impl->compressed, &ioError);
        if (NRT_IO_SUCCESS(currentPos) &&
            NRT_IO_SUCCESS(ioSize) &&
            currentPos + OPENJPEG_STREAM_SIZE >= ioSize)
        {
            /* The write failed because implStreamWrite() failed because
             * nrt_IOInterface_write() failed because we didn't have enough
             * room left in the buffer that we copy to prior to flushing out
             * to disk in OpenJPEGWriter_write().  The buffer is sized to the
             * uncompressed image size, so this only occurs if the compressed
             * image is actually larger than the uncompressed size.
             * TODO: Handle resizing the buffer on the fly when this occurs
             * inside implStreamWrite().  Long-term if we're able to thread
             * per tile, we won't have to reallocate nearly as much.
             */
            nrt_Error_init(error,
                           "Error writing tile: Compressed image is larger "
                            "than uncompressed image",
                            NRT_CTXT,
                            NRT_ERR_INVALID_OBJECT);
        }

        /*nrt_Error_init(error, "Error writing tile", NRT_CTXT,
          NRT_ERR_INVALID_OBJECT);*/
        goto CATCH_ERROR;
    }

    goto CLEANUP;

    CATCH_ERROR:
    {
        rc = NRT_FAILURE;
    }

    CLEANUP:
    {
        if(newTileBuf)
            J2K_FREE(newTileBuf);
    }

    return rc;
}

J2KPRIV( NRT_BOOL)
OpenJPEGWriter_write(J2K_USER_DATA *data, nrt_IOInterface *io, nrt_Error *error)
{
    OpenJPEGWriterImpl *impl = (OpenJPEGWriterImpl*) data;
    NRT_BOOL rc = NRT_SUCCESS;
    size_t compressedSize;

    memset(error->message, 0, NRT_MAX_EMESSAGE);
    if(!opj_set_error_handler(impl->codec,
                              OpenJPEG_errorHandler,
                              error))
    {
        nrt_Error_init(error, "Unable to set OpenJPEG error handler", NRT_CTXT,
                       NRT_ERR_UNK);
        goto CATCH_ERROR;
    }

    if (!opj_end_compress(impl->codec, impl->stream))
    {
        /*nrt_Error_init(error, "Error ending compression", NRT_CTXT,
          NRT_ERR_INVALID_OBJECT);*/
        goto CATCH_ERROR;
    }

    /* just copy/write the compressed data to the output IO */
    compressedSize = (size_t)nrt_IOInterface_tell(impl->compressed, error);
    if (!nrt_IOInterface_write(io, impl->compressedBuf, compressedSize, error))
    {
        nrt_Error_init(error, "Error writing data", NRT_CTXT,
                       NRT_ERR_INVALID_OBJECT);
        goto CATCH_ERROR;
    }

    goto CLEANUP;

    CATCH_ERROR:
    {
        rc = NRT_FAILURE;
    }

    CLEANUP:
    {
    }

    return rc;
}

J2KPRIV( j2k_Container*)
OpenJPEGWriter_getContainer(J2K_USER_DATA *data, nrt_Error *error)
{
    (void)error;

    OpenJPEGWriterImpl *impl = (OpenJPEGWriterImpl*) data;
    return impl->container;
}

J2KPRIV(void)
OpenJPEGWriter_destruct(J2K_USER_DATA * data)
{
    if (data)
    {
        OpenJPEGWriterImpl* const impl = (OpenJPEGWriterImpl*) data;
        OpenJPEG_cleanup(&impl->stream, &impl->codec, &impl->image);
        nrt_IOInterface_destruct(&impl->compressed);
        J2K_FREE(data);
    }
}

/******************************************************************************/
/******************************************************************************/
/* PUBLIC FUNCTIONS                                                           */
/******************************************************************************/
/******************************************************************************/

J2KAPI(j2k_Reader*) j2k_Reader_open(const char *fname, nrt_Error *error)
{
    j2k_Reader *reader = NULL;
    nrt_IOInterface *io = NULL;

    if (!fname)
    {
        nrt_Error_init(error, "NULL filename", NRT_CTXT,
                       NRT_ERR_INVALID_OBJECT);
        goto CATCH_ERROR;
    }

    if (!(io = nrt_IOHandleAdapter_open(fname, NRT_ACCESS_READONLY,
                                        NRT_OPEN_EXISTING, error)))
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

J2KAPI(j2k_Reader*) j2k_Reader_openIO(nrt_IOInterface *io, nrt_Error *error)
{
    OpenJPEGReaderImpl *impl = NULL;
    j2k_Reader *reader = NULL;

    /* create the Reader interface */
    impl = (OpenJPEGReaderImpl *) J2K_MALLOC(sizeof(OpenJPEGReaderImpl));
    if (!impl)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(impl, 0, sizeof(OpenJPEGReaderImpl));

    reader = (j2k_Reader *) J2K_MALLOC(sizeof(j2k_Reader));
    if (!reader)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(reader, 0, sizeof(j2k_Reader));
    reader->data = impl;
    reader->iface = &ReaderInterface;

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

J2KAPI(j2k_Writer*) j2k_Writer_construct(j2k_Container *container,
                                         j2k_WriterOptions *writerOps,
                                         nrt_Error *error)
{
    j2k_Writer *writer = NULL;
    OpenJPEGWriterImpl *impl = NULL;

    writer = (j2k_Writer*) J2K_MALLOC(sizeof(j2k_Container));
    if (!writer)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(writer, 0, sizeof(j2k_Writer));

    /* create the Writer interface */
    impl = (OpenJPEGWriterImpl *) J2K_MALLOC(sizeof(OpenJPEGWriterImpl));
    if (!impl)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(impl, 0, sizeof(OpenJPEGWriterImpl));
    impl->container = container;

    if (!(OpenJPEG_initImage(impl, writerOps, error)))
    {
        goto CATCH_ERROR;
    }

    writer->data = impl;
    writer->iface = &WriterInterface;

    return writer;

    CATCH_ERROR:
    {
        if (writer)
        {
            j2k_Writer_destruct(&writer);
        }
        return NULL;
    }
}

#endif
