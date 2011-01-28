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

J2K_BOOL readFile(const char* filename, char **buf, nrt_Uint64 *bufSize,
                  nrt_Error *error)
{
    J2K_BOOL rc = J2K_TRUE;
    nrt_IOInterface *io = NULL;

    if (!(io = nrt_IOHandleAdapter_open(filename, NRT_ACCESS_READONLY,
                                        NRT_OPEN_EXISTING, error)))
        goto CATCH_ERROR;

    *bufSize = nrt_IOInterface_getSize(io, error);
    if (!(*buf = (char*)J2K_MALLOC(*bufSize)))
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    if (!nrt_IOInterface_read(io, *buf, *bufSize, error))
    {
        goto CATCH_ERROR;
    }

    goto CLEANUP;

    CATCH_ERROR:
    {
        rc = J2K_FALSE;
        if (*buf)
            J2K_FREE(*buf);
        *buf = NULL;
    }
    CLEANUP:
    {
        if (io)
            nrt_IOInterface_destruct(&io);
    }
    return rc;
}



int main(int argc, char **argv)
{
    int rc = 0;
    int argIt = 0;
    char *inName = NULL, *outName = NULL;
    nrt_Error error;
    j2k_Component *component = NULL;
    j2k_Container *container = NULL;
    j2k_Writer *writer = NULL;
    j2k_WriterOptions options;
    char *buf = NULL;
    nrt_Uint64 bufSize;
    nrt_IOInterface *outIO = NULL;
    nrt_Uint32 width, height, precision, tileWidth, tileHeight;

    for (argIt = 1; argIt < argc; ++argIt)
    {
        if (!inName)
        {
            inName = argv[argIt];
        }
        else if (!outName)
        {
            outName = argv[argIt];
        }
    }

    /* hardcoded for now... */
    width = 128;
    height = 128;
    precision = 8;
    tileWidth = width;
    tileHeight = height;

    if (!inName || !outName)
    {
        nrt_Error_initf(&error, NRT_CTXT, NRT_ERR_INVALID_OBJECT,
                        "Usage: %s <raw-input> <output-j2k>", argv[0]);
        goto CATCH_ERROR;
    }

    if (!(component = j2k_Component_construct(width, height, precision,
                                              0, 0, 0, 1, 1, &error)))
    {
        goto CATCH_ERROR;
    }

    if (!(container = j2k_Container_construct(width,
                                              height,
                                              1,
                                              &component,
                                              tileWidth,
                                              tileHeight,
                                              J2K_TYPE_MONO,
                                              &error)))
    {
        goto CATCH_ERROR;
    }

    memset(&options, 0, sizeof(j2k_WriterOptions));
    /* TODO set some options here */

    if (!(writer = j2k_Writer_construct(container, &options, &error)))
    {
        goto CATCH_ERROR;
    }

    if (!readFile(inName, &buf, &bufSize, &error))
    {
        goto CATCH_ERROR;
    }

    if (!j2k_Writer_setTile(writer, 0, 0, (nrt_Uint8*)buf,
                            (nrt_Uint32)bufSize, &error))
    {
        goto CATCH_ERROR;
    }

    if (!(outIO = nrt_IOHandleAdapter_open(outName, NRT_ACCESS_WRITEONLY,
                                           NRT_CREATE, &error)))
        goto CATCH_ERROR;

    if (!j2k_Writer_write(writer, outIO, &error))
        goto CATCH_ERROR;

    goto CLEANUP;

    CATCH_ERROR:
    {
        nrt_Error_print(&error, stdout, "Exiting...");
        rc = 1;
    }
    CLEANUP:
    {
        if (container)
            j2k_Container_destruct(&container);
        if (writer)
            j2k_Writer_destruct(&writer);
        if (buf)
            J2K_FREE(buf);
        if (outIO)
            nrt_IOInterface_destruct(&outIO);
    }
    return rc;

}
