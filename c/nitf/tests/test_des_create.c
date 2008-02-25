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

/*
    Test program for creating a DE segment

  This program creates a NITF file with a file header and a single DE Segment.

  The record is cloned from an input NITF.

  The DES data and user header fields are set and the result written out as
  a new NITF

  The input NITF file should have no DES segments

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
    nitf_DESegment *des;           /* DE segment to read */
    nitf_FileSecurity *security;   /* File security for DE segment header */
    nitf_IUserSegment *usrSegIface;/* User segment supplying DES TRE descriptor */
    int bad;                       /* Error flag */
    nitf_TRE *usrHdr;              /* User header for DES */
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

    nitf_Reader_destruct(&reader);
    nitf_IOHandle_close(in);

    /*  Create the DE segment */

    des = nitf_Record_newDataExtensionSegment(record, error);
    if (!des)
    {
        nitf_Record_destruct(&record);
        nitf_Error_print(error, stderr, "Error creating new DE segment ");
        exit(EXIT_FAILURE);
    }

    security = nitf_FileSecurity_clone(record->header->securityGroup, error);
    nitf_FileSecurity_destruct(&(des->subheader->securityGroup));
    des->subheader->securityGroup = security;

    /*  Create the user header and assign to the DE segemnt */

    usrSegIface = nitf_UserSegment_getInterface("TEST_DES", &bad, error);
    if (bad)
    {
        nitf_Record_destruct(&record);
        nitf_Error_print(error, stderr, "No error getting DES spec ");
        exit(EXIT_FAILURE);
    }

    usrHdr = nitf_TRE_construct("DES",
                                usrSegIface->headerDesc, NITF_TRE_DEFAULT_LENGTH, error);
    if (!usrHdr)
    {
        nitf_Record_destruct(&record);
        nitf_Error_print(error, stderr, "No error creating new DE segment ");
        exit(EXIT_FAILURE);
    }

    nitf_TRE_setValue(usrHdr, "NITF_TEST_DES_COUNT", "16", 2, error);
    nitf_TRE_setValue(usrHdr, "NITF_TEST_DES_START", "065", 3, error);
    nitf_TRE_setValue(usrHdr, "NITF_TEST_DES_INCREMENT", "01", 2, error);

    des->subheader->subheaderFields = usrHdr;

    nitf_Field_setString(des->subheader->NITF_DE, "DE", error);
    nitf_Field_setString(des->subheader->NITF_DESTAG, "TEST_DES", error);
    nitf_Field_setString(des->subheader->NITF_DESVER, "01", error);
    nitf_Field_setString(des->subheader->NITF_DESCLAS, "U", error);

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
