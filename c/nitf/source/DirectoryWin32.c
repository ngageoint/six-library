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

#include "nitf/Directory.h"

#ifdef WIN32


NITF_CXX_GUARD typedef struct _DirectoryWin32
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
NITFAPI(nitf_Directory *) nitf_Directory_construct(nitf_Error * error)
{
    DirectoryWin32 *dir =
        (DirectoryWin32 *) NITF_MALLOC(sizeof(DirectoryWin32));
    if (!dir)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }
    dir->handle = INVALID_HANDLE_VALUE;

    return (nitf_Directory *) dir;
}


/*!
 *  Find the first file in a directory.
 *
 *
 */

NITFAPI(const char *) nitf_Directory_findFirstFile(nitf_Directory * dir,
        const char *path)
{
    DirectoryWin32 *nDir = (DirectoryWin32 *) dir;
    char buffer[NITF_MAX_PATH] = "";
    NITF_SNPRINTF(buffer, NITF_MAX_PATH, "%s\\*", path);
    nDir->handle = FindFirstFile(buffer, &(nDir->fileData));
    if (nDir->handle == INVALID_HANDLE_VALUE)
        return NULL;
    return nDir->fileData.cFileName;
}


NITFAPI(const char *) nitf_Directory_findNextFile(nitf_Directory * dir)
{
    DirectoryWin32 *nDir = (DirectoryWin32 *) dir;
    if (FindNextFile(nDir->handle, &(nDir->fileData)) == 0)
        return NULL;
    return nDir->fileData.cFileName;
}


NITFAPI(void) nitf_Directory_destruct(nitf_Directory ** dir)
{
    if (*dir)
    {
        DirectoryWin32 *nDir = (DirectoryWin32 *) * dir;
        FindClose(nDir->handle);
        nDir->handle = INVALID_HANDLE_VALUE;
        NITF_FREE(*dir);
        *dir = NULL;
    }
}


NITFAPI(NITF_BOOL) nitf_Directory_exists(const char *dir)
{
    DWORD what = GetFileAttributes(dir);
    if (what == 0xFFFFFFFF)
        return NITF_FAILURE;

    if (what == FILE_ATTRIBUTE_DIRECTORY)
        return NITF_SUCCESS;
    return NITF_FAILURE;

}


NITF_CXX_ENDGUARD
#endif
