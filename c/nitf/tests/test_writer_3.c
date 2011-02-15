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


/* *********************************************************************
** This test tests the round-trip process of taking an input NITF
** file and writing it to a new file. This includes writing the image
** segments (headers, extensions, and image data). This is an example
** of how users can write the image data to their NITF file
** ********************************************************************/

nitf_Record *doRead(const char *inFile);

#define SHOW(X) printf("%s=[%s]\n", #X, X)
#define SHOWI(X) printf("%s=[%ld]\n", #X, X)
#define SHOWLL(X) printf("%s=[%lld]\n", #X, X)
#define SHOW_VAL(X) printf("%s=[%.*s]\n", #X, ((X==0)?8:((X->raw==0)?5:(int)X->length)), ((X==0)?"(nulptr)":((X->raw==0)?"(nul)":X->raw)))

#define GET_UINT32(FIELD, DATA_PTR, ERROR) \
    success = nitf_Field_get(FIELD, DATA_PTR, NITF_CONV_UINT, NITF_INT32_SZ, ERROR); \
    if (!success) goto CATCH_ERROR;

#define GET_UINT64(FIELD, DATA_PTR, ERROR) \
    success = nitf_Field_get(FIELD, DATA_PTR, NITF_CONV_UINT, NITF_INT64_SZ, ERROR); \
    if (!success) goto CATCH_ERROR;

/* pass in a negative number for bandNum if you don't want a band in the name */
char *makeBandName(const char *rootFile, const char* segment, int segmentNum, int bandNum)
{
    char *file = (char *) NITF_MALLOC(NITF_MAX_PATH);
    int pos;

    /* find end slash */
    for (pos = strlen(rootFile) - 1;
            pos && rootFile[pos] != '\\' && rootFile[pos] != '/'; pos--);

    if (bandNum >= 0)
        NITF_SNPRINTF(file, NITF_MAX_PATH,
                      "%s__%s_%d_band_%d", &rootFile[pos + 1],
                      segment, segmentNum, bandNum);
    else
        NITF_SNPRINTF(file, NITF_MAX_PATH, "%s__%s_%d", &rootFile[pos + 1],
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
void showFileHeader(nitf_FileHeader * header)
{
    unsigned int i;
    nitf_Uint32 num;
    nitf_Error error;
    nitf_Uint32 len;
    nitf_Uint64 dataLen;
    NITF_BOOL success;

    SHOW_VAL(header->fileHeader);
    SHOW_VAL(header->fileVersion);
    SHOW_VAL(header->complianceLevel);
    SHOW_VAL(header->systemType);
    SHOW_VAL(header->originStationID);
    SHOW_VAL(header->fileDateTime);
    SHOW_VAL(header->fileTitle);
    SHOW_VAL(header->classification);
    SHOW_VAL(header->messageCopyNum);
    SHOW_VAL(header->messageNumCopies);
    SHOW_VAL(header->encrypted);
    SHOW_VAL(header->backgroundColor);
    SHOW_VAL(header->originatorName);
    SHOW_VAL(header->originatorPhone);

    SHOW_VAL(header->fileLength);
    SHOW_VAL(header->headerLength);

    /*  Attention: If the classification is U, the security group  */
    /*  section should be empty!  For that reason, we wont print   */
    /*  The security group for now                                 */

    SHOW_VAL(header->securityGroup->classificationSystem);
    SHOW_VAL(header->securityGroup->codewords);
    SHOW_VAL(header->securityGroup->controlAndHandling);
    SHOW_VAL(header->securityGroup->releasingInstructions);
    SHOW_VAL(header->securityGroup->declassificationType);
    SHOW_VAL(header->securityGroup->declassificationDate);
    SHOW_VAL(header->securityGroup->declassificationExemption);
    SHOW_VAL(header->securityGroup->downgrade);
    SHOW_VAL(header->securityGroup->downgradeDateTime);
    SHOW_VAL(header->securityGroup->classificationText);
    SHOW_VAL(header->securityGroup->classificationAuthorityType);
    SHOW_VAL(header->securityGroup->classificationAuthority);
    SHOW_VAL(header->securityGroup->classificationReason);
    SHOW_VAL(header->securityGroup->securitySourceDate);
    SHOW_VAL(header->securityGroup->securityControlNumber);

    GET_UINT32(header->numImages, &num, &error);
    printf("The number of IMAGES contained in this file [%ld]\n", (long)num);
    for (i = 0; i < num; i++)
    {
        GET_UINT32(header->imageInfo[i]->lengthSubheader, &len, &error);
        GET_UINT64(header->imageInfo[i]->lengthData, &dataLen, &error);
        printf("\tThe length of IMAGE subheader [%d]: %ld bytes\n",
               i, (long)len);
        printf("\tThe length of the IMAGE data: %lu bytes\n\n", dataLen);
    }

    return;

CATCH_ERROR:
    printf("Error processing\n");
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
        freeBandName(&inFile);

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

void doWrite(nitf_Record * record, char *inRootFile, char *outFile)
{
    nitf_ListIterator iter;

    nitf_ImageWriter *iWriter;
    nitf_ImageSource *iSource;

    nitf_SegmentWriter *segmentWriter;
    nitf_SegmentSource *segmentSource;

    nitf_ListIterator end;
    int i;
    int numImages;
    int numTexts;
    int numDataExtensions;
    nitf_Writer *writer = NULL;
    nitf_Error error;
    nitf_IOHandle output_io = nitf_IOHandle_create(outFile,
                              NITF_ACCESS_WRITEONLY,
                              NITF_CREATE,
                              &error);

    if (NITF_INVALID_HANDLE(output_io))
    {
        goto CATCH_ERROR;
    }

    writer = nitf_Writer_construct(&error);
    if (!writer)
    {
        goto CATCH_ERROR;
    }
    if (!nitf_Writer_prepare(writer, record, output_io, &error))
    {
        goto CATCH_ERROR;
    }

    if (!nitf_Field_get
            (record->header->numImages, &numImages, NITF_CONV_INT,
             NITF_INT32_SZ, &error))
    {
        nitf_Error_print(&error, stderr, "nitf::Value::get() failed");
        numImages = 0;
    }

    if (!nitf_Field_get
            (record->header->numTexts, &numTexts, NITF_CONV_INT,
             NITF_INT32_SZ, &error))
    {
        nitf_Error_print(&error, stderr, "nitf::Value::get() failed");
        numTexts = 0;
    }

    if (!nitf_Field_get
            (record->header->numDataExtensions, &numDataExtensions, NITF_CONV_INT,
             NITF_INT32_SZ, &error))
    {
        nitf_Error_print(&error, stderr, "nitf::Value::get() failed");
        numDataExtensions = 0;
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
            iSource = setupBands(nbands, i, inRootFile);
            if (!iSource)
                goto CATCH_ERROR;
            if (!nitf_ImageWriter_attachSource(iWriter, iSource, &error))
                goto CATCH_ERROR;
        }
    }

    if (record->texts)
    {
        end = nitf_List_end(record->texts);
        for (i = 0; i < numTexts; i++)
        {
            nitf_TextSegment *textSeg = NULL;
            char *inFile = NULL;
            nitf_IOHandle sourceHandle;

            iter = nitf_List_at(record->texts, i);
            assert(nitf_ListIterator_notEqualTo(&iter, &end));

            textSeg = (nitf_TextSegment *) nitf_ListIterator_get(&iter);
            assert(textSeg);

            segmentWriter = nitf_Writer_newTextWriter(writer, i, &error);
            if (!segmentWriter)
            {
                goto CATCH_ERROR;
            }

            /* setup file */
            inFile = makeBandName(inRootFile, "text", i, -1);
            sourceHandle =
                nitf_IOHandle_create(inFile, NITF_ACCESS_READONLY,
                                     NITF_OPEN_EXISTING, &error);
            if (NITF_INVALID_HANDLE(sourceHandle))
                goto CATCH_ERROR;

            freeBandName(&inFile);

            segmentSource = nitf_SegmentFileSource_construct(sourceHandle, 0, 0, &error);
            if (!segmentSource)
                goto CATCH_ERROR;
            if (!nitf_SegmentWriter_attachSource(segmentWriter, segmentSource, &error))
                goto CATCH_ERROR;
        }
    }

    if (record->dataExtensions)
    {
        end = nitf_List_end(record->dataExtensions);
        for (i = 0; i < numDataExtensions; i++)
        {
            nitf_DESegment *DESeg = NULL;
            char *inFile = NULL;
            nitf_IOHandle sourceHandle;

            iter = nitf_List_at(record->dataExtensions, i);
            assert(nitf_ListIterator_notEqualTo(&iter, &end));

            DESeg = (nitf_DESegment *) nitf_ListIterator_get(&iter);
            assert(DESeg);

            segmentWriter = nitf_Writer_newDEWriter(writer, i, &error);
            if (!segmentWriter)
            {
                goto CATCH_ERROR;
            }

            /* setup file */
            inFile = makeBandName(inRootFile, "DE", i, -1);
            sourceHandle =
                nitf_IOHandle_create(inFile, NITF_ACCESS_READONLY,
                                     NITF_OPEN_EXISTING, &error);
            if (NITF_INVALID_HANDLE(sourceHandle))
                goto CATCH_ERROR;

            freeBandName(&inFile);

            segmentSource = nitf_SegmentFileSource_construct(sourceHandle, 0, 0, &error);
            if (!segmentSource)
                goto CATCH_ERROR;
            if (!nitf_SegmentWriter_attachSource(segmentWriter, segmentSource, &error))
                goto CATCH_ERROR;

        }
    }

    if (!nitf_Writer_write(writer, &error))
    {
        goto CATCH_ERROR;
    }


    /*nitf_ImageSource_destruct(&iSource);*/
    nitf_IOHandle_close(output_io);
    nitf_Writer_destruct(&writer);
    return;
CATCH_ERROR:
    nitf_Error_print(&error, stderr, "During write");
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{

    nitf_Record *record = NULL; /* a record object */
#if 0
    nitf_ListIterator iter;     /* current pos iterator */
    nitf_ListIterator end;      /* end of list iterator */
    nitf_ImageSegment *segment = NULL;  /* the image segment */

    NITF_BOOL success;          /* status bool */

    nitf_IOHandle input_io;     /* input IOHandle */
    nitf_IOHandle output_io;    /* output IOHandle */
#endif

    /*  Check argv and make sure we are happy  */
    if (argc != 3)
    {
        printf("Usage: %s <input-file> <output-file> \n", argv[0]);
        exit(EXIT_FAILURE);
    }
    record = doRead(argv[1]);

    showFileHeader(record->header);
    doWrite(record, argv[1], argv[2]);
    nitf_Record_destruct(&record);

    return 0;
}

/* this writes the text data from a text segment to a file */
void writeTextData(nitf_TextSegment * segment,
                   const char *fileName,
                   nitf_SegmentReader * reader,
                   int textNumber, nitf_Error * error)
{

    size_t toRead;
    char * buf = NULL;
    char * outName = NULL;

    nitf_IOHandle file;

    toRead = (size_t)(segment->end - segment->offset);

    buf = (char*)NITF_MALLOC(toRead + 1);
    if (!buf)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return;
    }

    /* get the data */
    if (nitf_SegmentReader_read(reader, buf, toRead, error) != NITF_SUCCESS)
    {
        /* TODO populate error */
        goto CATCH_ERROR;
    }

    outName = makeBandName(fileName, "text", textNumber, -1);
    file = nitf_IOHandle_create(outName, NITF_ACCESS_WRITEONLY,
                                NITF_CREATE, error);
    freeBandName(&outName);

    if (NITF_INVALID_HANDLE(file))
    {
        goto CATCH_ERROR;
    }
    if (!nitf_IOHandle_write(file, (const char*)buf, toRead, error))
        goto CATCH_ERROR;
    nitf_IOHandle_close(file);

CATCH_ERROR:
    if (buf) NITF_FREE(buf);
    return;
}

/*XXX */
/* this writes the data extension data from a data segment to a file */
void writeDEData(nitf_DESegment * segment,
                 const char *fileName,
                 nitf_SegmentReader * reader,
                 int DENumber, nitf_Error * error)
{

#define DE_READ_SIZE 16*1024

    /*XXX
        size_t toRead = DE_READ_SIZE;
        size_t leftToRead;
        size_t amtToRead;
    */
    nitf_Uint64 toRead = DE_READ_SIZE;
    nitf_Uint64 leftToRead;
    nitf_Uint64 amtToRead;
    char * buf = NULL;
    char * outName = NULL;

    nitf_IOHandle file;

    leftToRead = (size_t)(segment->end - segment->offset); 
    //leftToRead = (size_t) reader->user->virtualLength;
    fprintf(stderr, "XXX Data Ext write %lu %lu %lu\n", leftToRead, segment->end , segment->offset);

    buf = (char*)NITF_MALLOC(toRead + 1);
    if (!buf)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return;
    }
    fprintf(stderr, "XXX Data Ext A\n");

    /*  Make file output file anme and create */
    outName = makeBandName(fileName, "DE", DENumber, -1);
    fprintf(stderr, "XXX Data Ext names %s %s\n", fileName, outName);
    file = nitf_IOHandle_create(outName, NITF_ACCESS_WRITEONLY,
                                NITF_CREATE, error);
    freeBandName(&outName);
    fprintf(stderr, "XXX Data Ext B\n");

    if (NITF_INVALID_HANDLE(file))
    {
        goto CATCH_ERROR;
    }

    /* get the data and write to file */
    while (leftToRead > 0)
    {
        fprintf(stderr, "XXX Data Ext C\n");
        amtToRead = DE_READ_SIZE;
        if (amtToRead > leftToRead)
            amtToRead = leftToRead;
        fprintf(stderr, "XXX Data Ext C2 %lu\n", amtToRead);
        if (nitf_SegmentReader_read(reader, buf, (size_t)amtToRead, error) != NITF_SUCCESS)
        {
            /* TODO populate error */
            goto CATCH_ERROR;
        }

        fprintf(stderr, "XXX Data Ext D %lu\n", amtToRead);
        if (!nitf_IOHandle_write(file, (const char*)buf, (size_t)amtToRead, error))
            goto CATCH_ERROR;
        fprintf(stderr, "XXX Data Ext E\n");

        leftToRead -= amtToRead;
    }
    nitf_IOHandle_close(file);

CATCH_ERROR:
    if (buf) NITF_FREE(buf);
    return;
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
    nitf_Uint8 **buffer = NULL;
    nitf_Uint32 band;
    nitf_Uint32 *bandList = NULL;

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


    buffer = (nitf_Uint8 **) malloc(sizeof(nitf_Uint8*) * nBands);
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
    /*  This should change to returning failures!  */
    /*
       if (! nitf_ImageIO_read(segment->imageIO, io, &subimage, buffer, &padded,
       error) )
       {
       nitf_Error_print(error, stderr, "Read failed");
       goto CATCH_ERROR;
       }
     */
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
nitf_Record *doRead(const char *inFile)
{
    /*  This is the error we hopefully wont receive  */
    nitf_Error e;

    /*  This is the reader  */
    nitf_Reader *reader;

    /*  This is the record of the file we are reading  */
    nitf_Record *record;

    /*  This is the io handle we will give the reader to parse  */
    nitf_IOHandle io;

    int count = 0;
    int numImages;
    int numTexts;
    int numDataExtensions;

    nitf_ListIterator iter;
    nitf_ListIterator end;


    nitf_ImageSegment *imageSegment = NULL;
    nitf_ImageReader *deserializer = NULL;
    nitf_TextSegment *textSegment = NULL;
    nitf_DESegment *deSegment = NULL;
    nitf_SegmentReader *segmentReader = NULL;
    nitf_SegmentReader *deReader = NULL;

    reader = nitf_Reader_construct(&e);
    if (!reader)
    {
        nitf_Error_print(&e, stderr, "nitf::Reader::construct() failed");
        exit(EXIT_FAILURE);
    }

    /*  If you did, though, we'll be nice and open it for you  */
    io = nitf_IOHandle_create(inFile,
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
        nitf_Error_print(&e, stderr, "nitf::Field::get() failed");
        numImages = 0;
    }

    if (!nitf_Field_get
            (record->header->numTexts, &numTexts, NITF_CONV_INT,
             NITF_INT32_SZ, &e))
    {
        nitf_Error_print(&e, stderr, "nitf::Field::get() failed");
        numTexts = 0;
    }

    if (!nitf_Field_get
            (record->header->numDataExtensions, &numDataExtensions, NITF_CONV_INT,
             NITF_INT32_SZ, &e))
    {
        nitf_Error_print(&e, stderr, "nitf::Field::get() failed");
        numDataExtensions = 0;
    }

    if (record->images)
    {
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
            deserializer = nitf_Reader_newImageReader(reader, count, &e);
            if (!deserializer)
            {
                nitf_Error_print(&e, stderr, "Couldnt spawn deserializer");
                exit(EXIT_FAILURE);
            }
            printf("Writing image %d... ", count);
            /*  Write the thing out  */
            manuallyWriteImageBands(imageSegment, inFile, deserializer, count,
                                    &e);

            nitf_ImageReader_destruct(&deserializer);

            printf("done.\n");
            /*  Increment the iterator so we can continue  */
            nitf_ListIterator_increment(&iter);
        }
    }


    /* loop over texts and read the data to a file */
    if (record->texts)
    {
        end = nitf_List_end(record->texts);

        for (count = 0; count < numTexts; ++count)
        {
            iter = nitf_List_at(record->texts, count);
            if (nitf_ListIterator_equals(&iter, &end))
            {
                printf("Out of bounds on iterator [%d]!\n", count);
                exit(EXIT_FAILURE);
            }
            textSegment = (nitf_TextSegment *) nitf_ListIterator_get(&iter);
            segmentReader = nitf_Reader_newTextReader(reader, count, &e);
            if (!segmentReader)
            {
                nitf_Error_print(&e, stderr, "Couldnt spawn deserializer");
                exit(EXIT_FAILURE);
            }
            printf("Writing text %d... ", count);
            /*  Write the thing out  */
            writeTextData(textSegment, inFile, segmentReader, count, &e);
            nitf_SegmentReader_destruct(&segmentReader);

            /*  Increment the iterator so we can continue  */
            nitf_ListIterator_increment(&iter);
        }
    }

    /*XXX*/
    /* loop over data extensions and read the data to a file */
    if (record->dataExtensions)
    {
        fprintf(stderr, "XXX Data Ext %d\n", numDataExtensions);
        end = nitf_List_end(record->dataExtensions);

        for (count = 0; count < numDataExtensions;  ++count)
        {
            iter = nitf_List_at(record->dataExtensions, count);
            if (nitf_ListIterator_equals(&iter, &end))
            {
                printf("Out of bounds on iterator [%d]!\n", count);
                exit(EXIT_FAILURE);
            }
            deSegment = (nitf_DESegment *) nitf_ListIterator_get(&iter);
            deReader = nitf_Reader_newDEReader(reader, count, &e);
            if (!deReader)
            {
                nitf_Error_print(&e, stderr, "Couldnt spawn deserializer");
                exit(EXIT_FAILURE);
            }
            printf("Writing data extension %d... ", count);
            /*  Write the thing out  */
            writeDEData(deSegment, inFile, deReader, count, &e);
            nitf_SegmentReader_destruct(&segmentReader);

            /*  Increment the iterator so we can continue  */
            nitf_ListIterator_increment(&iter);
        }
    }
    nitf_Reader_destruct(&reader);
    return record;

}
