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

#include "nitf/DEReader.h"

/*======================= nitf_DEReader_read ===============================*/

NITFAPI(NITF_BOOL) nitf_DEReader_read(nitf_DEReader * deReader,
                                      NITF_DATA * buffer, size_t count,
                                      nitf_Error * error)
{
    nitf_UserSegment *user;     /* Associated user segment */
    NITF_BOOL ret;              /* Return value */

    user = deReader->user;

    /*   Check for request out of bounds */

    if (count + user->virtualOffset > user->virtualLength)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "Seek offset out of bounds");
        return (NITF_FAILURE);
    }

    /*    Check for type dependent read */

    if (user->iface->read != NULL)
    {
        ret = (*(user->iface->read)) (user, buffer, count, error);
        user->virtualOffset += count;
        return ret;
    }

    /*
       Normal direct read

       Do the read via IOHandle_read. If the virtaul offset is 0 seek to the
       basOffset first. This handles the initial read. If the offset is not
       0 assume the file is positioned to the correct place

     */

    if (user->virtualOffset == 0)
        if (!NITF_IO_SUCCESS(nitf_IOHandle_seek(deReader->inputHandle,
                                                user->baseOffset, NITF_SEEK_SET,
                                                error)))
            return (NITF_FAILURE);

    ret = nitf_IOHandle_read
          (deReader->inputHandle, (char *) buffer, count, error);
    user->virtualOffset += count;

    return ret;
}


/*======================= nitf_DEReader_seek ===============================*/

NITFAPI(off_t) nitf_DEReader_seek(nitf_DEReader * deReader,
                                  off_t offset, int whence,
                                  nitf_Error * error)
{
    nitf_UserSegment *user;     /* Associated user segment */
    nitf_Uint64 baseOffset;     /* Bas offset to user data */
    off_t actualPosition;       /* Real file position (no base offset) */

    user = deReader->user;
    baseOffset = user->baseOffset;

    /*   Check for request out of bounds */

    switch (whence)
    {
        case NITF_SEEK_SET:
            if ((offset > user->virtualLength) || (offset < 0))
            {
                nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                                 "Seek offset out of bounds\n");
                return ((off_t) - 1);
            }
            actualPosition = offset + baseOffset;
            break;
        case NITF_SEEK_CUR:
            if ((offset + user->virtualOffset > user->virtualLength)
                    || (offset + user->virtualOffset < 0))
            {
                nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                                 "Seek offset out of bounds\n");
                return ((off_t) - 1);
            }
            actualPosition = offset + user->virtualOffset + baseOffset;
            break;
        case NITF_SEEK_END:
            if ((offset + user->virtualLength > user->virtualLength)
                    || (offset + user->virtualLength < 0))
            {
                nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                                 "Seek offset out of bounds\n");
                return ((off_t) - 1);
            }
            actualPosition = offset + user->virtualLength + baseOffset;
            break;
        default:
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                             "Invalid seek\n");
            return ((off_t) - 1);

    }

    /*   Check for type dependent seek */

    if (user->iface->seek != NULL)
        return ((*(user->iface->seek)) (user, offset, whence, error));

    actualPosition =
        nitf_IOHandle_seek(deReader->inputHandle,
                           actualPosition, NITF_SEEK_SET, error);
    if (!NITF_IO_SUCCESS(actualPosition))
        return (actualPosition);

    user->virtualOffset = actualPosition - user->baseOffset;

    return user->virtualOffset;
}


/*======================= nitf_DEReader_tell ===============================*/

NITFAPI(off_t) nitf_DEReader_tell(nitf_DEReader * deReader,
                                  nitf_Error * error)
{
    return (deReader->user->virtualOffset);
}


/*======================= nitf_DEReader_getSize ============================*/

NITFAPI(off_t) nitf_DEReader_getSize(nitf_DEReader * deReader,
                                     nitf_Error * error)
{

    return (deReader->user->virtualLength);
}


/*======================= nitf_DEReader_destruct ===========================*/

NITFAPI(void) nitf_DEReader_destruct(nitf_DEReader ** deReader)
{
    if ((*deReader)->user != NULL)
        nitf_UserSegment_destruct(&((*deReader)->user));

    NITF_FREE(*deReader);
    return;
}
