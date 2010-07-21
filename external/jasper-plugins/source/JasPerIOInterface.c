/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#include <stdio.h>
#include <string.h>

#include <import/nitf.h>
#include <jasper/jasper.h>

NITF_CXX_GUARD

NITFPRIV(int) JasPer_read(jas_stream_obj_t *obj, char *buf, int cnt);
NITFPRIV(int) JasPer_write(jas_stream_obj_t *obj, char *buf, int cnt);
NITFPRIV(long) JasPer_seek(jas_stream_obj_t *obj, long offset, int origin);
NITFPRIV(int) JasPer_close(jas_stream_obj_t *obj);

static jas_stream_ops_t JasPerOps = { JasPer_read, JasPer_write, JasPer_seek,
                                      JasPer_close };

NITFAPI(jas_stream_t*) JasPer_wrapIOInterface(nitf_IOInterface *io,
        int openMode,
        nitf_Error *error)
{
    jas_stream_t *stream;

    if (!(stream = (jas_stream_t*)NITF_MALLOC(sizeof(jas_stream_t))))
    {
        nitf_Error_init(error,
                "Error creating stream object",
                NITF_CTXT,
                NITF_ERR_COMPRESSION);
        return NULL;
    }
    stream->openmode_ = openMode;
    stream->flags_ = 0;
    stream->rwcnt_ = 0;
    stream->rwlimit_ = -1;
    stream->obj_ = (void *) io;
    stream->ops_ = &JasPerOps;

    if ((stream->bufbase_ = NITF_MALLOC(JAS_STREAM_BUFSIZE +
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

NITFPRIV(int) JasPer_read(jas_stream_obj_t *obj, char *buf, int cnt)
{
    nitf_Error error;
    nitf_IOInterface *io;
    io = (nitf_IOInterface*)obj;

    if (!nitf_IOInterface_read(io, buf, (size_t)cnt, &error))
    {
        return -1;
    }
    return cnt;
}

NITFPRIV(int) JasPer_write(jas_stream_obj_t *obj, char *buf, int cnt)
{
    nitf_Error error;
    nitf_IOInterface *io;
    io = (nitf_IOInterface*)obj;

    if (!nitf_IOInterface_write(io, buf, (size_t)cnt, &error))
    {
        return -1;
    }
    return cnt;
}

NITFPRIV(long) JasPer_seek(jas_stream_obj_t *obj, long offset, int origin)
{
    nitf_Error error;
    nitf_Off off;
    nitf_IOInterface *io;
    io = (nitf_IOInterface*)obj;

    if (!(off = nitf_IOInterface_seek(io, offset, origin, &error)))
    {
        return -1;
    }
    return (long)off;
}

NITFPRIV(int) JasPer_close(jas_stream_obj_t *obj)
{
    /* TODO - I don't think we should close it.. */
    return 0;
}

NITF_CXX_ENDGUARD
