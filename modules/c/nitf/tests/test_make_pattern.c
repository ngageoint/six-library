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
*      NITF file generator
*
*  test_make_pattern creates a NITF file with a particular data pattern
*
* The layout of the data is defined by a test vector. This is the same
* specification format as the program test_ImageIO_writePattern
*
*  The command line call is:
*
*  test_make_pattern vector input output
*
*  The first argument is the test vector filename and the second is the
*  input file and the third is the output file
*
*  The input should be a "blank" image with no segments. It is used to
*  initialize the file header
*
*  See the test_ImageIO programs for documenation on the test vector format
*/

#include <import/nitf.h>

#include "test_ImageIO.h"

#define NUM_BANDS_MAX 50

/*  Local functions defined after main */

nitf_Record *readRecord(char *file);
void setupImageSubheader(
    test_nitf_ImageIOConstructArgs *args,
    nitf_Record *record, nitf_ImageSegment *seg);

nitf_ImageSource *makeImageSource(
    test_nitf_ImageIOConstructArgs *args, char ***data);

int main(int argc, char *argv[])
{
    char *vectorFile;                /* Vector file name */
    char *inputFile;                 /* Input file name */
    char *outputFile;                /* Output file name */
    FILE *vector;                    /* File stream for input vector */
    /* Arguments for new */
    test_nitf_ImageIOConstructArgs *newArgs;
    /* Arguments for read */
    test_nitf_ImageIOReadArgs *readArgs;
    char *errorStr;                  /* Error string */

    nitf_Record *record;           /* Record used for input and output */
    nitf_FileHeader *fileHdr;      /* File header */
    nitf_ImageSegment *imgSeg;     /* New image segment */
    nitf_Writer *writer;           /* Writer for output */
    nitf_ImageWriter *imgWriter;   /* Image writer */
    nitf_ImageSource *imgSource;   /* Image source */


    nitf_IOHandle out;               /* Handle for output */
    nitf_Error error;                /* Error object */
    nitf_Uint8 ***data;              /* Generated data [band][row][col] */

    if (argc < 4)
    {
        fprintf(stderr, "test_make_pattern vector input output\n");
        exit(-1);
    }
    vectorFile = argv[1];
    inputFile = argv[2];
    outputFile = argv[3];

    vector = fopen(vectorFile, "r");
    if (vector == NULL)
    {
        fprintf(stderr, "Error opening vector file %s\n", vectorFile);
        exit(-1);
    }

    newArgs = test_nitf_ImageIOReadConstructArgs(vector, &errorStr);
    if (newArgs == NULL)
    {
        fprintf(stderr, "%s\n", errorStr);
        return(-1);
    }

    fclose(vector);

    /*  Create the input record which will be used for output */

    record = readRecord(inputFile);

    /* Setup the image segment */

    imgSeg = nitf_Record_newImageSegment(record, &error);
    if (imgSeg == NULL)
    {
        nitf_Error_print(&error, stderr, "Error reading input ");
        exit(EXIT_FAILURE);
    }

    setupImageSubheader(newArgs, record, imgSeg);

    /*      Create Image */

    if (strcmp(newArgs->dataPattern, "brcI4") == 0)
    {
        data = (nitf_Uint8 ***) test_nitf_ImageIO_brcI4(newArgs, &errorStr);
        if (data == NULL)
        {
            fprintf(stderr, "%s\n", errorStr);
            exit(-1);
        }
    }
    else if (strcmp(newArgs->dataPattern, "brcC8") == 0)
    {
        data = (nitf_Uint8 ***) test_nitf_ImageIO_brcC8(newArgs, &errorStr);
        if (data == NULL)
        {
            fprintf(stderr, "%s\n", errorStr);
            exit(-1);
        }
    }
    else if (strncmp(newArgs->dataPattern, "blocks_", 7) == 0)
    {
        data = (nitf_Uint8 ***) test_nitf_ImageIO_block(newArgs, &errorStr);
        if (data == NULL)
        {
            fprintf(stderr, "%s\n", errorStr);
            exit(-1);
        }
    }
    else
    {
        fprintf(stderr, "Invalid pattern method %s\n");
        exit(-1);
    }

    /*      Create output file */

    out = nitf_IOHandle_create(outputFile,
                               NITF_ACCESS_WRITEONLY, NITF_CREATE | NITF_TRUNCATE, &error);
    if (NITF_INVALID_HANDLE(out))
    {
        nitf_Error_print(&error, stderr, "Error creating output file");
        exit(1);
    }

    writer = nitf_Writer_construct(&error);
    if (writer == NULL)
    {
        nitf_Error_print(&error, stderr, "Error creating writer object");
        exit(1);
    }

    if (!nitf_Writer_prepare(writer, record, out, &error))
    {
        nitf_Error_print(&error, stderr, "Error setting up write");
        exit(1);
    }

    imgWriter = nitf_Writer_newImageWriter(writer, 0, &error);
    if (imgWriter == NULL)
    {
        nitf_Error_print(&error, stderr, "Error setting up write");
        exit(1);
    }
    imgSource = makeImageSource(newArgs, (char ***) data);
    nitf_ImageWriter_setWriteCaching(imgWriter, 1);

    if (!nitf_ImageWriter_attachSource(imgWriter, imgSource, &error))
    {
        nitf_Error_print(&error, stderr, "Error setting up write");
        exit(1);
    }


    if (!nitf_Writer_write(writer, &error))
    {
        nitf_Error_print(&error, stderr, "Error writing up write");
        exit(1);
    }


    /* Destroy things */

    test_nitf_ImageIO_freeArray(data);
    nitf_IOHandle_close(out);
    exit(0);
}


/* Read in input file to create the record to use for the output */

nitf_Record *readRecord(char *file)
{
    nitf_IOHandle in;            /* Handle for input */
    nitf_Reader *reader;         /* Reader object */
    nitf_Record *record;         /* The result */
    nitf_Error error;            /* For errors */

    in = nitf_IOHandle_create(file,
                              NITF_ACCESS_READONLY, NITF_OPEN_EXISTING, &error);
    if (NITF_INVALID_HANDLE(in))
    {
        nitf_Error_print(&error, stderr, "Error opening input ");
        exit(EXIT_FAILURE);
    }

    reader = nitf_Reader_construct(&error);
    if (!reader)
    {
        nitf_Error_print(&error, stderr, "Error creating reader ");
        exit(EXIT_FAILURE);
    }

    record = nitf_Reader_read(reader, in, &error);
    if (!record)
    {
        nitf_Error_print(&error, stderr, "Error reading input ");
        exit(EXIT_FAILURE);
    }

    nitf_IOHandle_close(in);
    nitf_Reader_destruct(&reader);
    return(record);
}

/*  Function to initialize the image subheader based on the vector */

void setupImageSubheader(
    test_nitf_ImageIOConstructArgs *args,
    nitf_Record *record, nitf_ImageSegment *seg)
{
    nitf_ImageSubheader *subheader;   /* Subheader from segment */
    nitf_Uint32 nBands;               /* Number of bands */
    nitf_BandInfo **bands;            /* BandInfo array */
    nitf_Error errorObj;              /* Error object argument */
    nitf_Uint32 i;

    subheader = seg->subheader;
    nitf_Field_setUint32(subheader->numRows, args->nRows, &errorObj);
    nitf_Field_setUint32(subheader->numCols, args->nColumns, &errorObj);
    nitf_Field_setUint32(subheader->numImageBands, args->nBands, &errorObj);
    nitf_Field_setUint32(subheader->numMultispectralImageBands,
                         args->nMultiBands, &errorObj);
    nitf_Field_setUint32(subheader->numBlocksPerRow,
                         args->nBlksPerRow, &errorObj);
    nitf_Field_setUint32(subheader->numBlocksPerCol,
                         args->nBlksPerColumn, &errorObj);
    nitf_Field_setUint32(subheader->numPixelsPerVertBlock,
                         args->nRowsPerBlk, &errorObj);
    nitf_Field_setUint32(subheader->numPixelsPerHorizBlock,
                         args->nColumnsPerBlk, &errorObj);
    nitf_Field_setString(subheader->imageMode, args->mode, &errorObj);
    nitf_Field_setString(subheader->imageCompression,
                         args->compression, &errorObj);
    nBands = args->nBands;
    if (nBands == 0)
        nBands = args->nMultiBands;

    bands = (nitf_BandInfo **) NITF_MALLOC(sizeof(nitf_BandInfo *));
    if (bands == NULL)
    {
        nitf_Error_init(&errorObj, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        nitf_Error_print(&errorObj, stderr, "Error setting up band source ");
        exit(EXIT_FAILURE);
    }

    for (i = 0;i < nBands;i++)
    {
        bands[i] = nitf_BandInfo_construct(&errorObj);
        if (bands[i] == NULL)
        {
            nitf_Error_print(&errorObj, stderr, "Error setting up band source ");
            exit(EXIT_FAILURE);
        }

        if (!nitf_BandInfo_init(bands[i],
                                "M",      /* The band representation, Nth band */
                                " ",      /* The band subcategory */
                                "N",      /* The band filter condition */
                                "   ",    /* The band standard image filter code */
                                0,        /* The number of look-up tables */
                                0,        /* The number of entries/LUT */
                                NULL,     /* The look-up tables */
                                &errorObj))
        {
            nitf_Error_print(&errorObj, stderr, "Error setting up band source ");
            exit(EXIT_FAILURE);
        }
    }

    if (!nitf_ImageSubheader_setPixelInformation(subheader,
            args->pixelType,      /* Pixel value type */
            args->nBits,          /* Number of bits/pixel*/
            args->nBitsActual,    /* Actual number of bits/pixel */
            args->justify,        /* Pixel justification */
            "MONO",               /* Image representation */
            "SAR",                /* Image category */
            nBands,               /* Number of bands */
            bands,                /* Band information object list */
            &errorObj ))
    {
        nitf_Error_print(&errorObj, stderr, "Error setting up band source ");
        exit(EXIT_FAILURE);
    }

    return;
}

/*
   Make the image source for write

  The source is built memory data sources take from data

  &(data[band][0][0]) points to the contiguous data for band "band"
*/

nitf_ImageSource *makeImageSource(
    test_nitf_ImageIOConstructArgs *args, char ***data)
{
    nitf_ImageSource *imgSource;  /* The result */
    nitf_Uint32 nBands;           /* Number of bands */
    nitf_Off bandSize;               /* Size of individual bands */
    nitf_Error error;             /* Error object argument */
    nitf_Uint32 i;

    bandSize = (args->nRows) * (args->nColumns) * (NITF_NBPP_TO_BYTES(args->nBits));

    imgSource = nitf_ImageSource_construct(&error);
    if (imgSource == NULL)
    {
        nitf_Error_print(&error, stderr, "Error setting up image source ");
        exit(EXIT_FAILURE);
    }

    nBands = args->nBands;
    if (nBands == 0)
        nBands = args->nMultiBands;

    for (i = 0;i < nBands;i++)
    {
        nitf_DataSource * bandSrc;   /* Current band data source */

        bandSrc = nitf_MemorySource_construct(
                      &(data[i][0][0]), bandSize, 0, 0, 0, &error);
        if (bandSrc == NULL)
        {
            nitf_Error_print(&error, stderr, "Error setting up band source ");
            exit(EXIT_FAILURE);
        }

        if (!nitf_ImageSource_addBand(imgSource, bandSrc, &error))
        {
            nitf_Error_print(&error, stderr, "Error setting up band source ");
            exit(EXIT_FAILURE);
        }

    }
    return(imgSource);
}
