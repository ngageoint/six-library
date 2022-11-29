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

#include <import/nrt.h>
#include <import/j2k.h>

NRT_BOOL writeFile(uint32_t x0, uint32_t y0,
                   uint32_t x1, uint32_t y1, uint8_t *buf,
                   uint64_t bufSize, nrt_Error *error)
{
    NRT_BOOL rc = NRT_SUCCESS;
    char filename[NRT_MAX_PATH];
    nrt_IOHandle outHandle;

    NRT_SNPRINTF(filename, NRT_MAX_PATH, "raw-%d_%d__%d_%d.out", x0, y0, x1, y1);
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
    int argIt = 0;
    char *fname = NULL;
    nrt_Error error;
    j2k_Reader *reader = NULL;
    j2k_Container *container = NULL;
    uint64_t bufSize;
    uint32_t x0 = 0;
    uint32_t y0 = 0;
    uint32_t x1 = 0;
    uint32_t y1 = 0;
    uint8_t *buf = NULL;

    for (argIt = 1; argIt < argc; ++argIt)
    {
        if (strcmp(argv[argIt], "--x0") == 0)
        {
            if (argIt >= argc - 1)
                goto CATCH_ERROR;
            x0 = atoi(argv[++argIt]);
        }
        else if (strcmp(argv[argIt], "--y0") == 0)
        {
            if (argIt >= argc - 1)
                goto CATCH_ERROR;
            y0 = atoi(argv[++argIt]);
        }
        else if (strcmp(argv[argIt], "--x1") == 0)
        {
            if (argIt >= argc - 1)
                goto CATCH_ERROR;
            x1 = atoi(argv[++argIt]);
        }
        else if (strcmp(argv[argIt], "--y1") == 0)
        {
            if (argIt >= argc - 1)
                goto CATCH_ERROR;
            y1 = atoi(argv[++argIt]);
        }
        else if (!fname)
        {
            fname = argv[argIt];
        }
    }

    if (!fname)
    {
        printf("Usage: %s [--x0 --y0 --x1 --y1] <j2k-file>\n", argv[0]);
        goto CATCH_ERROR;
    }

    reader = j2k_Reader_open(fname, &error);
    if (!reader)
        goto CATCH_ERROR;
    container = j2k_Reader_getContainer(reader, &error);
    if (!container)
        goto CATCH_ERROR;

    if (x1 == 0)
        x1 = j2k_Container_getWidth(container, &error);
    if (y1 == 0)
        y1 = j2k_Container_getHeight(container, &error);

    if ((bufSize = j2k_Reader_readRegion(reader, x0, y0, x1, y1, &buf,
                                         &error)) == 0)
    {
        goto CATCH_ERROR;
    }

    if (!writeFile(x0, y0, x1, y1, buf, bufSize, &error))
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
