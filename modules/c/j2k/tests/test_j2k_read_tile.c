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

#include <import/nrt.h>
#include <import/j2k.h>

NRT_BOOL writeFile(j2k_Container *container, nrt_Uint32 tileX,
                   nrt_Uint32 tileY, nrt_Uint8 *buf, nrt_Uint32 bufSize,
                   nrt_Error *error)
{
    NRT_BOOL rc = NRT_SUCCESS;
    char filename[NRT_MAX_PATH];
    nrt_IOHandle outHandle;

    NRT_SNPRINTF(filename, NRT_MAX_PATH, "raw-%d_%d__%dx%d.out", tileX, tileY,
                 j2k_Container_getTileWidth(container, error),
                 j2k_Container_getTileHeight(container, error));
    outHandle = nrt_IOHandle_create(filename, NRT_ACCESS_WRITEONLY, NRT_CREATE,
                                    error);
    if (NRT_INVALID_HANDLE(outHandle))
    {
        goto CATCH_ERROR;
    }
    if (!nrt_IOHandle_write(outHandle, (const char *) buf, bufSize, error))
    {
        goto CATCH_ERROR;
    }
    printf("Wrote file: %s\n", filename);

    goto CLEANUP;

    CATCH_ERROR:
    {
        rc = NRT_FAILURE;
    }
    CLEANUP:
    {
        if (!NRT_INVALID_HANDLE(outHandle))
            nrt_IOHandle_close(outHandle);
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    nrt_Error error;
    nrt_IOHandle handle;
    j2k_Reader *reader = NULL;
    j2k_Container *container = NULL;
    int argIt = 0;
    char *fname = NULL;
    nrt_Uint32 tileX = 0;
    nrt_Uint32 tileY = 0;
    nrt_Uint32 bufSize;
    nrt_Uint8 *buf = NULL;

    for (argIt = 1; argIt < argc; ++argIt)
    {
        if (strcmp(argv[argIt], "--x") == 0)
        {
            if (argIt >= argc - 1)
                goto CATCH_ERROR;
            tileX = atoi(argv[++argIt]);
        }
        else if (strcmp(argv[argIt], "--y") == 0)
        {
            if (argIt >= argc - 1)
                goto CATCH_ERROR;
            tileY = atoi(argv[++argIt]);
        }
        else if (!fname)
        {
            fname = argv[argIt];
        }
    }

    if (!fname)
    {
        printf("Usage: %s [--x --y] <j2k-file>\n", argv[0]);
        goto CATCH_ERROR;
    }

    if (!(reader = j2k_Reader_open(fname, &error)))
        goto CATCH_ERROR;
    if (!(container = j2k_Reader_getContainer(reader, &error)))
        goto CATCH_ERROR;

    if ((bufSize = j2k_Reader_readTile(reader, tileX, tileY, &buf, &error)) == 0)
    {
        goto CATCH_ERROR;
    }

    if (!writeFile(container, tileX, tileY, buf, bufSize, &error))
    {
        goto CATCH_ERROR;
    }

    goto CLEANUP;

    CATCH_ERROR:
    {
        nrt_Error_print(&error, stdout, "Exiting...");
        rc = 1;
    }
    CLEANUP:
    {
        if (reader)
            j2k_Reader_destruct(&reader);
        if (buf)
            NRT_FREE(buf);
    }
    return rc;
}
