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

#include "nitf/IOHandle.h"

#ifdef WIN32


NITFAPI(nitf_IOHandle) nitf_IOHandle_create(const char *fname,
        nitf_AccessFlags access,
        nitf_CreationFlags creation,
        nitf_Error * error)
{
    HANDLE handle;

    if (access & NITF_ACCESS_WRITEONLY)
    {
        WIN32_FIND_DATA findData;
        handle = FindFirstFile(fname, &findData);
        if (handle != INVALID_HANDLE_VALUE)
        {
            creation |= TRUNCATE_EXISTING;
            FindClose(handle);
        }
    }

    handle = CreateFile(fname,
                        access,
                        FILE_SHARE_READ, NULL, creation, FILE_ATTRIBUTE_NORMAL, NULL);

    if (handle == INVALID_HANDLE_VALUE)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_OPENING_FILE);
    }
    return handle;
}


NITFAPI(NITF_BOOL) nitf_IOHandle_read(nitf_IOHandle handle,
                                      char *buf, size_t size,
                                      nitf_Error * error)
{
    DWORD bytesRead = 0;        /* Number of bytes read during last read operation */
    DWORD totalBytesRead = 0;   /* Total bytes read thus far */

    /* Make the next read */
    if (!ReadFile(handle, buf, size, &bytesRead, 0))
    {
        goto CATCH_ERROR;
    }

    return NITF_SUCCESS;

    /* An error occured */
CATCH_ERROR:
    {

        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_READING_FROM_FILE);

    }
    return NITF_FAILURE;
}


NITFAPI(NITF_BOOL) nitf_IOHandle_write(nitf_IOHandle handle,
                                       const char *buf, size_t size,
                                       nitf_Error * error)
{
    DWORD actuallyWritten = 0;

    do
    {
        /* Keep track of the bytes we read */
        DWORD bytesThisRead;
        /* Write the data */
        BOOL ok = WriteFile(handle, buf, size, &bytesThisRead, NULL);
        if (!ok)
        {
            /* If the function failed, we want to get the last error */
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                            NITF_ERR_READING_FROM_FILE);
            /* And fail */
            return NITF_FAILURE;
        }
        /* Otherwise, we want to accumulate this read until we are done */
        actuallyWritten += bytesThisRead;
    }
    while (actuallyWritten < size);

    return NITF_SUCCESS;
}


NITFAPI(nitf_Off) nitf_IOHandle_seek(nitf_IOHandle handle,
                                  nitf_Off offset, int whence,
                                  nitf_Error * error)
{
    LARGE_INTEGER largeInt;
    int lastError;
    largeInt.QuadPart = offset;
    largeInt.LowPart = SetFilePointer(handle, largeInt.LowPart,
                                      &largeInt.HighPart, whence);

    lastError = GetLastError();
    if ((largeInt.LowPart == INVALID_SET_FILE_POINTER) &&
            (lastError != NO_ERROR))
    {
        nitf_Error_initf(error,
                         NITF_CTXT, NITF_ERR_SEEKING_IN_FILE,
                         "SetFilePointer failed with error [%d]",
                         lastError);
        return (nitf_Off)-1;
    }
    return (nitf_Off) largeInt.QuadPart;
}


NITFAPI(nitf_Off) nitf_IOHandle_tell(nitf_IOHandle handle, nitf_Error * error)
{
    return nitf_IOHandle_seek(handle, 0, FILE_CURRENT, error);
}


NITFAPI(nitf_Off) nitf_IOHandle_getSize(nitf_IOHandle handle,
                                     nitf_Error * error)
{
    DWORD ret;
    DWORD highOff;
    ret = GetFileSize(handle, &highOff);
    if ((ret == -1))
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_STAT_FILE,
                         "GetFileSize failed with error [%d]",
                         GetLastError());
        return (nitf_Off)-1;
    }
    return ((highOff) << 32) + ret;
}


NITFAPI(void) nitf_IOHandle_close(nitf_IOHandle handle)
{
    CloseHandle(handle);
}
#endif
