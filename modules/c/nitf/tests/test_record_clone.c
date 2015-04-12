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

/* This test case just reads in a NITF file, clones it
 * then destructs the clone and original.
 * If you compile the library with the NITF_DEBUG flag set,
 * then you can make sure there are no memory leaks.
 *
 * In the future, we should use the cloned record to test
 * the functionality of a clone.
 */
int main(int argc, char **argv)
{
    /*  Get the error object       */
    nitf_Error     error;

    /*  This is the reader object  */
    nitf_Reader* reader;
    nitf_Record* record;
    nitf_Record* cloneRecord;

    /*  The IO handle  */
    nitf_IOHandle io;
    int num;

    /*  Check argv and make sure we are happy  */
    if ( argc != 2 )
    {
        printf("Usage: %s <nitf-file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    io = nitf_IOHandle_create(argv[1], NITF_ACCESS_READONLY, NITF_OPEN_EXISTING, &error);
    if ( NITF_INVALID_HANDLE( io ) )
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit( EXIT_FAILURE );
    }

    reader = nitf_Reader_construct(&error);
    if (!reader)
    {
        nitf_Error_print(&error, stdout, "Exiting(1) ...");
        exit( EXIT_FAILURE );
    }

    /* read the record */
    record =  nitf_Reader_read(reader, io, &error  );
    if (!record) goto CATCH_ERROR;

    cloneRecord = nitf_Record_clone(record, &error);
    if (!cloneRecord)
    {
        nitf_Error_print(&error, stdout, "Exiting(3) ...");
        exit( EXIT_FAILURE );
    }

    printf("Destructing Cloned Record\n");
    nitf_Record_destruct(&cloneRecord);

    nitf_IOHandle_close(io);
    printf("Destructing Original Record\n");
    nitf_Record_destruct(&record);

    nitf_Reader_destruct(&reader);

    return 0;

CATCH_ERROR:
    printf("!!! we had a problem reading the file !!!\n");
    nitf_Error_print(&error, stdout, "Exiting...");
    exit(EXIT_FAILURE);
}
