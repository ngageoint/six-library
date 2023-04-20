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

#include <inttypes.h>

#include <import/nrt.h>
#include <import/nitf.h>
#include <import/j2k.h>

NRT_BOOL writeFile(uint32_t x0, uint32_t y0,
                   uint32_t x1, uint32_t y1, uint8_t *buf,
                   uint64_t bufSize, const char* prefix, nrt_Error *error)
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

NRT_BOOL writeJ2K(uint32_t x0, uint32_t y0,
    uint32_t x1, uint32_t y1, uint8_t* buf,
    uint64_t bufSize, j2k_Container* inContainer,
    const char* prefix, nrt_Error* error)
{
    (void)bufSize;

    NRT_BOOL rc = NRT_SUCCESS;
    char outName[NRT_MAX_PATH];

    NRT_SNPRINTF(outName, NRT_MAX_PATH, "%s-raw-%d_%d__%d_%d.j2k", prefix, x0,
        y0, x1, y1);

    //uint32_t width = x1 - x0;
    //uint32_t height = y1 - y0;
    const uint32_t num_x_tiles = j2k_Container_getTilesX(inContainer, error);
    const uint32_t num_y_tiles = j2k_Container_getTilesY(inContainer, error);
    const uint32_t tile_height = j2k_Container_getTileHeight(inContainer, error);
    const uint32_t tile_width = j2k_Container_getTileWidth(inContainer, error);
    const uint32_t num_components = j2k_Container_getNumComponents(inContainer, error);
   
    // TODO: May need to handle this differently for multiple components
    j2k_Component* c = j2k_Container_getComponent(inContainer, 0, error);
    const uint32_t precision = j2k_Component_getPrecision(c, error);

    j2k_Writer* writer = NULL;
    uint32_t tileSize = 0;
    nrt_IOInterface* outIO = NULL;
    uint32_t bytes = 0;

    if (!inContainer)
    {
        goto CATCH_ERROR;
    }

    j2k_WriterOptions options;
    memset(&options, 0, sizeof(j2k_WriterOptions));
    /* TODO set some options here */

    writer = j2k_Writer_construct(inContainer, &options, error);
    if (!writer)
    {
        goto CATCH_ERROR;
    }

    outIO = nrt_IOHandleAdapter_open(outName, NRT_ACCESS_WRITEONLY,
        NRT_CREATE, error);
    if (!outIO)
        goto CATCH_ERROR;

    tileSize = tile_height * tile_width;
    bytes = (precision - 1) / 8 + 1;
    uint32_t x, y;
    for (y = 0; y < num_y_tiles; ++y)
    {
        for (x = 0; x < num_x_tiles; ++x)
        {
            const uint32_t index = y * num_x_tiles + x % num_x_tiles;
            const uint32_t offset = index * tile_height * tile_width * bytes * num_components;

            if (!j2k_Writer_setTile(writer, x, y, (uint8_t*)(buf + offset), tileSize, error))
            {
                goto CATCH_ERROR;
            }
        }
    }

    if (!j2k_Writer_write(writer, outIO, error))
        goto CATCH_ERROR;

    printf("Wrote file: %s\n", outName);

    goto CLEANUP;

    CATCH_ERROR:
    {
        rc = NRT_FAILURE;
    }

CLEANUP:
    {
        // nothing to cleanup
    }

    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    int argIt = 0, i = 0, dump = 0;
    uint32_t num = 0;
    char *fname = NULL;
    nrt_Error error;
    nrt_IOInterface *io = NULL;
    nitf_Reader *reader = NULL;
    nitf_Record *record = NULL;
    uint8_t *buf = NULL;

    for (argIt = 1; argIt < argc; ++argIt)
    {
        if (strcmp(argv[argIt], "--dump") == 0)
        {
            dump = 1;
        }
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

    io = nrt_IOHandleAdapter_open(fname, NRT_ACCESS_READONLY, NRT_OPEN_EXISTING,
                                        &error);
    if (!io)
        goto CATCH_ERROR;

    reader = nitf_Reader_construct(&error);
    if (!reader)
        goto CATCH_ERROR;

    record = nitf_Reader_readIO(reader, io, &error);
    if (!record)
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
                uint32_t cmpIt, nComponents;
                printf("Image %d contains J2K compressed data\n", (i + 1));
                printf("Offset: %" PRIu64 "\n", segment->imageOffset);
                if (!nrt_IOInterface_seek(io, (nrt_Off) segment->imageOffset,
                                          NRT_SEEK_SET, &error))
                    goto CATCH_ERROR;
                j2kReader = j2k_Reader_openIO(io, &error);
                if (!j2kReader)
                    goto CATCH_ERROR;
                container = j2k_Reader_getContainer(j2kReader, &error);
                if (!container)
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
                    j2k_Component* c = j2k_Container_getComponent(container, cmpIt, &error);
                    printf("===component %d===\n", (cmpIt + 1));
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
                    uint32_t width, height;
                    uint64_t bufSize;
                    if (buf)
                    {
                        NRT_FREE(buf);
                        buf = NULL;
                    }
                    width = j2k_Container_getWidth(container, &error);
                    height = j2k_Container_getHeight(container, &error);

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

                    if (!writeJ2K(0, 0, width, height, buf, bufSize,
                        container, namePrefix, &error))
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
