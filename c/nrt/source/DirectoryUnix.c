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

#ifndef WIN32

#include "nrt/Directory.h"

typedef struct _DirectoryUnix
{
    DIR *handle;
}
DirectoryUnix;

NRTAPI(nrt_Directory *) nrt_Directory_construct(nrt_Error * error)
{
    DirectoryUnix *dir = (DirectoryUnix *) NRT_MALLOC(sizeof(DirectoryUnix));

    if (!dir)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        return NULL;
    }
    dir->handle = NULL;
    return dir;
}

NRTAPI(const char *) nrt_Directory_findFirstFile(nrt_Directory * dir,
                                                 const char *path)
{
    DirectoryUnix *nDir = (DirectoryUnix *) dir;
    nDir->handle = opendir(path);
    if (nDir->handle == NULL)
        return NULL;
    return nrt_Directory_findNextFile(dir);
}

NRTAPI(const char *) nrt_Directory_findNextFile(nrt_Directory * dir)
{
    DirectoryUnix *nDir = (DirectoryUnix *) dir;
    struct dirent *entry = NULL;
    entry = readdir(nDir->handle);
    if (entry == NULL)
        return NULL;
    return entry->d_name;
}

NRTAPI(void) nrt_Directory_destruct(nrt_Directory ** dir)
{
    if (*dir)
    {
        DirectoryUnix *nDir = (DirectoryUnix *) * dir;
        if (nDir->handle != NULL)
        {
            assert(nDir->handle);
            closedir(nDir->handle);
        }
        NRT_FREE(nDir);
        *dir = NULL;
    }
}

NRTAPI(NRT_BOOL) nrt_Directory_exists(const char *path)
{
    struct stat info;
    if (stat(path, &info) == -1)
        return NRT_FAILURE;
    return (S_ISDIR(info.st_mode));
}
#endif
