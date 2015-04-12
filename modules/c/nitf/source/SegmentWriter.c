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

#include "nitf/SegmentWriter.h"

#define READ_SIZE 8192



/*
 *  Private implementation struct
 */
typedef struct _SegmentWriterImpl
{
    nitf_SegmentSource *segmentSource;
} SegmentWriterImpl;



NITFPRIV(void) SegmentWriter_destruct(NITF_DATA * data)
{
    SegmentWriterImpl *impl = (SegmentWriterImpl *) data;

    if (impl)
    {
        if (impl->segmentSource)
            nitf_SegmentSource_destruct(&impl->segmentSource);
        NITF_FREE(impl);
    }
}


NITFPRIV(NITF_BOOL) SegmentWriter_write(NITF_DATA * data,
                                        nitf_IOInterface* io, 
                                        nitf_Error * error)
{
    size_t size, bytesLeft;
    size_t readSize = READ_SIZE;
    size_t bytesToRead = READ_SIZE;
    char* buf = NULL;
    SegmentWriterImpl *impl = (SegmentWriterImpl *) data;
    
    size = (*impl->segmentSource->iface->getSize)(impl->segmentSource->data, error);
    bytesLeft = size;

    buf = (char*) NITF_MALLOC(readSize);
    if (!buf)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    while (bytesLeft > 0)
    {
        if (bytesLeft < readSize)
            bytesToRead = (size_t)bytesLeft;
        else
            bytesToRead = readSize;

        /* read the bytes */
        if (!(*(impl->segmentSource->iface->read))
                (impl->segmentSource->data, buf, bytesToRead, error))
        {
            goto CATCH_ERROR;
        }

        /* write them */
        if (!nitf_IOInterface_write(io, buf, (int)bytesToRead, error))
            goto CATCH_ERROR;
        bytesLeft -= bytesToRead;
    }

    NITF_FREE(buf);
    return NITF_SUCCESS;

CATCH_ERROR:
    if (buf) NITF_FREE(buf);
    return NITF_FAILURE;
}



NITFAPI(nitf_SegmentWriter *) nitf_SegmentWriter_construct(nitf_Error *error)
{
    static nitf_IWriteHandler iWriteHandler =
    {
        &SegmentWriter_write,
        &SegmentWriter_destruct
    };

    SegmentWriterImpl *impl = NULL;
    nitf_SegmentWriter *segmentWriter = NULL;

    impl = (SegmentWriterImpl *) NITF_MALLOC(sizeof(SegmentWriterImpl));
    if (!impl)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    impl->segmentSource = NULL;

    segmentWriter = (nitf_SegmentWriter *) NITF_MALLOC(sizeof(nitf_SegmentWriter));
    if (!segmentWriter)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    segmentWriter->data = impl;
    segmentWriter->iface = &iWriteHandler;
    return segmentWriter;

  CATCH_ERROR:
    if (impl)
        NITF_FREE(impl);
    return NULL;
}


NITFAPI(NITF_BOOL) nitf_SegmentWriter_attachSource
(
    nitf_SegmentWriter * segmentWriter,
    nitf_SegmentSource * segmentSource,
    nitf_Error * error
)
{
    SegmentWriterImpl *impl = (SegmentWriterImpl *) segmentWriter->data;
    if (impl->segmentSource != NULL)
    {
        nitf_Error_init(error, "Segment source already attached",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NITF_FAILURE;
    }
    impl->segmentSource = segmentSource;
    return NITF_SUCCESS;
}
