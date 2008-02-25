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

#include "nitf/SegmentWriter.h"

#define READ_SIZE 8192

NITFPROT(NITF_BOOL) nitf_SegmentWriter_write
(
    nitf_SegmentWriter * writer,
    nitf_Error * error
)
{
    size_t size = (*writer->segmentSource->iface->getSize)(writer->segmentSource->data);
    size_t bytesLeft = size;
    size_t readSize = READ_SIZE;
    size_t bytesToRead = READ_SIZE;
    char* buf;

    buf = (char*) NITF_MALLOC(readSize);
    if (buf == NULL)
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
        if (!(*(writer->segmentSource->iface->read))
                (writer->segmentSource->data, buf, bytesToRead, error))
        {
            goto CATCH_ERROR;
        }

        /* write them */
        if (!nitf_IOHandle_write(writer->outputHandle,
                                 buf, (int)bytesToRead, error))
        {
            goto CATCH_ERROR;
        }
        bytesLeft -= bytesToRead;
    }

    NITF_FREE(buf);

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

NITFAPI(void) nitf_SegmentWriter_destruct(nitf_SegmentWriter ** writer)
{
    if (*writer)
    {
        (*writer)->outputHandle = 0;
        if ((*writer)->segmentSource)
            nitf_SegmentSource_destruct(&(*writer)->segmentSource);
        
        NITF_FREE(*writer);
        *writer = NULL;
    }
}


NITFAPI(NITF_BOOL) nitf_SegmentWriter_attachSource
(
    nitf_SegmentWriter * writer,
    nitf_SegmentSource * segmentSource,
    nitf_Error * error
)
{
    if (writer->segmentSource != NULL)
    {
        nitf_Error_init(error,
                        "Segment source already attached",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NITF_FAILURE;
    }
    writer->segmentSource = segmentSource;
    return NITF_SUCCESS;
}
