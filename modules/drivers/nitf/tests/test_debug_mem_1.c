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

#include <import/nitf.h>

int main(int argc, char **argv)
{
    int i;
    char** str = (char**)NITF_MALLOC(sizeof(argc) * sizeof(char*));
    for (i = 0; i < argc; i++)
    {
        str[i] = (char*)NITF_MALLOC( strlen(argv[i]) + 1);
        strcpy( str[i], argv[i]);
        printf("Copied string [%d]: '%s'\n", i, str[i]);
    }
    for (i = 0; i < argc; i++)
    {
        printf("Deleting string [%d]: '%s'\n", i, str[i]);
        NITF_FREE( str[i] );
    }
    printf("Deleting master...\n");
    NITF_FREE( str );
    printf("Done.\n");
    return 0;
}
