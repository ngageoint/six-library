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

#include "nrt/Directory.h"

#ifdef WIN32

NRT_CXX_GUARD typedef struct _DirectoryWin32
{
    HANDLE handle;
    WIN32_FIND_DATA fileData;
}
DirectoryWin32;

/*!
 *  Create a directory object.  Gives back a directory object
 *  \param error An error to populate on failure
 *  \return A directory object
 */
NRTAPI(nrt_Directory *) nrt_Directory_construct(nrt_Error * error)
{
    DirectoryWin32 *dir = (DirectoryWin32 *) NRT_MALLOC(sizeof(DirectoryWin32));
    if (!dir)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        return NULL;
    }
    dir->handle = INVALID_HANDLE_VALUE;

    return (nrt_Directory *) dir;
}

/*!
 *  Find the first file in a directory.
 *
 *
 */

NRTAPI(const char *) nrt_Directory_findFirstFile(nrt_Directory * dir,
                                                 const char *path)
{
    DirectoryWin32 *nDir = (DirectoryWin32 *) dir;
    char buffer[NRT_MAX_PATH] = "";
    NRT_SNPRINTF(buffer, NRT_MAX_PATH, "%s\\*", path);
    nDir->handle = FindFirstFile(buffer, &(nDir->fileData));
    if (nDir->handle == INVALID_HANDLE_VALUE)
        return NULL;
    return nDir->fileData.cFileName;
}

NRTAPI(const char *) nrt_Directory_findNextFile(nrt_Directory * dir)
{
    DirectoryWin32 *nDir = (DirectoryWin32 *) dir;
    if (FindNextFile(nDir->handle, &(nDir->fileData)) == 0)
        return NULL;
    return nDir->fileData.cFileName;
}

NRTAPI(void) nrt_Directory_destruct(nrt_Directory ** dir)
{
    if (*dir)
    {
        DirectoryWin32 *nDir = (DirectoryWin32 *) * dir;
        FindClose(nDir->handle);
        nDir->handle = INVALID_HANDLE_VALUE;
        NRT_FREE(*dir);
        *dir = NULL;
    }
}

NRTAPI(NRT_BOOL) nrt_Directory_exists(const char *dir)
{
    const DWORD what = GetFileAttributes(dir);
    if (what == INVALID_FILE_ATTRIBUTES)
        return NRT_FAILURE;

    if (what & FILE_ATTRIBUTE_DIRECTORY)
        return NRT_SUCCESS;
    return NRT_FAILURE;

}

NRT_CXX_ENDGUARD
#endif
