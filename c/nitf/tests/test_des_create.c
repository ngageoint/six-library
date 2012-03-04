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
  Test program for creating a DE segment

  This program creates a NITF file with a file header and a single DE Segment.
  The record is cloned from an input NITF with either 0 subheaders or
  n image subheaders (anything else will fail to write properly)
  
  The DES data and user header fields are set and the result written out as
  a new NITF

  The calling sequence is:

    test_des_write inputFile outputFile

*/

static const char data[] = "123456789ABCDEF0";

#include <string.h>

#include <import/nitf.h>


char *makeBandName(const char *rootFile, const char* segment, int segmentNum, int bandNum)
{
    char *file = (char *) NITF_MALLOC(NITF_MAX_PATH);
    int pos;

    /* find end slash */
    for (pos = strlen(rootFile) - 1;
            pos && rootFile[pos] != '\\' && rootFile[pos] != '/'; pos--);

    if (bandNum >= 0)
        NITF_SNPRINTF(file, NITF_MAX_PATH, "%s__%s_%d_band_%d",
                      &rootFile[pos + 1], segment, segmentNum, bandNum);
    else
        NITF_SNPRINTF(file, NITF_MAX_PATH, "%s__%s_%d", &rootFile[pos + 1],\
                      segment, segmentNum);
    /* remove decimals */
    for (pos = strlen(file) - 1; pos; pos--)
    {
        if (file[pos] == '.')
        {
            file[pos] = '_';
        }
    }
    strcat(file, ".man");
    printf("File: %s\n", file);
    return file;
}

void freeBandName(char **rootFile)
{
    if (*rootFile)
    {
        NITF_FREE(*rootFile);
        *rootFile = NULL;

    }
}

void manuallyWriteImageBands(nitf_ImageSegment * segment,
                             const char *imageName,
                             nitf_ImageReader * deserializer,
                             int imageNumber, nitf_Error * error)
{
    char *file;
    nitf_Uint32 nBits, nBands, xBands, nRows, nColumns;
    size_t subimageSize;
    nitf_SubWindow *subimage;
    unsigned int i;
    int padded;
    nitf_Uint8 **buffer;
    nitf_Uint32 band;
    nitf_Uint32 *bandList;

    NITF_TRY_GET_UINT32(segment->subheader->numBitsPerPixel, &nBits,
                        error);
    NITF_TRY_GET_UINT32(segment->subheader->numImageBands, &nBands, error);
    NITF_TRY_GET_UINT32(segment->subheader->numMultispectralImageBands,
                        &xBands, error);
    nBands += xBands;
    NITF_TRY_GET_UINT32(segment->subheader->numRows, &nRows, error);
    NITF_TRY_GET_UINT32(segment->subheader->numCols, &nColumns, error);
    subimageSize = nRows * nColumns * NITF_NBPP_TO_BYTES(nBits);

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


    buffer = (nitf_Uint8 **) malloc(8 * nBands);
    band = 0;
    bandList = (nitf_Uint32 *) malloc(sizeof(nitf_Uint32 *) * nBands);

    subimage = nitf_SubWindow_construct(error);
    assert(subimage);

    subimage->startCol = 0;
    subimage->startRow = 0;
    subimage->numRows = nRows;
    subimage->numCols = nColumns;

    for (band = 0; band < nBands; band++)
        bandList[band] = band;
    subimage->bandList = bandList;
    subimage->numBands = nBands;

    assert(buffer);
    for (i = 0; i < nBands; i++)
    {
        buffer[i] = (nitf_Uint8 *) malloc(subimageSize);
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
        file = makeBandName(imageName, "img", imageNumber, i);
        toFile = nitf_IOHandle_create(file, NITF_ACCESS_WRITEONLY,
                                      NITF_CREATE, error);
        freeBandName(&file);
        if (NITF_INVALID_HANDLE(toFile))
        {
            goto CATCH_ERROR;

        }
        if (!nitf_IOHandle_write(toFile,
                                 (const char *) buffer[i],
                                 subimageSize, error))

        {
            goto CATCH_ERROR;
        }
        nitf_IOHandle_close(toFile);
    }

    /* free buffers */
    for (i = 0; i < nBands; i++)
    {
        free(buffer[i]);
    }
    free(buffer);
    free(bandList);
    nitf_SubWindow_destruct(&subimage);
    return;

CATCH_ERROR:
    /* free buffers */
    for (i = 0; i < nBands; i++)
    {
        free(buffer[i]);
    }
    free(buffer);
    free(bandList);
    nitf_Error_print(error, stderr, "Manual write failed");


}

nitf_ImageSource *setupBands(int nbands, int imageNum,
                             const char *inRootFile)
{
    nitf_Error error;
    int i;
    nitf_BandSource *bandSource;
    nitf_ImageSource *iSource = nitf_ImageSource_construct(&error);
    if (!iSource)
        goto CATCH_ERROR;
    for (i = 0; i < nbands; i++)
    {
        char *inFile = makeBandName(inRootFile, "img", imageNum, i);
        nitf_IOHandle sourceHandle =
            nitf_IOHandle_create(inFile, NITF_ACCESS_READONLY,
                                 NITF_OPEN_EXISTING, &error);
        if (NITF_INVALID_HANDLE(sourceHandle))
            goto CATCH_ERROR;

        freeBandName(&inFile);

        bandSource = nitf_FileSource_construct(sourceHandle,
                                               0, 0 /*gets ignored */ , 0,
                                               &error);
        if (!bandSource)
        {
            goto CATCH_ERROR;
        }
        if (!nitf_ImageSource_addBand(iSource, bandSource, &error))
        {
            goto CATCH_ERROR;
        }
    }
    return iSource;

CATCH_ERROR:
    nitf_Error_print(&error, stderr, "While constructing image source");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{

    nitf_ImageWriter *iWriter = NULL;
    nitf_ImageSource *iSource = NULL;
    nitf_ListIterator iter;
    nitf_ListIterator end;
    nitf_Reader *reader = NULL;
    nitf_Record *record = NULL;
    nitf_IOHandle in;
    nitf_DESegment *des = NULL;
    nitf_FileSecurity *security = NULL;
    int bad, i;
    nitf_IOHandle out;
    nitf_Writer *writer = NULL;
    nitf_SegmentWriter *desWriter = NULL;
    nitf_SegmentSource *desData = NULL;
    nitf_Error error;
    int numImages;

    if (argc != 3)
    {
        fprintf(stderr, "Usage %s inputFile outputFile\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /*    Get the input record */

    in = nitf_IOHandle_create(argv[1],
                              NITF_ACCESS_READONLY, NITF_OPEN_EXISTING,
                              &error);
    if (NITF_INVALID_HANDLE(in))
    {
        nitf_Error_print(&error, stderr, "Error opening input ");
        exit(EXIT_FAILURE);
    }

    reader = nitf_Reader_construct(&error);
    if (!reader)
    {
        nitf_IOHandle_close(in);
        nitf_Error_print(&error, stderr, "Error creating reader ");
        exit(EXIT_FAILURE);
    }

    record = nitf_Reader_read(reader, in, &error);
    if (!record)
    {
        nitf_Error_print(&error, stderr, "Error reading input ");
        goto CATCH_ERROR;
    }


    //nitf_Reader_destruct(&reader);
    if (!nitf_Field_get
            (record->header->numImages, &numImages, NITF_CONV_INT,
             NITF_INT32_SZ, &error))
    {
        nitf_Error_print(&error, stderr, "nitf::Value::get() failed");
        goto CATCH_ERROR;
    }


    if (record->images)
    {
        int count;
        nitf_ImageSegment* imageSegment;
        nitf_ImageReader* deserializer;
        end = nitf_List_end(record->images);

        for (count = 0; count < numImages; ++count)
        {
            iter = nitf_List_at(record->images, count);
            if (nitf_ListIterator_equals(&iter, &end))
            {
                printf("Out of bounds on iterator [%d]!\n", count);
                exit(EXIT_FAILURE);
            }
            imageSegment = (nitf_ImageSegment *) nitf_ListIterator_get(&iter);
            deserializer = nitf_Reader_newImageReader(reader, count, &error);
            if (!deserializer)
            {
                nitf_Error_print(&error, stderr, "Couldnt spawn deserializer");
                exit(EXIT_FAILURE);
            }
            printf("Writing image %d... ", count);
            /*  Write the thing out  */
            manuallyWriteImageBands(imageSegment, argv[1], deserializer, count,
                                    &error);

            nitf_ImageReader_destruct(&deserializer);

            printf("done.\n");
            /*  Increment the iterator so we can continue  */
            nitf_ListIterator_increment(&iter);
        }
    }


    des = nitf_Record_newDataExtensionSegment(record, &error);
    if (!des)
    {
        nitf_Record_destruct(&record);
        nitf_Error_print(&error, stderr, "Error creating new DE segment ");
        exit(EXIT_FAILURE);
    }

    security = nitf_FileSecurity_clone(record->header->securityGroup, &error);
    nitf_FileSecurity_destruct(&(des->subheader->securityGroup));
    des->subheader->securityGroup = security;

    

    des->subheader->subheaderFields = 
        nitf_TRE_construct("TEST DES", "TEST DES", &error);
    if (!des->subheader->subheaderFields)
    {
        nitf_Record_destruct(&record);
        nitf_Error_print(&error, stderr, "No error creating new DE segment ");
        exit(EXIT_FAILURE);
    }

/*     nitf_TRE_setField(des->subheader->subheaderFields,  */
/*                       "raw_data",  */
/*                       data, */
/*                       16,  */
/*                       &error); */


    nitf_TRE_setField(des->subheader->subheaderFields, 
                      "TEST_DES_COUNT", "16", 2, &error);
    nitf_TRE_setField(des->subheader->subheaderFields, 
                      "TEST_DES_START", "065", 3, &error);
    nitf_TRE_setField(des->subheader->subheaderFields, 
                      "TEST_DES_INCREMENT", "01", 2, &error);

    nitf_Field_setString(des->subheader->NITF_DE, "DE", &error);
    nitf_Field_setString(des->subheader->NITF_DESTAG, "TEST DES", &error);
    nitf_Field_setString(des->subheader->NITF_DESVER, "01", &error);
    nitf_Field_setString(des->subheader->NITF_DESCLAS, "U", &error);


    out = nitf_IOHandle_create(argv[2], NITF_ACCESS_WRITEONLY, NITF_CREATE, &error);

    writer = nitf_Writer_construct(&error);
    if (!writer)
    {
        nitf_Error_print(&error, stderr, "Write setup failed ");
        goto CATCH_ERROR;
    }

    if (!nitf_Writer_prepare(writer, record, out, &error))
    {
        nitf_Error_print(&error, stderr, "Write setup failed ");
        goto CATCH_ERROR;
    }

    if (record->images)
    {
        end = nitf_List_end(record->images);
        for (i = 0; i < numImages; i++)
        {
            int nbands;
            nitf_ImageSegment *imseg = NULL;
            iter = nitf_List_at(record->images, i);
            assert(nitf_ListIterator_notEqualTo(&iter, &end));

            imseg = (nitf_ImageSegment *) nitf_ListIterator_get(&iter);
            assert(imseg);

            if (!nitf_Field_get
                    (imseg->subheader->numImageBands, &nbands, NITF_CONV_INT,
                     NITF_INT32_SZ, &error))
                goto CATCH_ERROR;

            iWriter = nitf_Writer_newImageWriter(writer, i, &error);
            if (!iWriter)
            {
                goto CATCH_ERROR;
            }
            iSource = setupBands(nbands, i, argv[1]);
            if (!iSource)
                goto CATCH_ERROR;
            if (!nitf_ImageWriter_attachSource(iWriter, iSource, &error))
                goto CATCH_ERROR;
        }
    }

    //nitf_IOHandle_close(in);
    
    /*  Create the DE segment */


    if (NITF_INVALID_HANDLE(out))
    {
        nitf_Error_print(&error, stderr, "Write setup failed ");
        goto CATCH_ERROR;

    }

    /*   Setup write */


    desWriter = nitf_Writer_newDEWriter(writer, 0, &error);
    if (desWriter == NULL)
    {
        nitf_Error_print(&error, stderr, "Write setup failed ");
        goto CATCH_ERROR;

    }

    desData = nitf_SegmentMemorySource_construct(data, strlen(data), 
                                                 0, 0, 0, &error);
    if (desData == NULL)
    {
        nitf_Error_print(&error, stderr, "Write setup failed ");
        goto CATCH_ERROR;

    }

    if (!nitf_SegmentWriter_attachSource(desWriter, desData, &error))
    {        
        nitf_Error_print(&error, stderr, "Write setup failed ");
        goto CATCH_ERROR;
    }

    if (!nitf_Writer_write(writer, &error))
    {
        nitf_Error_print(&error, stderr, "Write setup failed ");
        goto CATCH_ERROR;
    }
 CATCH_ERROR:
    /*    Clean-up */
    if (!NITF_INVALID_HANDLE(in))
        nitf_IOHandle_close(in);
    if (!NITF_INVALID_HANDLE(in))
        nitf_IOHandle_close(out);
    
    if (writer)
        nitf_Writer_destruct(&writer);
    if (record)
        nitf_Record_destruct(&record);
    if (reader)
        nitf_Reader_destruct(&reader);
    return 0;
}
