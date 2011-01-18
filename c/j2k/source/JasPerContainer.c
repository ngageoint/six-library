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

#include <import/nrt.h>
#include <jasper/jasper.h>
#include "j2k/Container.h"


typedef struct _IOControl
{
    nrt_IOInterface *io;
    nrt_Off offset;
    nrt_Off length;
    nrt_Off ioSize;
    int isRead;
    nrt_Error error;
}IOControl;

/*
 *  Private implementation struct
 */
typedef struct _JasPerContainerImpl
{
    nrt_Off ioOffset;
    nrt_IOInterface *io;
    nrt_Uint32 width;
    nrt_Uint32 height;
    nrt_Uint32 nComponents;
    nrt_Uint32 bppComponent;
    int ownIO;
} JasPerContainerImpl;


NRTPRIV(int) JasPer_readIO(jas_stream_obj_t *obj, char *buf, int cnt);
NRTPRIV(int) JasPer_writeIO(jas_stream_obj_t *obj, char *buf, int cnt);
NRTPRIV(long) JasPer_seekIO(jas_stream_obj_t *obj, long offset, int origin);
NRTPRIV(int) JasPer_closeIO(jas_stream_obj_t *obj);

static jas_stream_ops_t JasPerOps =
{   JasPer_readIO, JasPer_writeIO, JasPer_seekIO,
    JasPer_closeIO};

NRTPRIV(jas_stream_t*) JasPer_createIO(nrt_IOInterface *io,
        int openMode,
        nrt_Error *error)
{
    jas_stream_t *stream = NULL;
    IOControl *ctrl = NULL;

    if (!(stream = (jas_stream_t*)NRT_MALLOC(sizeof(jas_stream_t))))
    {
        nrt_Error_init(error,
                "Error creating stream object",
                NRT_CTXT,
                NRT_ERR_MEMORY);
        return NULL;
    }
    memset(stream, 0, sizeof(jas_stream_t));

    if (!(ctrl = (IOControl *) NRT_MALLOC(sizeof(IOControl))))
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
    stream->ops_ = &JasPerOps;

    if ((stream->bufbase_ = NRT_MALLOC(JAS_STREAM_BUFSIZE +
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

NRTPRIV(int) JasPer_readIO(jas_stream_obj_t *obj, char *buf, int cnt)
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

NRTPRIV(int) JasPer_writeIO(jas_stream_obj_t *obj, char *buf, int cnt)
{
    nrt_Error error;
    IOControl *ctrl = (IOControl*)obj;

    if (!nrt_IOInterface_write(ctrl->io, buf, (size_t)cnt, &ctrl->error))
    {
        return -1;
    }
    return cnt;
}

NRTPRIV(long) JasPer_seekIO(jas_stream_obj_t *obj, long offset, int origin)
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

NRTPRIV(int) JasPer_closeIO(jas_stream_obj_t *obj)
{
    /* TODO - I don't think we should close it.. */
    return 0;
}


NRTPRIV(void)
JasPer_destroy(jas_stream_t **stream, jas_image_t **image)
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

NRTPRIV( NRT_BOOL)
JasPer_setup(JasPerContainerImpl *impl, jas_stream_t **stream,
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

    return NRT_SUCCESS;

    CATCH_ERROR:
    {
        JasPer_destroy(stream, image);
        return NRT_FAILURE;
    }
}

NRTPRIV( NRT_BOOL)
JasPer_readHeader(JasPerContainerImpl *impl, nrt_Error *error)
{
    jas_stream_t *stream = NULL;
    jas_image_t *image = NULL;
    NRT_BOOL rc = NRT_SUCCESS;

    if (!JasPer_setup(impl, &stream, &image, error))
    {
        goto CATCH_ERROR;
    }

    impl->width = jas_image_width(image);
    impl->height = jas_image_height(image);
    impl->nComponents = jas_image_numcmpts(image);
    impl->bppComponent = (jas_image_cmptprec(image, 0) - 1) / 8 + 1;

    /* JasPer hides tile information inside the jpc decoder, so we do not
     * have access to it here.
     *
     * For now, we will fake the tile information by representing raw J2K as
     * having 1x1 tiles.
     *
     * TODO: in the future if the J2K data is embedded in a NITF file, we could
     * use the NITF metadata to extract tile information.
     */

    goto CLEANUP;

    CATCH_ERROR:
    {
        rc = NRT_FAILURE;
    }

    CLEANUP:
    {
        JasPer_destroy(&stream, &image);
    }
    return rc;
}

NRTPRIV( nrt_Uint32)
JasPer_getTilesX(J2K_USER_DATA *data, nrt_Error *error)
{
    JasPerContainerImpl *impl = (JasPerContainerImpl*) data;
    /* hardcoded, since we do not have access to tile data in raw J2K */
    return 1;
}

NRTPRIV( nrt_Uint32)
JasPer_getTilesY(J2K_USER_DATA *data, nrt_Error *error)
{
    JasPerContainerImpl *impl = (JasPerContainerImpl*) data;
    /* hardcoded, since we do not have access to tile data in raw J2K */
    return 1;
}

NRTPRIV( nrt_Uint32)
JasPer_getTileWidth(J2K_USER_DATA *data, nrt_Error *error)
{
    JasPerContainerImpl *impl = (JasPerContainerImpl*) data;
    /* hardcoded, since we do not have access to tile data in raw J2K */
    return impl->width;
}

NRTPRIV( nrt_Uint32)
JasPer_getTileHeight(J2K_USER_DATA *data, nrt_Error *error)
{
    JasPerContainerImpl *impl = (JasPerContainerImpl*) data;
    /* hardcoded, since we do not have access to tile data in raw J2K */
    return impl->height;
}

NRTPRIV( nrt_Uint32)
JasPer_getWidth(J2K_USER_DATA *data, nrt_Error *error)
{
    JasPerContainerImpl *impl = (JasPerContainerImpl*) data;
    return impl->width;
}

NRTPRIV( nrt_Uint32)
JasPer_getHeight(J2K_USER_DATA *data, nrt_Error *error)
{
    JasPerContainerImpl *impl = (JasPerContainerImpl*) data;
    return impl->height;
}

NRTPRIV( nrt_Uint32)
JasPer_getNumComponents(J2K_USER_DATA *data, nrt_Error *error)
{
    JasPerContainerImpl *impl = (JasPerContainerImpl*) data;
    return impl->nComponents;
}

NRTPRIV( nrt_Uint32)
JasPer_getComponentBytes(J2K_USER_DATA *data, nrt_Error *error)
{
    JasPerContainerImpl *impl = (JasPerContainerImpl*) data;
    return impl->bppComponent;
}


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


NRTPRIV( nrt_Uint64)
JasPer_readRegion(J2K_USER_DATA *data, nrt_Uint32 x0, nrt_Uint32 y0,
                  nrt_Uint32 x1, nrt_Uint32 y1, nrt_Uint8 **buf,
                  nrt_Error *error)
{
    JasPerContainerImpl *impl = (JasPerContainerImpl*) data;
    jas_stream_t *stream = NULL;
    jas_image_t *image = NULL;
    nrt_Uint32 i, cmptIdx;
    nrt_Uint64 bufSize, componentSize;
    nrt_Uint8 *bufPtr = NULL;

    if (!JasPer_setup(impl, &stream, &image, error))
    {
        goto CATCH_ERROR;
    }

    if (x1 == 0)
        x1 = impl->width;
    if (y1 == 0)
        y1 = impl->height;

    componentSize = (nrt_Uint64)(x1 - x0) * (y1 - y0) * impl->bppComponent;
    bufSize = componentSize * impl->nComponents;

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

    bufPtr = *buf;
    for (cmptIdx = 0; cmptIdx < impl->nComponents; ++cmptIdx)
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

        switch(impl->bppComponent)
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
        JasPer_destroy(&stream, &image);
    }
    return bufSize;
}

NRTPRIV( nrt_Uint32)
JasPer_readTile(J2K_USER_DATA *data, nrt_Uint32 tileX, nrt_Uint32 tileY,
                nrt_Uint8 **buf, nrt_Error *error)
{
    JasPerContainerImpl *impl = (JasPerContainerImpl*) data;

    /* TODO - for now, we are treating the image as 1x1 */
    /* might want to return an error instead */
    return (nrt_Uint32)JasPer_readRegion(data, 0, 0, impl->width,
                                         impl->height, buf, error);
}

NRTPRIV(void)
JasPer_destruct(J2K_USER_DATA * data)
{
    if (data)
    {
        JasPerContainerImpl *impl = (JasPerContainerImpl*) data;
        if (impl->ownIO && impl->io)
        {
            nrt_IOInterface_destruct(&impl->io);
            impl->io = NULL;
        }
        NRT_FREE(data);
    }
}

NRTAPI(j2k_Container*) j2k_Container_open(const char *fname, nrt_Error *error)
{
    j2k_Container *container = NULL;
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

    if (!(container = j2k_Container_openIO(io, error)))
        goto CATCH_ERROR;

    ((JasPerContainerImpl*) container->data)->ownIO = 1;

    return container;

    CATCH_ERROR:
    {
        if (io)
            nrt_IOInterface_destruct(&io);
        if (container)
            j2k_Container_destruct(&container);
        return NULL;
    }
}

NRTAPI(j2k_Container*) j2k_Container_openIO(nrt_IOInterface *io,
                                            nrt_Error *error)
{
    static j2k_IContainer containerInterface =
    { /* since we can't access the tiling information, we can't read tiles */
      NULL,
      &JasPer_getTilesX, &JasPer_getTilesY, &JasPer_getTileWidth,
      &JasPer_getTileHeight, &JasPer_getWidth, &JasPer_getHeight,
      &JasPer_getNumComponents, &JasPer_getComponentBytes, &JasPer_readTile,
      &JasPer_readRegion, &JasPer_destruct };

    JasPerContainerImpl *impl = NULL;
    j2k_Container *container = NULL;

    /* Initialize jasper  */
    if (jas_init())
    {
        nrt_Error_init(error, "Error initializing JasPer library",
                       NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    impl = (JasPerContainerImpl *) NRT_MALLOC(sizeof(JasPerContainerImpl));
    if (!impl)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(impl, 0, sizeof(JasPerContainerImpl));

    impl->io = io;
    impl->ioOffset = nrt_IOInterface_tell(io, error);

    if (!JasPer_readHeader(impl, error))
    {
        goto CATCH_ERROR;
    }

    container = (j2k_Container *) NRT_MALLOC(sizeof(j2k_Container));
    if (!container)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    container->data = impl;
    container->iface = &containerInterface;
    return container;

    CATCH_ERROR:
    {
        if (container)
        {
            j2k_Container_destruct(&container);
        }
        else if (impl)
        {
            JasPer_destruct((J2K_USER_DATA*) impl);
        }
        return NULL;
    }
}

#endif
