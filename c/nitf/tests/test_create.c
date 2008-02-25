/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
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

#include <nitf/IOHandle.h>


int main(int argc, char** argv)
{
    nitf_IOHandle handle;
    nitf_Error    error;
    if (argc != 2)
    {
        printf("Usage %s <file-to-create>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    handle = nitf_IOHandle_create(argv[1], NITF_ACCESS_READONLY, NITF_CREATE, &error);
    if ( NITF_INVALID_HANDLE( handle ) )
    {
        nitf_Error_print(&error, stderr, "Test failed!");
        exit(EXIT_FAILURE);
    }
    printf("Create succeeded.  Check file permissions to make sure they're OK\n");
    nitf_IOHandle_close(handle);
    return 0;
}
