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

/*
 *  Take a buffer of memory that we read out of a NITF segment
 *  and write it one of two-ways.
 *
 *  For non RGB 24-bit true-color data, write each band separately.
 *  This demonstrates the typical, non-accelerated, generic read.
 *
 *  For 24-bit true-color, if its band interleaved by pixel (mode 'P')
 *  demonstrate accelerated mode, and read without de-interleaving pixels,
 *  then write it to one big image as is.
 *
 */
void writeImage(nitf_ImageSegment * segment,
                char *imageName,
                nitf_ImageReader * deserializer,
                int imageNumber,
                nitf_Uint32 rowSkipFactor,
                nitf_Uint32 columnSkipFactor,
                NITF_BOOL optz,
                nitf_Error * error)
{

    nitf_Uint32 nBits, nBands, xBands, nRows, nColumns;
    size_t subimageSize;
    nitf_SubWindow *subimage;
    unsigned int i;
    int padded;
    nitf_Uint8 **buffer = NULL;
    nitf_Uint32 band;
    nitf_Uint32 *bandList = NULL;

    nitf_DownSampler *pixelSkip;

    /* TODO, replace these macros! */
    NITF_TRY_GET_UINT32(segment->subheader->numBitsPerPixel, &nBits,
                        error);
    NITF_TRY_GET_UINT32(segment->subheader->numImageBands, &nBands, error);
    NITF_TRY_GET_UINT32(segment->subheader->numMultispectralImageBands,
                        &xBands, error);
    nBands += xBands;
    NITF_TRY_GET_UINT32(segment->subheader->numRows, &nRows, error);
    NITF_TRY_GET_UINT32(segment->subheader->numCols, &nColumns, error);
    subimageSize = (nRows / rowSkipFactor) * (nColumns / columnSkipFactor)
                   * NITF_NBPP_TO_BYTES(nBits);

    printf("Image number: %d\n", imageNumber);
    printf("NBANDS -> %d\n"
           "XBANDS -> %d\n"
           "NROWS -> %d\n"
           "NCOLS -> %d\n"
           "PVTYPE -> %.*s\n"
           "NBPP -> %.*s\n"
           "ABPP -> %.*s\n"
           "PJUST -> %.*s\n"
           "IMODE -> %.*s\n"
           "NBPR -> %.*s\n"
           "NBPC -> %.*s\n"
           "NPPBH -> %.*s\n"
           "NPPBV -> %.*s\n"
           "IC -> %.*s\n"
           "COMRAT -> %.*s\n",
           nBands,
           xBands,
           nRows,
           nColumns,
           (int)segment->subheader->pixelValueType->length,
           segment->subheader->pixelValueType->raw,
           (int)segment->subheader->numBitsPerPixel->length,
           segment->subheader->numBitsPerPixel->raw,
           (int)segment->subheader->actualBitsPerPixel->length,
           segment->subheader->actualBitsPerPixel->raw,
           (int)segment->subheader->pixelJustification->length,
           segment->subheader->pixelJustification->raw,
           (int)segment->subheader->imageMode->length,
           segment->subheader->imageMode->raw,
           (int)segment->subheader->numBlocksPerRow->length,
           segment->subheader->numBlocksPerRow->raw,
           (int)segment->subheader->numBlocksPerCol->length,
           segment->subheader->numBlocksPerCol->raw,
           (int)segment->subheader->numPixelsPerHorizBlock->length,
           segment->subheader->numPixelsPerHorizBlock->raw,
           (int)segment->subheader->numPixelsPerVertBlock->length,
           segment->subheader->numPixelsPerVertBlock->raw,
           (int)segment->subheader->imageCompression->length,
           segment->subheader->imageCompression->raw,
           (int)segment->subheader->compressionRate->length,
           segment->subheader->compressionRate->raw);
    

    if (optz)
    {
	/*
         *  There is an accelerated mode for band-interleaved by pixel data.
         *  In that case, we assume that the user doesnt want the data
         *  de-interleaved into separate band buffers.  To make this work
         *  you have to tell us that you want only one band back,
         *  and you have to provide us a singular buffer that is the
         *  actual number of bands x the pixel size.  Then we will
         *  read the window and not de-interleave.
         *  To demonstrate, for RGB24 images, let's we write out the 1 band
	 *  - this will be MUCH faster
	 */
	if (nBands == 3
            && segment->subheader->imageMode->raw[0] == 'P'
            && strncmp("RGB", segment->subheader->imageRepresentation->raw, 3) == 0
            && NITF_NBPP_TO_BYTES(nBits) == 1
            && (strncmp("NC", segment->subheader->imageCompression->raw, 2)
                || strncmp("NM", segment->subheader->imageCompression->raw, 2)))
	{
            subimageSize *= nBands;
            nBands = 1;
            printf("Using accelerated 3-band RGB mode pix-interleaved image\n");
	}


	if (nBands == 2
            && segment->subheader->NITF_IMODE->raw[0] == 'P'
            && NITF_NBPP_TO_BYTES(nBits) == 4
            && segment->subheader->bandInfo[0]->NITF_ISUBCAT->raw[0] == 'I'
            && (strncmp("NC", segment->subheader->NITF_IC->raw, 2)
                || strncmp("NM", segment->subheader->NITF_IC->raw, 2)))
	{
            subimageSize *= nBands;
            nBands = 1;
            printf("Using accelerated 2-band IQ mode pix-interleaved image\n");
	}
    }
    subimage = nitf_SubWindow_construct(error);
    assert(subimage);

    /* 
     *  You need a buffer for each band (unless this is an 
     *  accelerated IQ complex or RGB read, in which case, you
     *  can set the number of bands to 1, and size your buffer
     *  accordingly to receive band-interleaved by pixel data)
     */
    buffer = (nitf_Uint8 **) NITF_MALLOC(nBands * sizeof(nitf_Uint8*));

    /* An iterator for bands */
    band = 0;

    /* 
     *  This tells us what order to give you bands in.  Normally
     *  you just want it in the order of the banding.  For example,
     *  in a non-accelerated band-interleaved by pixel cases, you might
     *  have a band of magnitude and a band of phase.  If you order the
     *  bandList backwards, the phase buffer comes first in the output
     */
    bandList = (nitf_Uint32 *) NITF_MALLOC(sizeof(nitf_Uint32 *) * nBands);

    /* This example reads all rows and cols starting at 0, 0 */
    subimage->startCol = 0;
    subimage->startRow = 0;

    /* Request rows is the full rows dividied by pixel skip (usually 1) */
    subimage->numRows = nRows / rowSkipFactor;

    /* Request columns is the full columns divided by pixel skip (usually 1) */
    subimage->numCols = nColumns / columnSkipFactor;

    /* Construct our pixel skip downsampler (does nothing if skips are 1) */
    pixelSkip = nitf_PixelSkip_construct(rowSkipFactor, 
                                         columnSkipFactor, 
                                         error);
    if (!pixelSkip)
    {
        nitf_Error_print(error, stderr, "Pixel Skip construction failed");
        goto CATCH_ERROR;
    }
    if (!nitf_SubWindow_setDownSampler(subimage, pixelSkip, error))
    {
        nitf_Error_print(error, stderr, "Set down sampler failed");
        goto CATCH_ERROR;
    }

    for (band = 0; band < nBands; band++)
        bandList[band] = band;
    subimage->bandList = bandList;
    subimage->numBands = nBands;

    assert(buffer);
    for (i = 0; i < nBands; i++)
    {
        buffer[i] = (nitf_Uint8 *) NITF_MALLOC(subimageSize);
        assert(buffer[i]);
    }
    if (!nitf_ImageReader_read
            (deserializer, subimage, buffer, &padded, error))
    {
        nitf_Error_print(error, stderr, "Read failed");
        goto CATCH_ERROR;
    }
    for (i = 0; i < nBands; i++)
    {

        nitf_IOHandle toFile;
        char file[NITF_MAX_PATH];
        int pos;

        /* find end slash */
        for (pos = strlen(imageName) - 1;
                pos && imageName[pos] != '\\' && imageName[pos] != '/';
                pos--);

        pos = pos == 0 ? pos : pos + 1;
        NITF_SNPRINTF(file, NITF_MAX_PATH,
                      "%s_%d__%d_%d_%d_band_%d", &imageName[pos],
                      imageNumber, nRows / rowSkipFactor,
                      nColumns / columnSkipFactor, nBits, i);
        /* remove decimals */
        for (pos = strlen(file) - 1; pos; pos--)
            if (file[pos] == '.')
                file[pos] = '_';
        strcat(file, ".out");
        printf("File: %s\n", file);
        toFile = nitf_IOHandle_create(file, NITF_ACCESS_WRITEONLY,
                                      NITF_CREATE, error);
        if (NITF_INVALID_HANDLE(toFile))
        {
            nitf_Error_print(error, stderr,
                             "IO handle creation failed for raw band");
            goto CATCH_ERROR;
        }
        if (!nitf_IOHandle_write(toFile,
                                 (const char *) buffer[i],
                                 subimageSize, error))
        {
            nitf_Error_print(error, stderr, 
                             "IO handle write failed for raw band");
            goto CATCH_ERROR;
        }
        nitf_IOHandle_close(toFile);
    }

    /* free buffers */
    for (i = 0; i < nBands; i++)
        NITF_FREE(buffer[i]);

    NITF_FREE(buffer);
    NITF_FREE(bandList);
    nitf_SubWindow_destruct(&subimage);
    nitf_DownSampler_destruct(&pixelSkip);

    return;

CATCH_ERROR:
    /* free buffers */
    for (i = 0; i < nBands; i++)
        NITF_FREE(buffer[i]);
    NITF_FREE(buffer);
    NITF_FREE(bandList);
    printf("ERROR processing\n");
}

int main(int argc, char **argv)
{

    /*  This is the error we hopefully wont receive  */
    nitf_Error e;

    /* Skip factors */
    nitf_Uint32 rowSkipFactor = 1;
    nitf_Uint32 columnSkipFactor = 1;

    /*  This is the reader  */
    nitf_Reader *reader;

    /*  This is the record of the file we are reading  */
    nitf_Record *record;


    /*  This is the io handle we will give the reader to parse  */
    nitf_IOHandle io;

    int count = 0;
    int numImages;

    /*  These iterators are for going through the image segments  */
    nitf_ListIterator iter;
    nitf_ListIterator end;

    char* inputFile;
    NITF_BOOL optz = 0;

    /*  If you didnt give us a nitf file, we're croaking  */
    if (argc < 2)
    {
        printf("Usage: %s <nitf-file> (-o)\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc == 3)
    {
        optz = 1;

        if (strcmp(argv[1], "-o") == 0)
        {
            inputFile = argv[2];
        }
        else if (strcmp(argv[2], "-o") == 0)
        {
            inputFile = argv[1];
        }
        else
        {
            printf("Usage: %s <nitf-file> (-o)\n", argv[0]);
            exit(EXIT_FAILURE);

        }
    }
    else
        inputFile = argv[1];

    /*  You should use this function to test that you have a valid NITF */
    if (nitf_Reader_getNITFVersion( inputFile ) == NITF_VER_UNKNOWN)
    {
        printf("This file does not appear to be a valid NITF");
        exit(EXIT_FAILURE);
    }

    reader = nitf_Reader_construct(&e);
    if (!reader)
    {
        nitf_Error_print(&e, stderr, "Reader creation failed");
        exit(EXIT_FAILURE);
    }

    /*  
     *  As of 2.5, you do not have to use an IOHandle if you use 
     *  readIO instead of read()
     */
    io = nitf_IOHandle_create(inputFile,
                              NITF_ACCESS_READONLY,
                              NITF_OPEN_EXISTING, &e);

    if (NITF_INVALID_HANDLE(io))
    {
        nitf_Error_print(&e, stderr, "IO creation failed");
        exit(EXIT_FAILURE);
    }

    /*  Read the file  */
    record = nitf_Reader_read(reader, io, &e);
    if (!record)
    {
        nitf_Error_print(&e, stderr, "Read failed");
        nitf_IOHandle_close(io);
        nitf_Reader_destruct(&reader);
        exit(EXIT_FAILURE);
    }

    numImages = nitf_Record_getNumImages(record, &e);

    if ( NITF_INVALID_NUM_SEGMENTS( numImages ) )
    {
        nitf_Error_print(&e, stderr, "Failed to get the number of images");
        nitf_IOHandle_close(io);
        nitf_Reader_destruct(&reader);
        nitf_Record_destruct( &record );
        exit(EXIT_FAILURE);
    }

    /*  Set the iterator to traverse the list of image segments  */
    iter = nitf_List_begin(record->images);

    /*  And set this one to the end, so we'll know when we're done!  */
    end = nitf_List_end(record->images);

    for (count = 0; count < numImages; ++count)
    {
        nitf_ImageSegment *imageSegment =
            (nitf_ImageSegment *) nitf_ListIterator_get(&iter);

        nitf_ImageReader *deserializer =
            nitf_Reader_newImageReader(reader, count, &e);

        if (!deserializer)
        {
            nitf_Error_print(&e, stderr, "Couldnt spawn deserializer");
            exit(EXIT_FAILURE);
        }
        
        printf("Writing image %d... ", count);

        /*  Write the thing out  */
        writeImage(imageSegment, inputFile, deserializer, count,
                   rowSkipFactor, columnSkipFactor, optz, &e);

        nitf_ImageReader_destruct(&deserializer);
        
        printf("done.\n");

        /*  Increment the iterator so we can continue  */
        nitf_ListIterator_increment(&iter);
    }

    nitf_Record_destruct(&record);
    nitf_Reader_destruct(&reader);
    nitf_IOHandle_close(io);

    return 0;
}
