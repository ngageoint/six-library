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

#ifdef HAVE_JASPER_H

#include "j2k/Container.h"
#include "j2k/Reader.h"
#include "j2k/Writer.h"

#include <jasper/jasper.h>

/******************************************************************************/
/* TYPES & DECLARATIONS                                                       */
/******************************************************************************/

typedef struct _IOControl
{
    nrt_IOInterface *io;
    nrt_Off offset;
    nrt_Off length;
    nrt_Off ioSize;
    int isRead;
    nrt_Error error;
}IOControl;

typedef struct _JasPerReaderImpl
{
    nrt_Off ioOffset;
    nrt_IOInterface *io;
    int ownIO;
    j2k_Container *container;
} JasPerReaderImpl;

typedef struct _JasPerWriterImpl
{
    j2k_Container *container;
    jas_image_t *image;
} JasPerWriterImpl;

J2KPRIV(int) JasPerIO_read(jas_stream_obj_t *obj, char *buf, int cnt);
J2KPRIV(int) JasPerIO_write(jas_stream_obj_t *obj, char *buf, int cnt);
J2KPRIV(long) JasPerIO_seek(jas_stream_obj_t *obj, long offset, int origin);
J2KPRIV(int) JasPerIO_close(jas_stream_obj_t *obj);

static jas_stream_ops_t IOInterface = {JasPerIO_read, JasPerIO_write,
                                       JasPerIO_seek, JasPerIO_close};


J2KPRIV( nrt_Uint64)     JasPerReader_readTile(J2K_USER_DATA *, nrt_Uint32,
                                               nrt_Uint32, nrt_Uint8 **,
                                               nrt_Error *);
J2KPRIV( nrt_Uint64)     JasPerReader_readRegion(J2K_USER_DATA *, nrt_Uint32,
                                                 nrt_Uint32, nrt_Uint32,
                                                 nrt_Uint32, nrt_Uint8 **,
                                                 nrt_Error *);
J2KPRIV( j2k_Container*) JasPerReader_getContainer(J2K_USER_DATA *, nrt_Error *);
J2KPRIV(void)            JasPerReader_destruct(J2K_USER_DATA *);

static j2k_IReader ReaderInterface = {NULL, &JasPerReader_readTile,
                                      &JasPerReader_readRegion,
                                      &JasPerReader_getContainer,
                                      &JasPerReader_destruct };

J2KPRIV( NRT_BOOL)       JasPerWriter_setTile(J2K_USER_DATA *,
                                              nrt_Uint32, nrt_Uint32,
                                              nrt_Uint8 *, nrt_Uint32,
                                              nrt_Error *);
J2KPRIV( NRT_BOOL)       JasPerWriter_write(J2K_USER_DATA *, nrt_IOInterface *,
                                            nrt_Error *);
J2KPRIV( j2k_Container*) JasPerWriter_getContainer(J2K_USER_DATA *, nrt_Error *);
J2KPRIV(void)            JasPerWriter_destruct(J2K_USER_DATA *);

static j2k_IWriter WriterInterface = {&JasPerWriter_setTile,
                                      &JasPerWriter_write,
                                      &JasPerWriter_getContainer,
                                      &JasPerWriter_destruct };


J2KPRIV( J2K_BOOL) JasPer_setup(JasPerReaderImpl *, jas_stream_t **,
                                jas_image_t **, nrt_Error *);
J2KPRIV(void)      JasPer_cleanup(jas_stream_t **, jas_image_t **);
J2KPRIV( J2K_BOOL) JasPer_readHeader(JasPerReaderImpl *, nrt_Error *);
J2KPRIV( J2K_BOOL) JasPer_initImage(JasPerWriterImpl *, j2k_WriterOptions *,
                                    nrt_Error *);

/******************************************************************************/
/* IO                                                                         */
/******************************************************************************/

J2KPRIV(jas_stream_t*) JasPer_createIO(nrt_IOInterface *io,
        int openMode,
        nrt_Error *error)
{
    jas_stream_t *stream = NULL;
    IOControl *ctrl = NULL;

    if (!(stream = (jas_stream_t*)J2K_MALLOC(sizeof(jas_stream_t))))
    {
        nrt_Error_init(error,
                "Error creating stream object",
                NRT_CTXT,
                NRT_ERR_MEMORY);
        return NULL;
    }
    memset(stream, 0, sizeof(jas_stream_t));

    if (!(ctrl = (IOControl *) J2K_MALLOC(sizeof(IOControl))))
    {
        nrt_Error_init(error, "Error creating control object", NRT_CTXT,
                NRT_ERR_MEMORY);
        return NULL;
    }
    ctrl->io = io;
    ctrl->offset = nrt_IOInterface_tell(io, error);
    ctrl->ioSize = nrt_IOInterface_getSize(io, error);
    ctrl->length = ctrl->ioSize - ctrl->offset;

    stream->openmode_ = openMode;
    stream->flags_ = 0;
    stream->rwcnt_ = 0;
    stream->rwlimit_ = -1;
    stream->obj_ = (void *) ctrl;
    stream->ops_ = &IOInterface;

    if ((stream->bufbase_ = J2K_MALLOC(JAS_STREAM_BUFSIZE +
                            JAS_STREAM_MAXPUTBACK)))
    {
        stream->bufmode_ |= JAS_STREAM_FREEBUF;
        stream->bufsize_ = JAS_STREAM_BUFSIZE;
    }
    else
    {
        /* The buffer allocation has failed.  Resort to unbuffered ops. */
        stream->bufbase_ = stream->tinybuf_;
        stream->bufsize_ = 1;
    }
    stream->bufstart_ = &stream->bufbase_[JAS_STREAM_MAXPUTBACK];
    stream->ptr_ = stream->bufstart_;
    stream->cnt_ = 0;
    stream->bufmode_ |= JAS_STREAM_FULLBUF & JAS_STREAM_BUFMODEMASK;

    return stream;
}

J2KPRIV(int) JasPerIO_read(jas_stream_obj_t *obj, char *buf, int cnt)
{
    IOControl *ctrl = (IOControl*)obj;
    nrt_Off remaining;

    /* JasPer can request more bytes than we have, so we need to compute */
    remaining = ctrl->ioSize - nrt_IOInterface_tell(ctrl->io, &ctrl->error);
    if (remaining < (nrt_Off)cnt)
        cnt = (int)remaining;

    if (!nrt_IOInterface_read(ctrl->io, buf, (size_t)cnt, &ctrl->error))
    {
        return -1;
    }
    return cnt;
}

J2KPRIV(int) JasPerIO_write(jas_stream_obj_t *obj, char *buf, int cnt)
{
    nrt_Error error;
    IOControl *ctrl = (IOControl*)obj;

    if (!nrt_IOInterface_write(ctrl->io, buf, (size_t)cnt, &ctrl->error))
    {
        return -1;
    }
    return cnt;
}

J2KPRIV(long) JasPerIO_seek(jas_stream_obj_t *obj, long offset, int origin)
{
    nrt_Off off;
    int whence = NRT_SEEK_SET;
    IOControl *ctrl = (IOControl*)obj;

    if (origin == SEEK_SET)
    {
        offset = offset + ctrl->offset;
    }
    else if (origin == SEEK_END)
    {
        offset = offset + ctrl->offset + ctrl->length;
    }
    else if (origin == SEEK_CUR)
    {
        whence = NRT_SEEK_CUR;
    }

    if (!(off = nrt_IOInterface_seek(ctrl->io, offset, whence, &ctrl->error)))
    {
        return -1;
    }
    return (long)off;
}

J2KPRIV(int) JasPerIO_close(jas_stream_obj_t *obj)
{
    /* TODO - I don't think we should close it.. */
    return 0;
}


/******************************************************************************/
/* UTILITIES                                                                  */
/******************************************************************************/

J2KPRIV( J2K_BOOL)
JasPer_setup(JasPerReaderImpl *impl, jas_stream_t **stream,
             jas_image_t **image, nrt_Error *error)
{
    int fmtid;
    char *fmtname = NULL;
    jas_image_fmtinfo_t *fmtinfo;

    if (nrt_IOInterface_seek(impl->io, impl->ioOffset, NRT_SEEK_SET, error) < 0)
    {
        goto CATCH_ERROR;
    }

    if (!(*stream = JasPer_createIO(impl->io, JAS_STREAM_READ, error)))
    {
        goto CATCH_ERROR;
    }

    if ((fmtid = jas_image_getfmt(*stream)) < 0)
    {
        nrt_Error_init(error, "Error in JasPer: reading format ID", NRT_CTXT,
                       NRT_ERR_UNK);
        goto CATCH_ERROR;
    }

    if (!(fmtname = jas_image_fmttostr(fmtid)))
    {
        nrt_Error_init(error, "Error in JasPer: invalid format ID", NRT_CTXT,
                       NRT_ERR_UNK);
        goto CATCH_ERROR;
    }
    fmtinfo = jas_image_lookupfmtbyid(fmtid);

    if (!(*image = jas_image_decode(*stream, fmtid, NULL)))
    {
        nrt_Error_initf(error, NRT_CTXT, NRT_ERR_UNK,
                        "Error in JasPer: decoding image in format %s",
                        NRT_CTXT, fmtname);
        goto CATCH_ERROR;
    }

    return J2K_SUCCESS;

    CATCH_ERROR:
    {
        JasPer_cleanup(stream, image);
        return J2K_FAILURE;
    }
}

J2KPRIV(void)
JasPer_cleanup(jas_stream_t **stream, jas_image_t **image)
{
    if (stream && *stream)
    {
        jas_stream_close(*stream);
        *stream = NULL;
    }
    if (image && *image)
    {
        jas_image_destroy(*image);
        *image = NULL;
    }
}

J2KPRIV( NRT_BOOL)
JasPer_readHeader(JasPerReaderImpl *impl, nrt_Error *error)
{
    jas_stream_t *stream = NULL;
    jas_image_t *image = NULL;
    NRT_BOOL rc = NRT_SUCCESS;
    jas_clrspc_t colorSpace;

    if (!JasPer_setup(impl, &stream, &image, error))
    {
        goto CATCH_ERROR;
    }

    if (!impl->container)
    {
        /* initialize the container */
        nrt_Uint32 idx, nComponents, width, height;
        j2k_Component **components = NULL;
        int imageType;

        nComponents = jas_image_numcmpts(image);

        if (!(components = (j2k_Component**)J2K_MALLOC(
                sizeof(j2k_Component*) * nComponents)))
        {
            nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                           NRT_ERR_MEMORY);
            goto CATCH_ERROR;
        }

        for(idx = 0; idx < nComponents; ++idx)
        {
            jas_image_cmpt_t *c = image->cmpts_[idx];
            if (!(components[idx] = j2k_Component_construct(c->width_,
                                                            c->height_,
                                                            c->prec_,
                                                            c->sgnd_,
                                                            c->tlx_,
                                                            c->tly_,
                                                            c->hstep_,
                                                            c->vstep_,
                                                            error)))
            {
                goto CATCH_ERROR;
            }
        }

        colorSpace = jas_image_clrspc(image);
        if (jas_clrspc_fam(colorSpace) == JAS_CLRSPC_FAM_RGB)
            imageType = J2K_TYPE_RGB;
        else if (jas_clrspc_fam(colorSpace) == JAS_CLRSPC_FAM_GRAY)
            imageType = J2K_TYPE_MONO;
        else
            imageType = J2K_TYPE_UNKNOWN;

        width = jas_image_width(image);
        height = jas_image_height(image);

        /* JasPer hides tile information inside the jpc decoder, so we do not
         * have access to it here.
         *
         * For now, we will fake the tile information by representing raw J2K as
         * having 1x1 tiles.
         */
        if (!(impl->container = j2k_Container_construct(width, height,
                                                        nComponents,
                                                        components,
                                                        width, height,
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
        JasPer_cleanup(&stream, &image);
    }
    return rc;
}

J2KPRIV( NRT_BOOL)
JasPer_initImage(JasPerWriterImpl *impl, j2k_WriterOptions *writerOps,
                 nrt_Error *error)
{
    NRT_BOOL rc = NRT_SUCCESS;
    jas_clrspc_t colorSpace;
    jas_image_cmptparm_t *cmptParams = NULL;
    j2k_Component *component = NULL;
    nrt_Uint32 i, nComponents, height, width, nBits;
    int imageType;
    J2K_BOOL isSigned;

    nComponents = j2k_Container_getNumComponents(impl->container, error);
    width = j2k_Container_getWidth(impl->container, error);
    height = j2k_Container_getHeight(impl->container, error);
    nBits = j2k_Container_getPrecision(impl->container, error);
    imageType = j2k_Container_getImageType(impl->container, error);

    if (!(cmptParams = (jas_image_cmptparm_t*)J2K_MALLOC(sizeof(
            jas_image_cmptparm_t) * nComponents)))
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    for(i = 0; i < nComponents; ++i)
    {
        component = j2k_Container_getComponent(impl->container, i, error);
        cmptParams[i].width = j2k_Component_getWidth(component, error);
        cmptParams[i].height = j2k_Component_getHeight(component, error);
        cmptParams[i].prec = j2k_Component_getPrecision(component, error);
        cmptParams[i].tlx = j2k_Component_getOffsetX(component, error);
        cmptParams[i].tly = j2k_Component_getOffsetY(component, error);
        cmptParams[i].hstep = j2k_Component_getSeparationX(component, error);
        cmptParams[i].vstep = j2k_Component_getSeparationY(component, error);
        cmptParams[i].sgnd = j2k_Component_isSigned(component, error);
    }

    if (!(impl->image = jas_image_create(nComponents, cmptParams,
                                         JAS_CLRSPC_UNKNOWN)))
    {

        nrt_Error_init(error, "Error creating JasPer image", NRT_CTXT,
                       NRT_ERR_INVALID_OBJECT);
        goto CATCH_ERROR;
    }

    if (imageType == J2K_TYPE_RGB && nComponents == 3)
    {
        jas_image_setclrspc(impl->image, JAS_CLRSPC_GENRGB);
        jas_image_setcmpttype(impl->image, 0,
                              JAS_IMAGE_CT_COLOR(JAS_IMAGE_CT_RGB_R));
        jas_image_setcmpttype(impl->image, 1,
                              JAS_IMAGE_CT_COLOR(JAS_IMAGE_CT_RGB_G));
        jas_image_setcmpttype(impl->image, 2,
                              JAS_IMAGE_CT_COLOR(JAS_IMAGE_CT_RGB_B));
    }
    else
    {
        jas_image_setclrspc(impl->image, JAS_CLRSPC_GENGRAY);
        for(i = 0; i < nComponents; ++i)
        {
            jas_image_setcmpttype(impl->image, i,
                                  JAS_IMAGE_CT_COLOR(JAS_IMAGE_CT_GRAY_Y));
        }
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

#define PRIV_READ_MATRIX(_SZ, _X0, _Y0, _X1, _Y1) { \
nrt_Uint32 rowIdx, colIdx, rowCt, colCt; \
nrt_Uint##_SZ* data##_SZ = (nrt_Uint##_SZ*)bufPtr; \
jas_matrix_t* matrix##_SZ = NULL; \
rowCt = _Y1 - _Y0; colCt = _X1 - _X0; \
matrix##_SZ = jas_matrix_create(rowCt, colCt); \
if (!matrix##_SZ){ \
    nrt_Error_init(error, "Cannot allocate memory - JasPer jas_matrix_create failed!", \
            NRT_CTXT, NRT_ERR_MEMORY); \
    return 0; \
} \
jas_image_readcmpt (image, cmptIdx, _X0, _Y0, colCt, rowCt, matrix##_SZ); \
for (rowIdx = 0; rowIdx < rowCt; ++rowIdx){ \
    for (colIdx = 0; colIdx < colCt; ++colIdx){ \
        *data##_SZ++ = (nrt_Uint##_SZ)jas_matrix_get(matrix##_SZ, rowIdx, colIdx); \
    } \
} \
jas_matrix_destroy (matrix##_SZ); }


J2KPRIV( nrt_Uint64)
JasPerReader_readRegion(J2K_USER_DATA *data, nrt_Uint32 x0, nrt_Uint32 y0,
                  nrt_Uint32 x1, nrt_Uint32 y1, nrt_Uint8 **buf,
                  nrt_Error *error)
{
    JasPerReaderImpl *impl = (JasPerReaderImpl*) data;
    jas_stream_t *stream = NULL;
    jas_image_t *image = NULL;
    nrt_Uint32 i, cmptIdx, nBytes, nComponents;
    nrt_Uint64 bufSize, componentSize;
    nrt_Uint8 *bufPtr = NULL;

    if (!JasPer_setup(impl, &stream, &image, error))
    {
        goto CATCH_ERROR;
    }

    if (x1 == 0)
        x1 = j2k_Container_getWidth(impl->container, error);
    if (y1 == 0)
        y1 = j2k_Container_getHeight(impl->container, error);

    nBytes = (j2k_Container_getPrecision(impl->container, error) - 1) / 8 + 1;
    componentSize = (nrt_Uint64)(x1 - x0) * (y1 - y0) * nBytes;
    nComponents = j2k_Container_getNumComponents(impl->container, error);
    bufSize = componentSize * nComponents;

    if (buf && !*buf)
    {
        *buf = (nrt_Uint8*)J2K_MALLOC(bufSize);
        if (!*buf)
        {
            nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                           NRT_ERR_MEMORY);
            goto CATCH_ERROR;
        }
    }

    bufPtr = *buf;
    for (cmptIdx = 0; cmptIdx < nComponents; ++cmptIdx)
    {
        nrt_Uint32 nRows, nCols;
        nRows = jas_image_cmptheight(image, cmptIdx) *
                jas_image_cmptvstep(image, cmptIdx);
        nCols = jas_image_cmptwidth(image, cmptIdx) *
                jas_image_cmpthstep(image, cmptIdx);
        if (x1 > nCols || x0 >= x1 || y1 > nRows || y0 >= y1)
        {
            nrt_Error_init(error, "Invalid region requested", NRT_CTXT,
                                   NRT_ERR_INVALID_OBJECT);
            goto CATCH_ERROR;
        }

        switch(nBytes)
        {
        case 1:
        {
            PRIV_READ_MATRIX(8, x0, y0, x1, y1);
            break;
        }
        case 2:
        {
            PRIV_READ_MATRIX(16, x0, y0, x1, y1);
            break;
        }
        case 4:
        {
            PRIV_READ_MATRIX(32, x0, y0, x1, y1);
            break;
        }
        default:
            nrt_Error_init(error, "Unknown pixel size", NRT_CTXT,
                           NRT_ERR_INVALID_OBJECT);
            goto CATCH_ERROR;
        }
        bufPtr += componentSize;
    }

    goto CLEANUP;

    CATCH_ERROR:
    {
        bufSize = 0;
    }

    CLEANUP:
    {
        JasPer_cleanup(&stream, &image);
    }
    return bufSize;
}

J2KPRIV( nrt_Uint64)
JasPerReader_readTile(J2K_USER_DATA *data, nrt_Uint32 tileX, nrt_Uint32 tileY,
                nrt_Uint8 **buf, nrt_Error *error)
{
    JasPerReaderImpl *impl = (JasPerReaderImpl*) data;
    nrt_Uint32 width, height;

    width = j2k_Container_getWidth(impl->container, error);
    height = j2k_Container_getHeight(impl->container, error);

    /* TODO - for now, we are treating the image as 1x1 */
    /* might want to return an error instead */
    return JasPerReader_readRegion(data, 0, 0, width, height, buf, error);
}

J2KPRIV( j2k_Container*)
JasPerReader_getContainer(J2K_USER_DATA *data, nrt_Error *error)
{
    JasPerReaderImpl *impl = (JasPerReaderImpl*) data;
    return impl->container;
}


J2KPRIV(void)
JasPerReader_destruct(J2K_USER_DATA * data)
{
    if (data)
    {
        JasPerReaderImpl *impl = (JasPerReaderImpl*) data;
        if (impl->ownIO && impl->io)
        {
            nrt_IOInterface_destruct(&impl->io);
            impl->io = NULL;
        }
        if (impl->container)
        {
            j2k_Container_destruct(&impl->container);
        }
        J2K_FREE(data);
    }
}


/******************************************************************************/
/* WRITER                                                                     */
/******************************************************************************/

#define PRIV_WRITE_MATRIX(_SZ) { \
nrt_Uint32 rowIdx, colIdx, cmpIdx; \
nrt_Uint##_SZ* data##_SZ = (nrt_Uint##_SZ*)buf; \
jas_matrix_t* matrix##_SZ = jas_matrix_create(tileHeight, tileWidth); \
if (!matrix##_SZ){ \
    nrt_Error_init(error, "Cannot allocate memory - JasPer jas_matrix_create failed!", \
            NRT_CTXT, NRT_ERR_MEMORY); \
    goto CATCH_ERROR; \
} \
for (cmpIdx = 0; cmpIdx < nComponents; ++cmpIdx){ \
    for (rowIdx = 0; rowIdx < tileHeight; ++rowIdx){ \
        for (colIdx = 0; colIdx < tileWidth; ++colIdx){ \
            jas_matrix_set(matrix##_SZ, rowIdx, colIdx, (jas_seqent_t)*data##_SZ++); \
        } \
    } \
    if (jas_image_writecmpt (impl->image, cmpIdx, tileX * tileWidth, \
                              tileY * tileHeight, tileWidth, tileHeight, \
                              matrix##_SZ) != 0) { \
        nrt_Error_init(error, "JasPer was unable to write image component", \
                NRT_CTXT, NRT_ERR_UNK); \
        jas_matrix_destroy (matrix##_SZ); \
        goto CATCH_ERROR; \
    } \
} \
jas_matrix_destroy (matrix##_SZ); }


J2KPRIV( NRT_BOOL)
JasPerWriter_setTile(J2K_USER_DATA *data, nrt_Uint32 tileX, nrt_Uint32 tileY,
                     nrt_Uint8 *buf, nrt_Uint32 tileSize, nrt_Error *error)
{
    JasPerWriterImpl *impl = (JasPerWriterImpl*) data;
    NRT_BOOL rc = NRT_SUCCESS;
    nrt_Uint32 i, nComponents, tileHeight, tileWidth, nBits, nBytes;

    nComponents = j2k_Container_getNumComponents(impl->container, error);
    tileWidth = j2k_Container_getTileWidth(impl->container, error);
    tileHeight = j2k_Container_getTileHeight(impl->container, error);
    nBits = j2k_Container_getPrecision(impl->container, error);
    nBytes = (nBits - 1) / 8 + 1;

    switch(nBytes)
    {
    case 1:
        PRIV_WRITE_MATRIX(8);
        break;
    case 2:
        PRIV_WRITE_MATRIX(16);
        break;
    case 4:
        PRIV_WRITE_MATRIX(32);
        break;
    default:
        nrt_Error_init(error, "Invalid pixel size", NRT_CTXT,
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

J2KPRIV( NRT_BOOL)
JasPerWriter_write(J2K_USER_DATA *data, nrt_IOInterface *io, nrt_Error *error)
{
    JasPerWriterImpl *impl = (JasPerWriterImpl*) data;
    jas_stream_t *stream = NULL;
    NRT_BOOL rc = NRT_SUCCESS;

    if (!(stream = JasPer_createIO(io, JAS_STREAM_WRITE| JAS_STREAM_BINARY,
                                   error)))
    {
        goto CATCH_ERROR;
    }

    if (jas_image_encode(impl->image, stream,
            jas_image_strtofmt((char*)"jpc"), NULL) != 0)
    {
        nrt_Error_init(error, "Error encoding image", NRT_CTXT,
                       NRT_ERR_INVALID_OBJECT);
        goto CATCH_ERROR;
    }
    jas_stream_flush(stream);

    if (jas_stream_close(stream) != 0)
    {
        nrt_Error_init(error, "Error closing stream", NRT_CTXT,
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
JasPerWriter_getContainer(J2K_USER_DATA *data, nrt_Error *error)
{
    JasPerWriterImpl *impl = (JasPerWriterImpl*) data;
    return impl->container;
}

J2KPRIV(void)
JasPerWriter_destruct(J2K_USER_DATA * data)
{
    if (data)
    {
        JasPerWriterImpl *impl = (JasPerWriterImpl*) data;

        /* should we clear the formats? */
        /*jas_image_clearfmts();*/
        if (impl->image)
        {
            jas_image_destroy(impl->image);
        }
        /* we'll leave the container alone, unless we decide to clone it */
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

    ((JasPerReaderImpl*) reader->data)->ownIO = 1;

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
    JasPerReaderImpl *impl = NULL;
    j2k_Reader *reader = NULL;

    /* Initialize jasper - TODO is this ok to do more than once? */
    if (jas_init())
    {
        nrt_Error_init(error, "Error initializing JasPer library",
                       NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    /* create the Reader interface */
    impl = (JasPerReaderImpl *) J2K_MALLOC(sizeof(JasPerReaderImpl));
    if (!impl)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(impl, 0, sizeof(JasPerReaderImpl));

    reader = (j2k_Reader*) J2K_MALLOC(sizeof(j2k_Reader));
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

    if (!JasPer_readHeader(impl, error))
    {
        goto CATCH_ERROR;
    }

    /* done */
    return reader;

    CATCH_ERROR:
    {
        if (reader)
        {
            j2k_Reader_destruct(&reader);
        }
        else if (impl)
        {
            JasPerReader_destruct((J2K_USER_DATA*) impl);
        }
        return NULL;
    }
}


J2KAPI(j2k_Writer*) j2k_Writer_construct(j2k_Container *container,
                                         j2k_WriterOptions *options,
                                         nrt_Error *error)
{
    j2k_Writer *writer = NULL;
    JasPerWriterImpl *impl = NULL;

    /* Initialize jasper - TODO is this ok to do more than once? */
    if (jas_init())
    {
        nrt_Error_init(error, "Error initializing JasPer library",
                       NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    writer = (j2k_Writer*) J2K_MALLOC(sizeof(j2k_Container));
    if (!writer)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(writer, 0, sizeof(j2k_Writer));

    /* create the Writer interface */
    impl = (JasPerWriterImpl *) J2K_MALLOC(sizeof(JasPerWriterImpl));
    if (!impl)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(impl, 0, sizeof(JasPerWriterImpl));
    impl->container = container;

    if (!(JasPer_initImage(impl, options, error)))
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
