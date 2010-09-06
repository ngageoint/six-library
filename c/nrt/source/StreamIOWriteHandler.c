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
 * License along with this program;
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "nrt/StreamIOWriteHandler.h"


typedef struct _WriteHandlerImpl
{
    nrt_IOInterface* ioHandle;
    nrt_Uint64       offset;
    nrt_Uint64       bytes;
} WriteHandlerImpl;


#define _STREAM_CHUNK_SIZE 8192

/*
 *  Private read implementation for file source.
 */
NRTPRIV(NRT_BOOL) WriteHandler_write
    (NRT_DATA * data, nrt_IOInterface* output, nrt_Error * error)
{
    WriteHandlerImpl *impl = NULL;
    nrt_Uint64 toWrite;
    nrt_Uint32 bytesThisPass;
    char *buf = NULL;

    /* cast it to the structure we know about */
    impl = (WriteHandlerImpl *) data;

    buf = (char*)NRT_MALLOC(_STREAM_CHUNK_SIZE);
    if (!buf)
    {
        nrt_Error_init(error, NRT_STRERROR( NRT_ERRNO ),
                NRT_CTXT, NRT_ERR_MEMORY);
        return NRT_FAILURE;
    }

    /* stream the input to the output in chunks */

    /* first, seek to the right spot of the input handle */
    if (!NRT_IO_SUCCESS(
            nrt_IOInterface_seek(
                impl->ioHandle, impl->offset, NRT_SEEK_SET, error
                )
            )
        )
        goto CATCH_ERROR;

    toWrite = impl->bytes;
    while (toWrite > 0)
    {
        bytesThisPass = toWrite >= _STREAM_CHUNK_SIZE ? _STREAM_CHUNK_SIZE :
                (nrt_Uint32) toWrite;

        /* read */
        if (!nrt_IOInterface_read(impl->ioHandle, buf, bytesThisPass, error))
            goto CATCH_ERROR;

        /* write */
        if (!nrt_IOInterface_write(output, buf, bytesThisPass, error))
            goto CATCH_ERROR;

        /* update count */
        toWrite -= bytesThisPass;
    }

    NRT_FREE(buf);

    return NRT_SUCCESS;

  CATCH_ERROR:
    if (buf)
        NRT_FREE(buf);
    return NRT_FAILURE;
}


NRTPRIV(void) WriteHandler_destruct(NRT_DATA * data)
{
    if (data)
    {
        WriteHandlerImpl *impl = (WriteHandlerImpl*)data;
        NRT_FREE(impl);
    }
}


NRTAPI(nrt_WriteHandler*)
nrt_StreamIOWriteHandler_construct(nrt_IOInterface *ioHandle,
                                    nrt_Uint64 offset,
                                    nrt_Uint64 bytes,
                                    nrt_Error *error)
{
    nrt_WriteHandler *writeHandler = NULL;
    WriteHandlerImpl *impl = NULL;

    /* make the interface */
    static nrt_IWriteHandler iWriteHandler = {
        &WriteHandler_write,
        &WriteHandler_destruct
    };

    /* construct the persisent one */
    impl = (WriteHandlerImpl *) NRT_MALLOC(sizeof(WriteHandlerImpl));
    if (!impl)
    {
        nrt_Error_init(error, NRT_STRERROR( NRT_ERRNO ),
                NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    impl->ioHandle = ioHandle;
    impl->offset = offset;
    impl->bytes = bytes;

    writeHandler =
        (nrt_WriteHandler *) NRT_MALLOC(sizeof(nrt_WriteHandler));

    if (!writeHandler)
    {
        nrt_Error_init(error, NRT_STRERROR( NRT_ERRNO ),
                NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    writeHandler->data = impl;
    writeHandler->iface = &iWriteHandler;

    /* return successfully */
    return writeHandler;

  CATCH_ERROR:
    if (writeHandler)
        nrt_WriteHandler_destruct(&writeHandler);
    if (impl)
        NRT_FREE(impl);
    return NULL;
}
