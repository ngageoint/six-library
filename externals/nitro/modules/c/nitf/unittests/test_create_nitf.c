/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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


/**
 * This test serves as an example to show how one can construct and write
 * a NITF from scratch.
 */


#include <import/nitf.h>
#include "Test.h"

#include "nitro_image_.c_"

const char* RGB[3] = {"R", "G", "B"};



NITF_BOOL populateFileHeader(nitf_Record *record, const char* title,
                             nitf_Error *error)
{
    /* the file header is already created, so just grab it */
    nitf_FileHeader *header = record->header;


/*     if (!nitf_Field_setUint32(header->complianceLevel, 3, error)) */
/*         goto CATCH_ERROR; */
    if (!nitf_Field_setString(header->originStationID, "SF.net", error))
        goto CATCH_ERROR;
    /* the filedatetime can get auto-populated at write-time now, if blank */
    /*if (!nitf_Field_setString(header->fileDateTime, "20080812000000", error))
        goto CATCH_ERROR;*/
    if (!nitf_Field_setString(header->fileTitle, title, error))
        goto CATCH_ERROR;

    return NITF_SUCCESS;

  CATCH_ERROR:
    return NITF_FAILURE;
}


NITF_BOOL setCornersFromDMSBox(nitf_ImageSubheader* header, nitf_Error * error)
{
    /*
     *  You could do this in degrees as easily
     *  but this way we get to show off some new utilities
     */
    int latTopDMS[3]    = { 42, 17, 50 };
    int latBottomDMS[3] = { 42, 15, 14 };
    int lonEastDMS[3]   = { -83, 42, 12 };
    int lonWestDMS[3]   = { -83, 45, 44 };

    double latTopDecimal =
        nitf_Utils_geographicToDecimal(latTopDMS[0],
                                       latTopDMS[1],
                                       latTopDMS[2]);

    double latBottomDecimal =
        nitf_Utils_geographicToDecimal(latBottomDMS[0],
                                       latBottomDMS[1],
                                       latBottomDMS[2]);

    double lonEastDecimal =
        nitf_Utils_geographicToDecimal(lonEastDMS[0],
                                       lonEastDMS[1],
                                       lonEastDMS[2]);


    double lonWestDecimal =
        nitf_Utils_geographicToDecimal(lonWestDMS[0],
                                       lonWestDMS[1],
                                       lonWestDMS[2]);

    double corners[4][2];
    corners[0][0] = latTopDecimal;     corners[0][1] = lonWestDecimal;
    corners[1][0] = latTopDecimal;     corners[1][1] = lonEastDecimal;
    corners[2][0] = latBottomDecimal;  corners[2][1] = lonEastDecimal;
    corners[3][0] = latBottomDecimal;  corners[3][1] = lonWestDecimal;

    return nitf_ImageSubheader_setCornersFromLatLons(header,
                                                     NITF_CORNERS_DECIMAL,
                                                     corners,
                                                     error);


}

NITF_BOOL addImageSegment(nitf_Record *record, nitf_Error *error)
{
    nitf_ImageSegment *segment = NULL;
    nitf_ImageSubheader *header = NULL;
    nitf_BandInfo **bands = NULL;

    int i;

    segment = nitf_Record_newImageSegment(record, error);
    if (!segment)
        goto CATCH_ERROR;

    /* grab the subheader */
    header = segment->subheader;

    /* populate some fields */
    if (!nitf_Field_setString(header->imageId, "NITRO-TEST", error))
        goto CATCH_ERROR;
    /* fake the date */
    if (!nitf_Field_setString(header->imageDateAndTime, "20080812000000", error))
        goto CATCH_ERROR;

    /* Set the geo-corners to Ann Arbor, MI */
    if (!setCornersFromDMSBox(header, error))
        goto CATCH_ERROR;

    /* create a BandInfo buffer */
    bands = (nitf_BandInfo **) NITF_MALLOC(sizeof(nitf_BandInfo *) * 3);
    if (bands == NULL)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }


    for (i = 0; i < 3; ++i)
    {
        bands[i] = nitf_BandInfo_construct(error);
        if (!bands[i])
            goto CATCH_ERROR;

        if (!nitf_BandInfo_init(bands[i],
                                RGB[i],   /* The band representation, Nth band */
                                " ",      /* The band subcategory */
                                "N",      /* The band filter condition */
                                "   ",    /* The band standard image filter code */
                                0,        /* The number of look-up tables */
                                0,        /* The number of entries/LUT */
                                NULL,     /* The look-up tables */
                                error))
            goto CATCH_ERROR;
    }

    /* set the pixel information */
    if (!nitf_ImageSubheader_setPixelInformation(header,    /* header */
                                                 "INT",     /* Pixel value type */
                                                 8,         /* Number of bits/pixel */
                                                 8,         /* Actual number of bits/pixel */
                                                 "R",       /* Pixel justification */
                                                 "RGB",     /* Image representation */
                                                 "VIS",     /* Image category */
                                                 3,         /* Number of bands */
                                                 bands,     /* Band information object list */
                                                 error))
        goto CATCH_ERROR;

    /* for fun, let's add a comment */
    if (nitf_ImageSubheader_insertImageComment(header, "NITF generated by NITRO",
                                                0, error) < 0)
        goto CATCH_ERROR;

    /* set the blocking info */
    if (!nitf_ImageSubheader_setBlocking(header,
                                         NITRO_IMAGE.height, /*!< The number of rows */
                                         NITRO_IMAGE.width,  /*!< The number of columns */
                                         NITRO_IMAGE.height, /*!< The number of rows/block */
                                         NITRO_IMAGE.width,  /*!< The number of columns/block */
                                         "P",                /*!< Image mode */
                                         error))
        goto CATCH_ERROR;


    return NITF_SUCCESS;

  CATCH_ERROR:
    return NITF_FAILURE;
}


NITF_BOOL writeNITF(nitf_Record *record, const char* filename, nitf_Error *error)
{
    nitf_IOHandle out;
    nitf_Writer *writer = NULL;
    nitf_ImageWriter *imageWriter = NULL;
    nitf_ImageSource *imageSource;
    uint32_t i;

    /* create the IOHandle */
    out = nitf_IOHandle_create(filename, NITF_ACCESS_WRITEONLY,
                               NITF_CREATE, error);

    if (NITF_INVALID_HANDLE(out))
        goto CATCH_ERROR;

    writer = nitf_Writer_construct(error);
    if (!writer)
        goto CATCH_ERROR;

    /* prepare the writer to write this record */
    if (!nitf_Writer_prepare(writer, record, out, error))
        goto CATCH_ERROR;

    /* get a new ImageWriter for the 1st image (index 0) */
    imageWriter = nitf_Writer_newImageWriter(writer, 0, NULL, error);
    if (!imageWriter)
        goto CATCH_ERROR;

    /* create an ImageSource for our embedded image */
    imageSource = nitf_ImageSource_construct(error);
    if (!imageSource)
        goto CATCH_ERROR;

    /* make one bandSource per band */
    for (i = 0; i < 3; ++i)
    {
        nitf_BandSource *bandSource = nitf_MemorySource_construct(
            (char*)NITRO_IMAGE.data, NITRO_IMAGE.width * NITRO_IMAGE.height,
                i, 1, 2, error);
        if (!bandSource)
            goto CATCH_ERROR;

        /* attach the band to the image */
        if (!nitf_ImageSource_addBand(imageSource, bandSource, error))
            goto CATCH_ERROR;
    }

    /* enable caching within the writer */
    nitf_ImageWriter_setWriteCaching(imageWriter, 1);

    /* attach the ImageSource to the writer */
    if (!nitf_ImageWriter_attachSource(imageWriter, imageSource, error))
        goto CATCH_ERROR;

    /* finally, write it! */
    if (!nitf_Writer_write(writer, error))
    {
        nitf_Error_print(error, stderr, "Error writing up write");
        exit(1);
    }

    /* cleanup */
    nitf_IOHandle_close(out);
    nitf_Writer_destruct(&writer);
    return NITF_SUCCESS;

  CATCH_ERROR:
    if (writer) nitf_Writer_destruct(&writer);
    return NITF_FAILURE;
}


TEST_CASE_ARGS(testCreate)
{
    nitf_Record *record = NULL;
    nitf_Error error;
    const char* outname = argc > 1 ? argv[1] : "test_create.ntf";

    record = nitf_Record_construct(NITF_VER_21, &error);
    TEST_ASSERT(record != NULL);
    TEST_ASSERT(populateFileHeader(record, outname, &error));
    TEST_ASSERT(addImageSegment(record, &error));
    TEST_ASSERT(writeNITF(record, outname, &error));
    nitf_Record_destruct(&record);
}

TEST_CASE_ARGS(testRead)
{
    nitf_Reader *reader = NULL;
    nitf_Record *record = NULL;
    nitf_Error error;
    nitf_IOHandle io;
    const char* outname = argc > 1 ? argv[1] : "test_create.ntf";

    io = nitf_IOHandle_create(outname, NITF_ACCESS_READONLY, NITF_OPEN_EXISTING, &error);
    reader = nitf_Reader_construct(&error);
    TEST_ASSERT(reader != NULL);
    record = nitf_Reader_read(reader, io, &error);
    TEST_ASSERT(record != NULL);
    nitf_Reader_destruct(&reader);
    nitf_Record_destruct(&record);
}

TEST_MAIN(
    (void)argc;
    (void)argv;
    CHECK_ARGS(testCreate);
    CHECK_ARGS(testRead);
    )

