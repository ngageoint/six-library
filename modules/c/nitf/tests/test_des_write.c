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

  This program writes a NITF file with a file header and a single DE Segment.

  The record is cloned from an input NITF with a single DE segment. Some
  fields and the data are modified and the result written out as a new NITF

  This program uses the test_DES_example DE segment

  The calling sequence is:

    test_des_write inputFile outputFile

*/

static char data[] = "123456789ABCDEF0";

#include <string.h>

#include <import/nitf.h>
#include "nitf/UserSegment.h"

extern nitf_IUserSegment test_DES_example;

int main(int argc, char *argv[])
{
    nitf_Reader *reader;           /* Reader object */
    nitf_Record *record;           /* Record used for input and output */
    nitf_IOHandle in;              /* Input I/O handle */
    nitf_ListIterator desIter;     /* Iterator for getting the DES */
    nitf_DESegment *des;           /* DE segment to read */
    nitf_IOHandle out;             /* Output I/O handle */
    nitf_Writer *writer;           /* Writer object */
    nitf_SegmentWriter *desWriter; /* Segment writer for DE segment */
    nitf_SegmentSource *desData;   /* DE segment data source */
    static nitf_Error errorObj;    /* Error object for messages */
    nitf_Error *error;             /* Pointer to the error object */

    error = &errorObj;

    if (argc != 3)
    {
        fprintf(stderr, "Usage %s inputFile outputFile\n", argv[0]);
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

    /*    Get the user header */

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

    nitf_Reader_destruct(&reader);
    nitf_IOHandle_close(in);

    /*LJW    nitf_TRE_print(des->subheader->subheaderFields, error); */

    /*   Create output */

    out = nitf_IOHandle_create(argv[2], NITF_ACCESS_WRITEONLY, NITF_CREATE, error);
    if (NITF_INVALID_HANDLE(out))
    {
        nitf_Record_destruct(&record);
        nitf_Error_print(error, stderr, "Write setup failed ");
        exit(EXIT_FAILURE);
    }

    /*   Setup write */

    writer = nitf_Writer_construct(error);
    if (!writer)
    {
        nitf_IOHandle_close(out);
        nitf_Record_destruct(&record);
        nitf_Error_print(error, stderr, "Write setup failed ");
        exit(EXIT_FAILURE);
    }

    if (!nitf_Writer_prepare(writer, record, out, error))
    {
        nitf_IOHandle_close(out);
        nitf_Record_destruct(&record);
        nitf_Writer_destruct(&writer);
        nitf_Error_print(error, stderr, "Write setup failed ");
        exit(EXIT_FAILURE);
    }

    desWriter = nitf_Writer_newDEWriter(writer, 0, error);
    if (desWriter == NULL)
    {
        nitf_IOHandle_close(out);
        nitf_Record_destruct(&record);
        nitf_Writer_destruct(&writer);
        nitf_Error_print(error, stderr, "Write setup failed ");
        exit(EXIT_FAILURE);
    }

    desData = nitf_SegmentMemorySource_construct(data, strlen(data), 0, 0, error);
    if (desData == NULL)
    {
        nitf_IOHandle_close(out);
        nitf_Record_destruct(&record);
        nitf_Writer_destruct(&writer);
        nitf_Error_print(error, stderr, "Write setup failed ");
        exit(EXIT_FAILURE);
    }

    if (!nitf_SegmentWriter_attachSource(desWriter, desData, error))
    {
        nitf_IOHandle_close(out);
        nitf_SegmentSource_destruct(&desData);
        nitf_Record_destruct(&record);
        nitf_Writer_destruct(&writer);
        nitf_Error_print(error, stderr, "Write setup failed ");
        exit(EXIT_FAILURE);
    }

    if (!nitf_Writer_write(writer, error))
    {
        nitf_IOHandle_close(out);
        nitf_Record_destruct(&record);
        nitf_Writer_destruct(&writer);
        nitf_Error_print(error, stderr, "Write setup failed ");
        exit(EXIT_FAILURE);
    }

    /*    Clean-up */

    nitf_IOHandle_close(out);
    nitf_Writer_destruct(&writer);
    nitf_Record_destruct(&record);
    exit(EXIT_SUCCESS);
}
