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

#include "nitf/Directory.h"

typedef struct _DirectoryUnix
{
    DIR *handle;
}
DirectoryUnix;

NITFAPI(nitf_Directory *) nitf_Directory_construct(nitf_Error * error)
{
    DirectoryUnix *dir =
        (DirectoryUnix *) NITF_MALLOC(sizeof(DirectoryUnix));

    if (!dir)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }
    dir->handle = NULL;
    return dir;
}


NITFAPI(const char *) nitf_Directory_findFirstFile(nitf_Directory * dir,
        const char *path)
{
    DirectoryUnix *nDir = (DirectoryUnix *) dir;
    nDir->handle = opendir(path);
    if (nDir->handle == NULL)
        return NULL;
    return nitf_Directory_findNextFile(dir);
}


NITFAPI(const char *) nitf_Directory_findNextFile(nitf_Directory * dir)
{
    DirectoryUnix *nDir = (DirectoryUnix *) dir;
    struct dirent *entry = NULL;
    entry = readdir(nDir->handle);
    if (entry == NULL)
        return NULL;
    return entry->d_name;
}


NITFAPI(void) nitf_Directory_destruct(nitf_Directory ** dir)
{
    if (*dir)
    {
        DirectoryUnix *nDir = (DirectoryUnix *) * dir;
        if (nDir->handle != NULL)
        {
            assert(nDir->handle);
            closedir(nDir->handle);
        }
        NITF_FREE(nDir);
        *dir = NULL;
    }
}


NITFAPI(NITF_BOOL) nitf_Directory_exists(const char *path)
{
    struct stat info;
    if (stat(path, &info) == -1)
        return NITF_FAILURE;
    return (S_ISDIR(info.st_mode));
}
#endif
