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

#include "nrt/IOHandle.h"

#ifdef WIN32

NRTAPI(nrt_IOHandle) nrt_IOHandle_create(const char *fname,
                                         nrt_AccessFlags access,
                                         nrt_CreationFlags creation,
                                         nrt_Error * error)
{
    HANDLE handle;

    if (access & NRT_ACCESS_WRITEONLY)
    {
        WIN32_FIND_DATA findData;
        handle = FindFirstFile(fname, &findData);
        if (handle != INVALID_HANDLE_VALUE)
        {
            creation |= TRUNCATE_EXISTING;
            FindClose(handle);
        }
    }

    handle =
        CreateFile(fname, access, FILE_SHARE_READ, NULL, creation,
                   FILE_ATTRIBUTE_NORMAL, NULL);

    if (handle == INVALID_HANDLE_VALUE)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_OPENING_FILE);
    }
    return handle;
}

NRTAPI(NRT_BOOL) nrt_IOHandle_read(nrt_IOHandle handle, void* buf, size_t size,
                                   nrt_Error * error)
{
    static const DWORD MAX_READ_SIZE = (DWORD)-1;
    size_t bytesRead = 0;
    size_t bytesRemaining = size;

    while (bytesRead < size)
    {
        /* Determine how many bytes to read */
        const DWORD bytesToRead = (bytesRemaining > MAX_READ_SIZE) ?
            MAX_READ_SIZE : (DWORD)bytesRemaining;

        /* Read from file */
        DWORD bytesThisRead = 0;
        if (!ReadFile(handle,
                      (nrt_Uint8*)buf + bytesRead,
                      bytesToRead,
                      &bytesThisRead,
                      NULL))
        {
            nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                           NRT_ERR_READING_FROM_FILE);
            return NRT_FAILURE;
        }
        else if (bytesThisRead == 0)
        {
            // ReadFile does not fail when finding the EOF --
            // instead it reports 0 bytes read, so this stops an infinite loop
            // from Unexpected EOF
            nrt_Error_init(error, "Unexpected end of file", NRT_CTXT,
                           NRT_ERR_READING_FROM_FILE);
            return NRT_FAILURE;
        }

        bytesRead += bytesThisRead;
        bytesRemaining -= bytesThisRead;
    }

    return NRT_SUCCESS;
}

NRTAPI(NRT_BOOL) nrt_IOHandle_write(nrt_IOHandle handle, const void *buf,
                                    size_t size, nrt_Error * error)
{
    static const DWORD MAX_WRITE_SIZE = (DWORD)-1;
    size_t bytesRemaining = size;
    size_t bytesWritten = 0;

    while (bytesWritten < size)
    {
        /* Determine how many bytes to write */
        const DWORD bytesToWrite = (bytesRemaining > MAX_WRITE_SIZE) ?
            MAX_WRITE_SIZE : (DWORD)bytesRemaining;

        /* Write the data */
        DWORD bytesThisWrite = 0;

        if (!WriteFile(handle,
                       (const nrt_Uint8*)buf + bytesWritten,
                       bytesToWrite,
                       &bytesThisWrite,
                       NULL))
        {
            /* If the function failed, we want to get the last error */
            nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                           NRT_ERR_WRITING_TO_FILE);
            /* And fail */
            return NRT_FAILURE;
        }

        /* Otherwise, we want to accumulate this write until we are done */
        bytesRemaining -= bytesThisWrite;
        bytesWritten += bytesThisWrite;
    }

    return NRT_SUCCESS;
}

NRTAPI(nrt_Off) nrt_IOHandle_seek(nrt_IOHandle handle, nrt_Off offset,
                                  int whence, nrt_Error * error)
{
    LARGE_INTEGER largeInt;
    int lastError;
    largeInt.QuadPart = offset;
    largeInt.LowPart =
        SetFilePointer(handle, largeInt.LowPart, &largeInt.HighPart, whence);

    lastError = GetLastError();
    if ((largeInt.LowPart == INVALID_SET_FILE_POINTER)
        && (lastError != NO_ERROR))
    {
        nrt_Error_initf(error, NRT_CTXT, NRT_ERR_SEEKING_IN_FILE,
                        "SetFilePointer failed with error [%d]", lastError);
        return (nrt_Off) - 1;
    }
    return (nrt_Off) largeInt.QuadPart;
}

NRTAPI(nrt_Off) nrt_IOHandle_tell(nrt_IOHandle handle, nrt_Error * error)
{
    return nrt_IOHandle_seek(handle, 0, FILE_CURRENT, error);
}

NRTAPI(nrt_Off) nrt_IOHandle_getSize(nrt_IOHandle handle, nrt_Error * error)
{
    DWORD ret;
    DWORD highOff;
    nrt_Uint64 off;
    ret = GetFileSize(handle, &highOff);
    if ((ret == -1))
    {
        nrt_Error_initf(error, NRT_CTXT, NRT_ERR_STAT_FILE,
                        "GetFileSize failed with error [%d]", GetLastError());
        return (nrt_Off) - 1;
    }

    off = (nrt_Uint64)highOff;
    return (nrt_Off)((off << 32) + ret);
}

NRTAPI(void) nrt_IOHandle_close(nrt_IOHandle handle)
{
    CloseHandle(handle);
}
#endif
