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
 * License along with this program;
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "nitf/StreamIOWriteHandler.h"


typedef struct _WriteHandlerImpl
{
    nitf_IOInterface* ioHandle;
    nitf_Uint64       offset;
    nitf_Uint64       bytes;
} WriteHandlerImpl;


#define _STREAM_CHUNK_SIZE 8192

/*
 *  Private read implementation for file source.
 */
NITFPRIV(NITF_BOOL) WriteHandler_write
    (NITF_DATA * data, nitf_IOInterface* output, nitf_Error * error)
{
    WriteHandlerImpl *impl = NULL;
    nitf_Uint64 toWrite;
    nitf_Uint32 bytesThisPass;
    char *buf = NULL;

    /* cast it to the structure we know about */
    impl = (WriteHandlerImpl *) data;

    buf = (char*)NITF_MALLOC(_STREAM_CHUNK_SIZE);
    if (!buf)
    {
        nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),
                NITF_CTXT, NITF_ERR_MEMORY);
        return NITF_FAILURE;
    }

    /* stream the input to the output in chunks */

    /* first, seek to the right spot of the input handle */
    if (!NITF_IO_SUCCESS(
            nitf_IOInterface_seek(
                impl->ioHandle, impl->offset, NITF_SEEK_SET, error
                )
            )
        )
        goto CATCH_ERROR;

    toWrite = impl->bytes;
    while (toWrite > 0)
    {
        bytesThisPass = toWrite >= _STREAM_CHUNK_SIZE ? _STREAM_CHUNK_SIZE :
                (nitf_Uint32) toWrite;

        /* read */
        if (!nitf_IOInterface_read(impl->ioHandle, buf, bytesThisPass, error))
            goto CATCH_ERROR;

        /* write */
        if (!nitf_IOInterface_write(output, buf, bytesThisPass, error))
            goto CATCH_ERROR;

        /* update count */
        toWrite -= bytesThisPass;
    }

    NITF_FREE(buf);

    return NITF_SUCCESS;

  CATCH_ERROR:
    if (buf)
        NITF_FREE(buf);
    return NITF_FAILURE;
}


NITFPRIV(void) WriteHandler_destruct(NITF_DATA * data)
{
    if (data)
    {
        WriteHandlerImpl *impl = (WriteHandlerImpl*)data;
        NITF_FREE(impl);
    }
}


NITFAPI(nitf_WriteHandler*)
nitf_StreamIOWriteHandler_construct(nitf_IOInterface *ioHandle,
                                    nitf_Uint64 offset,
                                    nitf_Uint64 bytes,
                                    nitf_Error *error)
{
    nitf_WriteHandler *writeHandler = NULL;
    WriteHandlerImpl *impl = NULL;

    /* make the interface */
    static nitf_IWriteHandler iWriteHandler = {
        &WriteHandler_write,
        &WriteHandler_destruct
    };

    /* construct the persisent one */
    impl = (WriteHandlerImpl *) NITF_MALLOC(sizeof(WriteHandlerImpl));
    if (!impl)
    {
        nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),
                NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    impl->ioHandle = ioHandle;
    impl->offset = offset;
    impl->bytes = bytes;

    writeHandler =
        (nitf_WriteHandler *) NITF_MALLOC(sizeof(nitf_WriteHandler));

    if (!writeHandler)
    {
        nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),
                NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    writeHandler->data = impl;
    writeHandler->iface = &iWriteHandler;

    /* return successfully */
    return writeHandler;

  CATCH_ERROR:
    if (writeHandler)
        nitf_WriteHandler_destruct(&writeHandler);
    if (impl)
        NITF_FREE(impl);
    return NULL;
}
