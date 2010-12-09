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

void writeImage(nitf_ImageSegment* segment,
                nitf_IOHandle io,
                char* imageName,
                nitf_Record* record,
                int imageNumber,
                nitf_Error* error)
{

    nitf_Uint32 nBits, nBands, xBands, nRows, nColumns;
    size_t subimageSize;
    nitf_SubWindow *subimage;
    unsigned int i;

    int padded;
    nitf_Uint8** buffer;
    nitf_Uint32 band;
    nitf_Uint32 *bandList;
    char file[NITF_MAX_PATH];

    nitf_IOHandle toFile;

    NITF_TRY_GET_UINT32(segment->subheader->numBitsPerPixel, &nBits, error );
    NITF_TRY_GET_UINT32(segment->subheader->numImageBands, &nBands, error );
    NITF_TRY_GET_UINT32(segment->subheader->numMultispectralImageBands, &xBands, error );
    nBands += xBands;
    NITF_TRY_GET_UINT32(segment->subheader->numRows, &nRows, error  );
    NITF_TRY_GET_UINT32(segment->subheader->numCols, &nColumns, error );

    printf("Image number: %d\n", imageNumber);
    printf("\nCLEVEL -> %.*s\n"
           "NBANDS -> %d\n"
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
           record->header->complianceLevel->length,
           record->header->complianceLevel->raw,
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

    if (nBands != 1)
    {
        printf("This is a single band test case, but the image is multi-band.  Exiting...\n");
        exit(EXIT_FAILURE);
    }
    buffer = (nitf_Uint8 **)malloc(sizeof(nitf_Uint8)); // Malloc one dimension
    band = 0;
    bandList = (nitf_Uint32 *)malloc(sizeof(nitf_Uint32));

    subimage = nitf_SubWindow_construct(&error);
    assert(subimage);

    subimage->startCol = 0;
    subimage->startRow = 0;
    subimage->numRows = 1;/*nRows;*/
    subimage->numCols = nColumns;
    /*subimageSize = nRows * nColumns * NITF_NBPP_TO_BYTES(nBits);*/
    subimageSize = nColumns * NITF_NBPP_TO_BYTES(nBits);
    printf("Subimage size = %d, nBytes = %d\n",
           subimageSize,
           NITF_NBPP_TO_BYTES(nBits));
    for (band = 0; band < nBands; band++)
        bandList[band] = band;
    subimage->bandList = bandList;
    subimage->numBands = nBands;

    assert(buffer);
    buffer[0] = (nitf_Uint8*)malloc(subimageSize);


    /* find end slash */
    for (i = strlen(imageName) - 1;
            i && imageName[i] != '\\' && imageName[i] != '/';
            i--);

    NITF_SNPRINTF(file, NITF_MAX_PATH, "%s_%d__%d_%d_%d_per_line_1band",
                  &imageName[i + 1], imageNumber, nRows, nColumns, nBits);


    for (i = strlen(file) - 1; i; i--)
    {
        if (file[i] == '.')
        {
            file[i] = '_';
        }
    }
    strcat(file, ".out");

    printf("Filename: %s\n", file);

    toFile = nitf_IOHandle_create(file, NITF_ACCESS_WRITEONLY,
                                  NITF_CREATE | NITF_TRUNCATE,
                                  error);
    if (! NITF_IO_SUCCESS(toFile))
    {
        nitf_Error_print(error, stderr, "nitf::IOHandle::create() failed");
        goto CATCH_ERROR;

    }


    for (i = 0; i < nRows; i++)
    {
        subimage->startRow = i;
        /*  This should change to returning failures!  */
        printf("Reading one line... ");
        if (!nitf_ImageIO_read(segment->imageIO,
                               io,
                               subimage,
                               buffer,
                               &padded,
                               error) )
        {
            printf("failed!\n");
            nitf_Error_print(error, stderr, "Read failed");

            goto CATCH_ERROR;
        }

        printf("done\n");

        printf("Writing one line... ");
        if (!nitf_IOHandle_write(toFile,
                                 (const char*)buffer[0],
                                 subimageSize, error))

        {
            printf("failed!\n");
            nitf_Error_print(error,
                             stderr,
                             "write failed");
            break;
        }
        printf("done\n");

    }



    nitf_IOHandle_close(toFile);

    free(buffer[0]);
    free(buffer);
    free(bandList);

    return;

CATCH_ERROR:
    free(buffer[0]);
    free(buffer);
    free(bandList);
    printf("ERROR processing\n");

}
int main(int argc, char **argv)
{

    /*  This is the error we hopefully wont receive  */
    nitf_Error         e;

    /*  This is the reader  */
    nitf_Reader*     reader;

    /*  This is the record of the file we are reading  */
    nitf_Record*       record;


    /*  This is the io handle we will give the reader to parse  */
    nitf_IOHandle      io;

    int count = 0;


    /*  These iterators are for going through the image segments  */
    nitf_ListIterator  iter;
    nitf_ListIterator  end;

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

    record = nitf_Record_construct(&e);
    if (!record)
    {
        nitf_Error_print(&e, stderr, "nitf::Record::construct() failed");
        exit(EXIT_FAILURE);
    }

    /*  If you did, though, we'll be nice and open it for you  */
    io = nitf_IOHandle_create(argv[1], NITF_ACCESS_READONLY,
                              NITF_OPEN_EXISTING, &e);

    /*  But, oh boy, if you gave us a bad location...!  */
    if ( NITF_INVALID_HANDLE( io ) )
    {
        /*  You had this coming!  */
        nitf_Error_print(&e, stderr, "nitf::IOHandle::create() failed");
        exit(EXIT_FAILURE);
    }

    /*  Read the file  */
    if (! nitf_Reader_read(reader,
                           io,
                           record,
                           &e) )
    {
        nitf_Error_print(&e, stderr, "nitf::Reader::read() failed");
        exit(EXIT_FAILURE);
    }

    /*  Set the iterator to traverse the list of image segments  */
    iter = nitf_List_begin(record->images);
    /*  And set this one to the end, so we'll know when we're done!  */
    end  = nitf_List_end(record->images);

    /*  While we are not done...  */
    while ( nitf_ListIterator_notEqualTo(&iter, &end) )
    {

        /*  Get the image segment as its proper object  */
        nitf_ImageSegment* imageSegment =
            (nitf_ImageSegment*)nitf_ListIterator_get(&iter);

        printf("Writing image %d... ", count);

        /*  Write the thing out  */
        writeImage(imageSegment, io, argv[1], record, count, &e);
        printf("done.\n");
        /*  Increment the iterator so we can continue  */
        nitf_ListIterator_increment(&iter);
        count++;
    }

    nitf_Record_destruct(&record);
    nitf_Reader_destruct(&reader);
    nitf_IOHandle_close(io);

    return 0;
}
