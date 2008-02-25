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

void writeImage(nitf_ImageSegment * segment,
                char *imageName,
                nitf_ImageReader * deserializer,
                int imageNumber,
                nitf_Uint32 rowSkipFactor,
                nitf_Uint32 columnSkipFactor,
                nitf_Error * error)
{

    nitf_Uint32 nBits, nBands, xBands, nRows, nColumns;
    size_t subimageSize;
    nitf_SubWindow *subimage;
    unsigned int i;
    int padded;
    nitf_Uint8 **buffer;
    nitf_Uint32 band;
    nitf_Uint32 *bandList;

    nitf_DownSampler *pixelSkip;
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
           segment->subheader->pixelValueType->length,
           segment->subheader->pixelValueType->raw,
           segment->subheader->numBitsPerPixel->length,
           segment->subheader->numBitsPerPixel->raw,
           segment->subheader->actualBitsPerPixel->length,
           segment->subheader->actualBitsPerPixel->raw,
           segment->subheader->pixelJustification->length,
           segment->subheader->pixelJustification->raw,
           segment->subheader->imageMode->length,
           segment->subheader->imageMode->raw,
           segment->subheader->numBlocksPerRow->length,
           segment->subheader->numBlocksPerRow->raw,
           segment->subheader->numBlocksPerCol->length,
           segment->subheader->numBlocksPerCol->raw,
           segment->subheader->numPixelsPerHorizBlock->length,
           segment->subheader->numPixelsPerHorizBlock->raw,
           segment->subheader->numPixelsPerVertBlock->length,
           segment->subheader->numPixelsPerVertBlock->raw,
           segment->subheader->imageCompression->length,
           segment->subheader->imageCompression->raw,
           segment->subheader->compressionRate->length,
           segment->subheader->compressionRate->raw);

	/* for RGB24 images, let's just write out the 1 band
	 * - this will be MUCH faster
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
		printf("**Using optimization to write out a 3-band RGB pix-interleaved image**\n");
	}

    subimage = nitf_SubWindow_construct(error);
    assert(subimage);

    buffer = (nitf_Uint8 **) NITF_MALLOC(nBands * sizeof(nitf_Uint8*));
    band = 0;
    bandList = (nitf_Uint32 *) NITF_MALLOC(sizeof(nitf_Uint32 *) * nBands);
    subimage->startCol = 0;
    subimage->startRow = 0;
    subimage->numRows = nRows / rowSkipFactor;
    subimage->numCols = nColumns / columnSkipFactor;

    pixelSkip = nitf_PixelSkip_construct(rowSkipFactor, columnSkipFactor, error);
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
        sprintf(file, "%s_%d__%d_%d_%d_band_%d", &imageName[pos],
                imageNumber, nRows / rowSkipFactor, nColumns / columnSkipFactor,
                nBits, i);
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
                             "nitf::IOHandle::create() failed");
            goto CATCH_ERROR;
        }
        if (!nitf_IOHandle_write(toFile,
                                 (const char *) buffer[i],
                                 subimageSize, error))
        {
            nitf_Error_print(error, stderr, "nitf::IOHandle::print failed");
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


void writeImageSegment(char *imageName,
                       nitf_SegmentReader * reader,
                       int imageNumber,
                       nitf_Error * error)
{
    nitf_Uint8 *buffer = NULL;
    nitf_IOHandle toFile;
    char file[NITF_MAX_PATH];
    int pos;
    off_t size = nitf_SegmentReader_getSize(reader, error);
    printf("Segment Size: %d\n", size);
    buffer = (nitf_Uint8 *) NITF_MALLOC(size);
    assert(buffer);
    
    if (!nitf_SegmentReader_read(reader, buffer, size, error))
    {
        nitf_Error_print(error, stderr, "Read failed");
        goto CATCH_ERROR;
    }
    
    /* find end slash */
    for (pos = strlen(imageName) - 1;
            pos && imageName[pos] != '\\' && imageName[pos] != '/'; pos--);
    pos = pos == 0 ? pos : pos + 1;
    sprintf(file, "%s_%d", &imageName[pos], imageNumber);
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
        nitf_Error_print(error, stderr, "nitf::IOHandle::create() failed");
        goto CATCH_ERROR;
    }
    if (!nitf_IOHandle_write(toFile, (const char *) buffer, size, error))
    {
        nitf_Error_print(error, stderr, "nitf::IOHandle::print failed");
        goto CATCH_ERROR;
    }
    nitf_IOHandle_close(toFile);
    NITF_FREE(buffer);
    return;

CATCH_ERROR:
    NITF_FREE(buffer);
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

    /*  If you didnt give us a nitf file, we're croaking  */
    if (argc != 2)
    {
        printf("Usage: %s <nitf-file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    reader = nitf_Reader_construct(&e);
    if (!reader)
    {
        nitf_Error_print(&e, stderr, "nitf::Reader::construct() failed");
        exit(EXIT_FAILURE);
    }

    /*  If you did, though, we'll be nice and open it for you  */
    io = nitf_IOHandle_create(argv[1],
                              NITF_ACCESS_READONLY,
                              NITF_OPEN_EXISTING, &e);

    /*  But, oh boy, if you gave us a bad location...!  */
    if (NITF_INVALID_HANDLE(io))
    {
        /*  You had this coming!  */
        nitf_Error_print(&e, stderr, "nitf::IOHandle::create() failed");
        exit(EXIT_FAILURE);
    }

    /*  Read the file  */
    record = nitf_Reader_read(reader, io, &e);
    if (!record)
    {
        nitf_Error_print(&e, stderr, "nitf::Reader::read() failed");
        exit(EXIT_FAILURE);
    }

    if (!nitf_Field_get
            (record->header->numImages, &numImages, NITF_CONV_INT,
             NITF_INT32_SZ, &e))
    {
        nitf_Error_print(&e, stderr, "nitf::Value::get() failed");
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
        writeImage(imageSegment, argv[1], deserializer, count,
                   rowSkipFactor, columnSkipFactor, &e);
        nitf_ImageReader_destruct(&deserializer);
        
        /*writeImageSegment(argv[1], deserializer, count, &e);
        nitf_SegmentReader_destruct(&deserializer);*/

        printf("done.\n");
        /*  Increment the iterator so we can continue  */
        nitf_ListIterator_increment(&iter);
    }

    nitf_Record_destruct(&record);
    if (!NITF_IO_SUCCESS(nitf_IOHandle_seek(io, 0, NITF_SEEK_SET, &e)))
    {
        nitf_Error_print(&e, stderr, "On second read");
    }
    else
    {
        record = nitf_Reader_read(reader, io, &e);
        if (!record)
            nitf_Error_print(&e, stderr, "On parse of second read");
        else
        {
            printf("Second read worked\n");
            nitf_Record_destruct(&record);
        }
    }

    nitf_Reader_destruct(&reader);
    nitf_IOHandle_close(io);

    return 0;
}
