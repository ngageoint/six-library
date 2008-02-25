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

#include <import/nitf.h>

/*  This is not the real max, so please dont break this  */
/*  its only a test case!!!                              */
#define MAX_TRE 8192

int main(int argc, char **argv)
{
    nitf_Error error;
    nitf_IOHandle io;
    nitf_Extensions* ext;
    nitf_List* l;
    off_t fileSize;

    if (argc != 2)
    {
        printf("Usage: %s <dummy-extensions>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    io = nitf_IOHandle_create(argv[1],
                              NITF_ACCESS_READONLY,
                              NITF_OPEN_EXISTING,
                              &error);
    if ( NITF_INVALID_HANDLE(io) )
    {
        nitf_Error_print(&error, stdout, "Exiting (0) ...");
        exit(EXIT_FAILURE);
    }
    fileSize = nitf_IOHandle_getSize(io, &error);
    printf("I will read [%d] bytes from \"%s\"\n", (int)fileSize, argv[1]);

    ext = nitf_Extensions_construct(&error);
    if (!ext)
    {
        nitf_Error_print(&error, stdout, "Exiting (1)...");
    }
    while (fileSize > 0)
    {

        char treName[7];
        char treLength[6];
        int intLength;
        memset(treName, 0, 7);
        memset(treLength, 0, 6);



        if (! NITF_IO_SUCCESS(nitf_IOHandle_read(io, treName, 6, &error)) )
        {
            nitf_Error_print(&error, stdout, "Skipping (2) ...");
            break;
        }
        fileSize -= 6;

        printf("Collected tre tag [%s]\n", treName );
        if (! NITF_IO_SUCCESS(nitf_IOHandle_read(io, treLength, 5, &error)))
        {
            nitf_Error_print(&error, stdout, "Exiting (3) ...");
            exit(EXIT_FAILURE);
        }

        fileSize -= 5;

        printf("Collected tre length [%s]\n", treLength);
        intLength = atoi(treLength);
        if (intLength)
        {
            off_t treOff = nitf_IOHandle_tell(io, &error);
            nitf_TRE* tre = nitf_TRE_construct(treName, NULL, intLength, &error);
            char *data = NULL;

            assert(treOff);
            if (!tre)
            {
                nitf_Error_print(&error, stdout, "Exiting (4) ...");
            }

            assert(intLength < MAX_TRE);

            /*  malloc the space for the raw data */
            data = (char*)NITF_MALLOC( tre->length );
            if (!data)
            {
                nitf_Error_fprintf(&error,
                                   stdout, "[%s] malloc failed, Exiting (6) ...",
                                   treName);
                exit(EXIT_FAILURE);
            }
            memset(data, 0, tre->length);

            if (! NITF_IO_SUCCESS( nitf_IOHandle_read(io,
                                   data,
                                   tre->length,
                                   &error) ) )
            {
                nitf_Error_print(&error, stdout, "Exiting (5) ...");
                exit(EXIT_FAILURE);
            }
            printf("Collected data from tag [%s]\n", data);
            fileSize -= intLength;
            if (!nitf_Extensions_insert(ext, treName, tre, &error))
            {

                nitf_Error_fprintf(&error,
                                   stdout, "[%s] Exiting (6) ...",
                                   treName);
                exit(EXIT_FAILURE);
            }

            /* free data */
        }

    }
    nitf_HashTable_print(ext->hash);
    l = nitf_Extensions_get(ext, "DANTRE");
    if (l)
    {
        nitf_ListIterator iter = nitf_List_begin(l);
        nitf_ListIterator end  = nitf_List_end(l);
        printf("Found at least one DANTRE extension!!\n");
        while ( nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_TRE* tre = (nitf_TRE*)nitf_ListIterator_get(&iter);
            printf("\tTRE tag: [%s]\n", tre->tag);

            nitf_ListIterator_increment(&iter);
        }
    }

    nitf_Extensions_destruct(&ext);
    nitf_IOHandle_close(io);
    return 0;
}
