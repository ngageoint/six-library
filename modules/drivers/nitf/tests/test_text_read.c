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
    Test program for reading a Text segment

  This program reads a NITF file

  The calling sequence is:

    test_text_read inputFile

*/

#include <import/nitf.h>

int main(int argc, char *argv[])
{
    nitf_Reader *reader;        /* Reader object */
    nitf_Record *record;        /* Record used for input and output */
    nitf_IOHandle in;           /* Input I/O handle */
    nitf_ListIterator iter;     /* Iterator for getting the Texts */
    nitf_ListIterator end;      /* Iterator marking the end of texts */
    nitf_TextSegment *text;     /* Text segment to read */
    nitf_SegmentReader *textReader;/* Text reader object for reading data */
    char *data;                 /* Data buffer */
    int count;                  /* keeps track of iter count */
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

    count = 0;
    iter = nitf_List_begin(record->texts);
    end = nitf_List_end(record->texts);
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        text = (nitf_TextSegment *) nitf_ListIterator_get(&iter);
        textReader = nitf_Reader_newTextReader(reader, count, error);

        if (!textReader)
        {
            nitf_Reader_destruct(&reader);
            nitf_Record_destruct(&record);
            nitf_IOHandle_close(in);
            nitf_Error_print(error, stderr, "Could not create TextReader");
            exit(EXIT_FAILURE);
        }

        fprintf(stdout, "Data length = %d\n", textReader->dataLength);
        fprintf(stdout, "File offset = %lu\n", textReader->baseOffset);
        fprintf(stdout, "Virtual offset = %lu\n", textReader->virtualOffset);

        data = (char *) NITF_MALLOC
               (nitf_SegmentReader_getSize(textReader, error) + 1);
        if (!data)
        {
            nitf_SegmentReader_destruct(&textReader);
            nitf_Reader_destruct(&reader);
            nitf_Record_destruct(&record);
            nitf_IOHandle_close(in);
            nitf_Error_print(error, stderr, "Could not allocate data buffer ");
            exit(EXIT_FAILURE);
        }
        memset(data, 0, nitf_SegmentReader_getSize(textReader, error) + 1);


        if (!nitf_SegmentReader_read
                (textReader, data, textReader->dataLength, error))
        {
            NITF_FREE(data);
            nitf_SegmentReader_destruct(&textReader);
            nitf_Reader_destruct(&reader);
            nitf_Record_destruct(&record);
            nitf_IOHandle_close(in);
            nitf_Error_print(error, stderr, "Read failed ");
            exit(EXIT_FAILURE);
        }

        fprintf(stdout, "Data: |%s|\n", data);

        /* clean up */
        if (data) NITF_FREE(data);
        if (textReader) nitf_SegmentReader_destruct(&textReader);


        nitf_ListIterator_increment(&iter);
        ++count;
    }

    /*    Clean-up */

    nitf_Reader_destruct(&reader);
    nitf_Record_destruct(&record);
    nitf_IOHandle_close(in);
    return 0;
}
