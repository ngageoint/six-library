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
 * License along with this program; If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <import/nitf.h>

int main(int argc, char** argv)
{
    nitf_Version version;
    if ( argc != 2 )
    {
        fprintf(stdout, "Usage: %s <nitf-file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    version = nitf_Reader_getNITFVersion(argv[1]);
    switch (version)
    {
        case NITF_VER_20:
            fprintf(stdout, "NITF 2.0\n");
            break;
        case NITF_VER_21:
            fprintf(stdout, "NITF 2.1\n");
            break;
        default:
            fprintf(stdout, "Unknown\n");
    }

    return 0;
}

