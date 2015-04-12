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

#include "nitf/SegmentReader.h"

NITFAPI(NITF_BOOL) nitf_SegmentReader_read(nitf_SegmentReader *
        segmentReader,
        NITF_DATA * buffer,
        size_t count,
        nitf_Error * error)
{
    NITF_BOOL ret;              /* Return value */

    /*   Check for request out of bounds */
    if (count + segmentReader->virtualOffset > segmentReader->dataLength)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "Seek offset out of bounds");
        return (NITF_FAILURE);
    }

    /*
       Do the read via IOHandle_read. If the virtaul offset is 0 seek to the
       baseOffset first. This handles the initial read. If the offset is not
       0 assume the file is positioned to the correct place

     */
    if (segmentReader->virtualOffset == 0)
        if (!NITF_IO_SUCCESS(nitf_IOInterface_seek(segmentReader->input,
                                                   segmentReader->baseOffset,
                                                   NITF_SEEK_SET, error)))
            return (NITF_FAILURE);

    ret = nitf_IOInterface_read
          (segmentReader->input, (char *) buffer, count, error);
    segmentReader->virtualOffset += count;
    return ret;
}


NITFAPI(nitf_Off) nitf_SegmentReader_seek(nitf_SegmentReader * segmentReader,
                                       nitf_Off offset,
                                       int whence, nitf_Error * error)
{
    nitf_Uint64 baseOffset;     /* Bas offset to the data */
    nitf_Off actualPosition;       /* Real file position (no base offset) */

    baseOffset = segmentReader->baseOffset;

    /*   Check for request out of bounds */
    switch (whence)
    {
        case NITF_SEEK_SET:
            if ((offset > segmentReader->dataLength) || (offset < 0))
            {
                nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                                 "Seek offset out of bounds\n");
                return ((nitf_Off) - 1);
            }
            actualPosition = offset + baseOffset;
            break;
        case NITF_SEEK_CUR:
            if ((offset + segmentReader->virtualOffset >
                    segmentReader->dataLength)
                    || (offset + segmentReader->virtualOffset < 0))
            {
                nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                                 "Seek offset out of bounds\n");
                return ((nitf_Off) - 1);
            }
            actualPosition =
                offset + segmentReader->virtualOffset + baseOffset;
            break;
        case NITF_SEEK_END:
            if ((offset + segmentReader->dataLength >
                    segmentReader->dataLength)
                    || (offset + segmentReader->dataLength < 0))
            {
                nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                                 "Seek offset out of bounds\n");
                return ((nitf_Off) - 1);
            }
            actualPosition = offset + segmentReader->dataLength + baseOffset;
            break;
        default:
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                             "Invalid seek\n");
            return ((nitf_Off) - 1);
    }

    actualPosition =
        nitf_IOInterface_seek(segmentReader->input,
                              actualPosition, NITF_SEEK_SET, error);

    if (!NITF_IO_SUCCESS(actualPosition))
        return (actualPosition);

    segmentReader->virtualOffset =
        actualPosition - segmentReader->baseOffset;

    return segmentReader->virtualOffset;
}


NITFAPI(nitf_Off) nitf_SegmentReader_tell(nitf_SegmentReader * segmentReader,
                                       nitf_Error * error)
{
    return (segmentReader->virtualOffset);
}


NITFAPI(nitf_Off) nitf_SegmentReader_getSize(nitf_SegmentReader *
        segmentReader,
        nitf_Error * error)
{
    return (segmentReader->dataLength);
}


NITFAPI(void) nitf_SegmentReader_destruct(nitf_SegmentReader **
        segmentReader)
{
    if ((*segmentReader) != NULL)
        NITF_FREE(*segmentReader);
    return;
}
