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

/*
    Test program for reading a DE segment

  This program reads a NITF file with a file header and a single DE Segment.

  This reader is for the test_DES_example DE segment and prints out the user
  header and reads and prints the user data

  The calling sequence is:

    test_DES_read inputFile

*/

#include <import/nitf.h>
#include "nitf/UserSegment.h"

extern nitf_IUserSegment test_DES_example;

int main(int argc, char *argv[])
{
    nitf_Reader *reader;        /* Reader object */
    nitf_Record *record;        /* Record used for input and output */
    nitf_IOHandle in;           /* Input I/O handle */
    nitf_ListIterator desIter;  /* Iterator for getting the DES */
    nitf_DESegment *des;        /* DE segment to read */
    nitf_DEReader *deReader;    /* DE reader object for reading data */
    char *data;                 /* Data buffer */
    static nitf_Error errorObj; /* Error object for messages */
    nitf_Error *error;          /* Pointer to the error object */

    error = &errorObj;

    if (argc != 2)
    {
        fprintf(stderr, "Usage %s inputFile\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /*    Get the input record */

    in = nitf_IOHandle_create(argv[1],
                              NITF_ACCESS_READONLY, NITF_OPEN_EXISTING,
                              error);
    if (NITF_INVALID_HANDLE(in))
    {
        nitf_Error_print(error, stderr, "Error opening input ");
        exit(EXIT_FAILURE);
    }

    reader = nitf_Reader_construct(error);
    if (!reader)
    {
        nitf_IOHandle_close(in);
        nitf_Error_print(error, stderr, "Error creating reader ");
        exit(EXIT_FAILURE);
    }

    record = nitf_Reader_read(reader, in, error);
    if (!record)
    {
        nitf_Reader_destruct(&reader);
        nitf_IOHandle_close(in);
        nitf_Error_print(error, stderr, "Error reading input ");
        exit(EXIT_FAILURE);
    }

    /*    Print the user header */

    desIter = nitf_List_at(record->dataExtensions, 0);
    des = (nitf_DESegment *) nitf_ListIterator_get(&desIter);
    if (!des)
    {
        nitf_Reader_destruct(&reader);
        nitf_Record_destruct(&record);
        nitf_IOHandle_close(in);
        nitf_Error_print(error, stderr, "No DE segment in file ");
        exit(EXIT_FAILURE);
    }

    nitf_TRE_print(des->subheader->subheaderFields, error);

    /*    Read the data and print it */

    deReader = nitf_Reader_newDEReader(reader, 0, error);
    if (!deReader)
    {
        nitf_Reader_destruct(&reader);
        nitf_Record_destruct(&record);
        nitf_IOHandle_close(in);
        nitf_Error_print(error, stderr, "Could not create DEReader ");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Data length = %d\n", deReader->user->dataLength);
    fprintf(stdout, "Virtual data length = %d\n",
            deReader->user->virtualLength);
    fprintf(stdout, "File offset = %lld\n", deReader->user->baseOffset);

    data = (char *) NITF_MALLOC(deReader->user->virtualLength + 2);
    if (!data)
    {
        nitf_DEReader_destruct(&deReader);
        nitf_Reader_destruct(&reader);
        nitf_Record_destruct(&record);
        nitf_IOHandle_close(in);
        nitf_Error_print(error, stderr, "Could not allocate data buffer ");
        exit(EXIT_FAILURE);
    }

    if (!nitf_DEReader_read
            (deReader, data, deReader->user->virtualLength, error))
    {
        NITF_FREE(data);
        nitf_DEReader_destruct(&deReader);
        nitf_Reader_destruct(&reader);
        nitf_Record_destruct(&record);
        nitf_IOHandle_close(in);
        nitf_Error_print(error, stderr, "Read failed ");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Data: |%s|\n", data);

    /*    Try some seeks */

    if (nitf_DEReader_seek(deReader, (nitf_Off) 4, SEEK_SET, error) == (nitf_Off) - 1)
    {
        NITF_FREE(data);
        nitf_DEReader_destruct(&deReader);
        nitf_Reader_destruct(&reader);
        nitf_Record_destruct(&record);
        nitf_IOHandle_close(in);
        nitf_Error_print(error, stderr, "Read failed ");
        exit(EXIT_FAILURE);
    }

    if (!nitf_DEReader_read(deReader, data, 1, error))
    {
        NITF_FREE(data);
        nitf_DEReader_destruct(&deReader);
        nitf_Reader_destruct(&reader);
        nitf_Record_destruct(&record);
        nitf_IOHandle_close(in);
        nitf_Error_print(error, stderr, "Read failed ");
        exit(EXIT_FAILURE);
    }

    data[1] = 0;
    fprintf(stdout, "Data after set seek: |%s|\n", data);

    /*
        The last read advanced to position 5 so relative position10 should be
        the last byte in the file
    */

    if (nitf_DEReader_seek(deReader, (nitf_Off) 10, SEEK_CUR, error) == (nitf_Off) - 1)
    {
        NITF_FREE(data);
        nitf_DEReader_destruct(&deReader);
        nitf_Reader_destruct(&reader);
        nitf_Record_destruct(&record);
        nitf_IOHandle_close(in);
        nitf_Error_print(error, stderr, "Read failed ");
        exit(EXIT_FAILURE);
    }

    if (!nitf_DEReader_read(deReader, data, 1, error))
    {
        NITF_FREE(data);
        nitf_DEReader_destruct(&deReader);
        nitf_Reader_destruct(&reader);
        nitf_Record_destruct(&record);
        nitf_IOHandle_close(in);
        nitf_Error_print(error, stderr, "Read failed ");
        exit(EXIT_FAILURE);
    }

    data[1] = 0;
    fprintf(stdout, "Data after current seek: |%s|\n", data);

    if (nitf_DEReader_seek(deReader, (nitf_Off) - 2, SEEK_END, error) == (nitf_Off) - 1)
    {
        NITF_FREE(data);
        nitf_DEReader_destruct(&deReader);
        nitf_Reader_destruct(&reader);
        nitf_Record_destruct(&record);
        nitf_IOHandle_close(in);
        nitf_Error_print(error, stderr, "Read failed ");
        exit(EXIT_FAILURE);
    }

    if (!nitf_DEReader_read(deReader, data, 1, error))
    {
        NITF_FREE(data);
        nitf_DEReader_destruct(&deReader);
        nitf_Reader_destruct(&reader);
        nitf_Record_destruct(&record);
        nitf_IOHandle_close(in);
        nitf_Error_print(error, stderr, "Read failed ");
        exit(EXIT_FAILURE);
    }

    data[1] = 0;
    fprintf(stdout, "Data after end seek: |%s|\n", data);

    /*    Clean-up */

    NITF_FREE(data);
    nitf_DEReader_destruct(&deReader);
    nitf_Reader_destruct(&reader);
    nitf_Record_destruct(&record);
    nitf_IOHandle_close(in);
    return 0;

}
