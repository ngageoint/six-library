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
#include <import/nitf.h>
#include <import/j2k.h>

NRT_BOOL writeFile(nrt_Uint32 x0, nrt_Uint32 y0,
                   nrt_Uint32 x1, nrt_Uint32 y1, nrt_Uint8 *buf,
                   nrt_Uint64 bufSize, const char* prefix, nrt_Error *error)
{
    NRT_BOOL rc = NRT_SUCCESS;
    char filename[NRT_MAX_PATH];
    nrt_IOHandle outHandle;

    NRT_SNPRINTF(filename, NRT_MAX_PATH, "%s-raw-%d_%d__%d_%d.out", prefix, x0,
                 y0, x1, y1);
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
    int argIt = 0, i = 0, num = 0, dump = 0;
    char *fname = NULL;
    nrt_Error error;
    nrt_IOInterface *io = NULL;
    nitf_Reader *reader = NULL;
    nitf_Record *record = NULL;
    nrt_Uint8 *buf = NULL;

    for (argIt = 1; argIt < argc; ++argIt)
    {
        if (strcmp(argv[argIt], "--dump") == 0)
            dump = 1;
        else if (!fname)
        {
            fname = argv[argIt];
        }
    }

    if (!fname)
    {
        nrt_Error_init(&error, "Usage: [--x0 --y0 --x1 --y1] <j2k-file>",
                       NRT_CTXT, NRT_ERR_INVALID_OBJECT);
        goto CATCH_ERROR;
    }

    if (nitf_Reader_getNITFVersion(fname) == NITF_VER_UNKNOWN)
    {
        nrt_Error_init(&error, "This file does not appear to be a valid NITF",
                       NRT_CTXT, NRT_ERR_INVALID_OBJECT);
        goto CATCH_ERROR;
    }

    if (!(io = nrt_IOHandleAdapter_open(fname, NRT_ACCESS_READONLY, NRT_OPEN_EXISTING,
                                        &error)))
        goto CATCH_ERROR;

    if (!(reader = nitf_Reader_construct(&error)))
        goto CATCH_ERROR;

    if (!(record = nitf_Reader_readIO(reader, io, &error)))
        goto CATCH_ERROR;

    num = nitf_Record_getNumImages(record, &error);
    if (num > 0)
    {
        nitf_ListIterator iter = nitf_List_begin(record->images);
        nitf_ListIterator end = nitf_List_end(record->images);

        for (i = 0; nitf_ListIterator_notEqualTo(&iter, &end); ++i)
        {
            nitf_ImageSegment *segment =
                    (nitf_ImageSegment *) nitf_ListIterator_get(&iter);
            nitf_ImageSubheader *subheader = segment->subheader;

            if (strcmp(subheader->imageCompression->raw, "C8") == 0)
            {
                j2k_Reader *j2kReader = NULL;
                j2k_Container *container = NULL;
                nrt_Uint32 cmpIt, nComponents;
                printf("Image %d contains J2K compressed data\n", (i + 1));
                printf("Offset: %d\n", segment->imageOffset);
                if (!nrt_IOInterface_seek(io, segment->imageOffset,
                                          NRT_SEEK_SET, &error))
                    goto CATCH_ERROR;
                if (!(j2kReader = j2k_Reader_openIO(io, &error)))
                    goto CATCH_ERROR;
                if (!(container = j2k_Reader_getContainer(j2kReader, &error)))
                    goto CATCH_ERROR;


                printf("grid width:\t%d\n", j2k_Container_getGridWidth(container, &error));
                printf("grid height:\t%d\n", j2k_Container_getGridHeight(container, &error));
                printf("tile width:\t%d\n", j2k_Container_getTileWidth(container, &error));
                printf("tile height:\t%d\n", j2k_Container_getTileHeight(container, &error));
                printf("x tiles:\t%d\n", j2k_Container_getTilesX(container, &error));
                printf("y tiles:\t%d\n", j2k_Container_getTilesY(container, &error));
                printf("image type:\t%d\n", j2k_Container_getImageType(container, &error));

                nComponents = j2k_Container_getNumComponents(container, &error);
                printf("components:\t%d\n", nComponents);

                for(cmpIt = 0; cmpIt < nComponents; ++cmpIt)
                {
                    printf("===component %d===\n", (cmpIt + 1));
                    j2k_Component *c = j2k_Container_getComponent(container, cmpIt, &error);
                    printf("width:\t\t%d\n", j2k_Component_getWidth(c, &error));
                    printf("height:\t\t%d\n", j2k_Component_getHeight(c, &error));
                    printf("precision:\t%d\n", j2k_Component_getPrecision(c, &error));
                    printf("x0:\t\t%d\n", j2k_Component_getOffsetX(c, &error));
                    printf("y0:\t\t%d\n", j2k_Component_getOffsetY(c, &error));
                    printf("x separation:\t%d\n", j2k_Component_getSeparationX(c, &error));
                    printf("y separation:\t%d\n", j2k_Component_getSeparationY(c, &error));
                    printf("signed:\t\t%d\n", j2k_Component_isSigned(c, &error));
                }

                if (dump)
                {
                    char namePrefix[NRT_MAX_PATH];
                    nrt_Uint32 width, height;
                    nrt_Uint64 bufSize;
                    if (buf)
                    {
                        NRT_FREE(buf);
                        buf = NULL;
                    }
                    width = j2k_Container_getWidth(container, &error);
                    height = j2k_Container_getWidth(container, &error);

                    if ((bufSize = j2k_Reader_readRegion(j2kReader, 0, 0,
                                                         width, height,
                                                         &buf, &error)) == 0)
                    {
                        goto CATCH_ERROR;
                    }

                    NRT_SNPRINTF(namePrefix, NRT_MAX_PATH, "image-%d", (i + 1));
                    if (!writeFile(0, 0, width, height, buf, bufSize,
                                   namePrefix, &error))
                    {
                        goto CATCH_ERROR;
                    }
                }
            }

            nitf_ListIterator_increment(&iter);
        }
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
            nitf_Reader_destruct(&reader);
        if (record)
            nitf_Record_destruct(&record);
        if (io)
            nrt_IOInterface_destruct(&io);
    }
    return rc;

}
