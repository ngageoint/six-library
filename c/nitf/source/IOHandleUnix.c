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

#ifndef WIN32

#include "nitf/IOHandle.h"

NITFAPI(nitf_IOHandle) nitf_IOHandle_create(const char *fname,
        nitf_AccessFlags access,
        nitf_CreationFlags creation,
        nitf_Error * error)
{
    int fd;
    if (access & NITF_ACCESS_WRITEONLY)
        creation |= NITF_TRUNCATE;
    fd = open(fname, access | creation, NITF_DEFAULT_PERM);

    if (fd == -1)
    {
        nitf_Error_init(error,
                        strerror(errno), NITF_CTXT, NITF_ERR_OPENING_FILE);
    }

    return fd;
}


NITFAPI(NITF_BOOL) nitf_IOHandle_read(nitf_IOHandle handle,
                                      char *buf, size_t size,
                                      nitf_Error * error)
{
    ssize_t bytesRead = 0;          /* Number of bytes read during last read operation */
    size_t totalBytesRead = 0;     /* Total bytes read thus far */
    int i;                      /* iterator */

    /* make sure the user actually wants data */
    if (size <= 0)
        return NITF_SUCCESS;

    /* Interrogate the IO handle */
    for (i = 1; i <= NITF_MAX_READ_ATTEMPTS; i++)
    {
        /* Make the next read */
        bytesRead =
            read(handle, buf + totalBytesRead, size - totalBytesRead);

        switch (bytesRead)
        {
            case -1:               /* Some type of error occured */
                switch (errno)
                {
                    case EINTR:
                    case EAGAIN:       /* A non-fatal error occured, keep trying */
                        break;

                    default:           /* We failed */
                        goto CATCH_ERROR;
                }
                break;

            case 0:                /* EOF (unexpected) */
                nitf_Error_init(error,
                                "Unexpected end of file",
                                NITF_CTXT, NITF_ERR_READING_FROM_FILE);
                return NITF_FAILURE;

            default:               /* We made progress */
                totalBytesRead += (size_t)bytesRead;

        }                       /* End of switch */

        /* Check for success */
        if (totalBytesRead == size)
        {
            return NITF_SUCCESS;
        }

    }                           /* End of for */

    /* We fell out of the for loop (not good) */
    /* goto CATCH_ERROR; */

    /* An error occured */
CATCH_ERROR:

    nitf_Error_init(error,
                    strerror(errno),
                    NITF_CTXT, NITF_ERR_READING_FROM_FILE);
    return NITF_FAILURE;
}


NITFAPI(NITF_BOOL) nitf_IOHandle_write(nitf_IOHandle handle,
                                       const char *buf, size_t size,
                                       nitf_Error * error)
{
    size_t bytesActuallyWritten = 0;

    do
    {
        ssize_t bytesThisRead = write(handle, buf, size);
        if (bytesThisRead == -1)
        {
            nitf_Error_init(error,
                            strerror(errno),
                            NITF_CTXT, NITF_ERR_WRITING_TO_FILE);
            return NITF_FAILURE;
        }
        bytesActuallyWritten += bytesThisRead;
    }
    while (bytesActuallyWritten < size );

    return NITF_SUCCESS;
}


NITFAPI(nitf_Off) nitf_IOHandle_seek(nitf_IOHandle handle,
                                  nitf_Off offset, int whence,
                                  nitf_Error * error)
{
    nitf_Off off = lseek(handle, offset, whence);
    if (off == (nitf_Off) - 1)
    {
        nitf_Error_init(error,
                        strerror(errno),
                        NITF_CTXT, NITF_ERR_SEEKING_IN_FILE);
    }
    return off;
}


NITFAPI(nitf_Off) nitf_IOHandle_tell(nitf_IOHandle handle, nitf_Error * error)
{
    return nitf_IOHandle_seek(handle, 0, SEEK_CUR, error);
}


NITFAPI(nitf_Off) nitf_IOHandle_getSize(nitf_IOHandle handle,
                                     nitf_Error * error)
{
    struct stat buf;
    int rval = fstat(handle, &buf);
    if (rval == -1)
    {
        nitf_Error_init(error,
                        strerror(errno), NITF_CTXT, NITF_ERR_STAT_FILE);
        return rval;
    }
    return buf.st_size;
}


NITFAPI(void) nitf_IOHandle_close(nitf_IOHandle handle)
{
    close(handle);
}
#endif
