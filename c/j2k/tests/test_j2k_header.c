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

int main(int argc, char **argv)
{
    int rc = 0;
    int argIt;
    int useJasPer = 0;
    nrt_Error error;
    j2k_Container *container = NULL;
    j2k_Reader *reader = NULL;
    char *fname = NULL;

    for (argIt = 1; argIt < argc; ++argIt)
    {
        if (!fname)
        {
            fname = argv[argIt];
        }
    }

    if (!fname)
    {
        nrt_Error_init(&error, "Usage: [options] <j2k-file>", NRT_CTXT,
                       NRT_ERR_INVALID_OBJECT);
        goto CATCH_ERROR;
    }

    if (!(reader = j2k_Reader_open(fname, &error)))
        goto CATCH_ERROR;

    if (!(container = j2k_Reader_getContainer(reader, &error)))
        goto CATCH_ERROR;

    printf("tile width:\t\t%d\n", j2k_Container_getTileWidth(container, &error));
    printf("tile height:\t\t%d\n", j2k_Container_getTileHeight(container, &error));
    printf("x tiles:\t\t%d\n", j2k_Container_getTilesX(container, &error));
    printf("y tiles:\t\t%d\n", j2k_Container_getTilesY(container, &error));
    printf("width:\t\t\t%d\n", j2k_Container_getWidth(container, &error));
    printf("height:\t\t\t%d\n", j2k_Container_getHeight(container, &error));
    printf("components:\t\t%d\n", j2k_Container_getNumComponents(container, &error));
    printf("component bytes:\t%d\n", j2k_Container_getComponentBytes(container, &error));
    printf("component bits:\t\t%d\n", j2k_Container_getComponentBits(container, &error));
    printf("image type:\t\t%d\n", j2k_Container_getImageType(container, &error));
    printf("is signed:\t\t%d\n", j2k_Container_isSigned(container, &error));

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
    }
    return rc;

}
