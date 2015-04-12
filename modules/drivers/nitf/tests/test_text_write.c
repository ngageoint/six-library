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

#include <stdio.h>
#include <string.h>

#include <import/nitf.h>

/* *********************************************************************
** This test creates a NITF from scratch with a single text segment.
** Its contents are passed in from the command line.
** ********************************************************************/

static const char* const DATE_TIME = "20120126000000";

static
NITF_BOOL initializeHeader(nitf_FileHeader* header, nitf_Error* error)
{
    return (nitf_Field_setString(header->fileHeader, "NITF", error) &&
            nitf_Field_setString(header->fileVersion, "02.10", error) &&
            nitf_Field_setUint32(header->complianceLevel, 3, error) &&
            nitf_Field_setString(header->systemType, "BF01", error) &&
            nitf_Field_setString(header->originStationID, "SF.net", error) &&
            nitf_Field_setString(header->fileDateTime, DATE_TIME, error) &&
            nitf_Field_setString(header->fileTitle, "Text Segment Test", error) &&
            nitf_Field_setString(header->classification, "U", error) &&
            nitf_Field_setUint32(header->encrypted, 0, error));
}

static
NITF_BOOL initializeTextSubheader(nitf_TextSubheader* header,
                                  nitf_Error* error)
{
    return (nitf_Field_setString(header->dateTime, DATE_TIME, error) &&
            nitf_Field_setString(header->title, "Text Segment Test", error));
}

int main(int argc, char** argv)
{
    const char* outText = NULL;
    const char* outPathname = NULL;
    nitf_IOHandle outIO = NRT_INVALID_HANDLE_VALUE;
    nitf_Writer* writer = NULL;
    nitf_TextSegment* textSegment = NULL;
    nitf_SegmentWriter* textWriter = NULL;
    nitf_SegmentSource* textSource = NULL;
    nitf_Record* record = NULL;
    nitf_Error error;

    error.level = NRT_NO_ERR;

    /* Parse the command line */
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <text to write> <output pathname>\n", argv[0]);
        return 1;
    }
    outText = argv[1];
    outPathname = argv[2];

    /* Do the work */
    outIO = nitf_IOHandle_create(outPathname,
                                 NITF_ACCESS_WRITEONLY,
                                 NITF_CREATE,
                                 &error);

    if (NITF_INVALID_HANDLE(outIO))
    {
        goto CATCH_ERROR;
    }

    writer = nitf_Writer_construct(&error);
    if (!writer)
    {
        goto CATCH_ERROR;
    }

    record = nitf_Record_construct(NITF_VER_21, &error);
    if (!record)
    {
        goto CATCH_ERROR;
    }

    if (!initializeHeader(record->header, &error))
    {
        goto CATCH_ERROR;
    }

    textSegment = nitf_Record_newTextSegment(record, &error);
    if (!textSegment)
    {
        fprintf(stderr, "new text segment failed\n");
        return 1;
    }
    if (!initializeTextSubheader(textSegment->subheader, &error))
    {
        goto CATCH_ERROR;
    }

    if (!nitf_Writer_prepare(writer, record, outIO, &error))
    {
        goto CATCH_ERROR;
        return 1;
    }

    textWriter = nitf_Writer_newTextWriter(writer, 0, &error);
    if (!textWriter)
    {
        goto CATCH_ERROR;
    }

    textSource = nitf_SegmentMemorySource_construct(outText,
                                                    strlen(outText),
                                                    0, 0, 0, &error);
    if (!textSource)
    {
        goto CATCH_ERROR;
    }

    if (!nitf_SegmentWriter_attachSource(textWriter, textSource, &error))
    {
        goto CATCH_ERROR;
    }

    nitf_Writer_write(writer, &error);

CATCH_ERROR:
    if (!NITF_INVALID_HANDLE(outIO))
    {
        nitf_IOHandle_close(outIO);
    }
    if (writer)
    {
        nitf_Writer_destruct(&writer);
    }

    if (error.level != NRT_NO_ERR)
    {
        nitf_Error_print(&error, stderr, "Exiting...");
        return 1;
    }

    return 0;
}
