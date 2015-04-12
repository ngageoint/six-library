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
    nrt_Uint32 cmpIt, nComponents;
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
