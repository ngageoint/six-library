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
    int argIt = 0;
    char *inName = NULL, *outName = NULL;
    nrt_Error error;
    j2k_Container *container = NULL;

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

    if (!inName || !outName)
    {
        printf("Usage: %s <raw-input> <output-j2k>\n", argv[0]);
        goto CATCH_ERROR;
    }

    if (!(container = j2k_Container_construct(128,
                                              128,
                                              1,
                                              8,
                                              128,
                                              128,
                                              J2K_TYPE_MONO,
                                              0,
                                              &error)))
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
        if (container)
            j2k_Container_destruct(&container);
    }
    return rc;

}
