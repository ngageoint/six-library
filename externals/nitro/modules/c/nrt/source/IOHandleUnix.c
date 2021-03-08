/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifdef _MSC_VER // Visual Studio
#pragma warning(disable: 4206) //	nonstandard extension used : translation unit is empty
#endif

#if !(defined(WIN32) || defined(_WIN32))

#include "nrt/IOHandle.h"

NRTAPI(nrt_IOHandle) nrt_IOHandle_create(const char *fname,
                                         nrt_AccessFlags access,
                                         nrt_CreationFlags creation,
                                         nrt_Error * error)
{
    int fd;
    if (access & NRT_ACCESS_WRITEONLY)
        creation |= NRT_TRUNCATE;
    fd = open(fname, access | creation, NRT_DEFAULT_PERM);

    if (fd == -1)
    {
        nrt_Error_init(error, strerror(errno), NRT_CTXT, NRT_ERR_OPENING_FILE);
    }

    return fd;
}

NRTAPI(NRT_BOOL) nrt_IOHandle_read(nrt_IOHandle handle, void* buf, size_t size,
                                   nrt_Error * error)
{
    ssize_t bytesRead = 0;      /* Number of bytes read during last read
                                 * operation */
    size_t totalBytesRead = 0;  /* Total bytes read thus far */
    int i;                      /* iterator */

    /* make sure the user actually wants data */
    if (size <= 0)
        return NRT_SUCCESS;

    /* Interrogate the IO handle */
    for (i = 1; i <= NRT_MAX_READ_ATTEMPTS; i++)
    {
        /* Make the next read */
        bytesRead = read(handle,
                         (uint8_t*)buf + totalBytesRead,
                         size - totalBytesRead);

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
            nrt_Error_init(error, "Unexpected end of file", NRT_CTXT,
                           NRT_ERR_READING_FROM_FILE);
            return NRT_FAILURE;

        default:               /* We made progress */
            totalBytesRead += (size_t) bytesRead;
            break;
        }

        /* Check for success */
        if (totalBytesRead == size)
        {
            return NRT_SUCCESS;
        }

    }                           /* End of for */

    /* We fell out of the for loop (not good) */
    /* goto CATCH_ERROR; */

    /* An error occured */
    CATCH_ERROR:

    nrt_Error_init(error, strerror(errno), NRT_CTXT, NRT_ERR_READING_FROM_FILE);
    return NRT_FAILURE;
}

NRTAPI(NRT_BOOL) nrt_IOHandle_write(nrt_IOHandle handle, const void *buf,
                                    size_t size, nrt_Error * error)
{
    size_t bytesActuallyWritten = 0;

    do
    {
        const ssize_t bytesThisWrite =
            write(handle, (const uint8_t*)buf + bytesActuallyWritten, size);
        if (bytesThisWrite == -1)
        {
            nrt_Error_init(error, strerror(errno), NRT_CTXT,
                           NRT_ERR_WRITING_TO_FILE);
            return NRT_FAILURE;
        }
        bytesActuallyWritten += bytesThisWrite;
    }
    while (bytesActuallyWritten < size);

    return NRT_SUCCESS;
}

NRTAPI(nrt_Off) nrt_IOHandle_seek(nrt_IOHandle handle, nrt_Off offset,
                                  int whence, nrt_Error * error)
{
    nrt_Off off = lseek(handle, offset, whence);
    if (off == (nrt_Off) - 1)
    {
        nrt_Error_init(error, strerror(errno), NRT_CTXT,
                       NRT_ERR_SEEKING_IN_FILE);
    }
    return off;
}

NRTAPI(nrt_Off) nrt_IOHandle_tell(nrt_IOHandle handle, nrt_Error * error)
{
    return nrt_IOHandle_seek(handle, 0, SEEK_CUR, error);
}

NRTAPI(nrt_Off) nrt_IOHandle_getSize(nrt_IOHandle handle, nrt_Error * error)
{
    struct stat buf;
    int rval = fstat(handle, &buf);
    if (rval == -1)
    {
        nrt_Error_init(error, strerror(errno), NRT_CTXT, NRT_ERR_STAT_FILE);
        return rval;
    }
    return buf.st_size;
}

NRTAPI(void) nrt_IOHandle_close(nrt_IOHandle handle)
{
    close(handle);
}
#endif
