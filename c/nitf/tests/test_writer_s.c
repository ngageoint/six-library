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
**
**  This version uses a nitf_ImageSource (memory version) to supply the
**  data to the writer (The memory buffers are filled using nitf_ImageIO_read)
**
** ********************************************************************/


#define SHOW(X) printf("%s=[%s]\n", #X, X)
#define SHOWI(X) printf("%s=[%ld]\n", #X, X)
#define SHOWLL(X) printf("%s=[%lld]\n", #X, X)
#define SHOW_VAL(X) printf("%s=[%.*s]\n", #X, ((X==0)?8:((X->raw==0)?5:X->length)), ((X==0)?"(nulptr)":((X->raw==0)?"(nul)":X->raw)))

#define GET_UINT32(FIELD, DATA_PTR, ERROR) \
    success = nitf_Field_get(FIELD, DATA_PTR, NITF_CONV_UINT, NITF_INT32_SZ, ERROR); \
    if (!success) goto CATCH_ERROR;

#define GET_UINT64(FIELD, DATA_PTR, ERROR) \
    success = nitf_Field_get(FIELD, DATA_PTR, NITF_CONV_UINT, NITF_INT64_SZ, ERROR); \
    if (!success) goto CATCH_ERROR;

void showFileHeader(nitf_FileHeader* header)
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
    printf("The number of IMAGES contained in this file [%ld]\n", num);
    for (i = 0; i < num; i++)
    {
        GET_UINT32(header->imageInfo[i]->lengthSubheader, &len, &error);
        GET_UINT64(header->imageInfo[i]->lengthData, &dataLen, &error);
        printf("\tThe length of IMAGE subheader [%d]: %ld bytes\n",
               i, len);
        printf("\tThe length of the IMAGE data: %lld bytes\n\n",
               dataLen);
    }

    return;

CATCH_ERROR:
    printf("Error processing\n");
}

int doWrite(nitf_ImageSegment* segment, nitf_ImageSource *imgSrc,
            nitf_IOHandle output_io, nitf_ImageIO* ioClone);

NITF_BOOL writeImage(nitf_ImageSegment* segment, nitf_IOHandle input_io, nitf_IOHandle output_io)
{
    nitf_Error error;
    nitf_Off offset;
    int ret;

    nitf_ImageIO* ioClone;

    nitf_Uint8** buffer;
    nitf_Uint32 nBits, nBands, xBands, nRows, nColumns;
    nitf_SubWindow *subimage;
    size_t subimageSize;
    nitf_Uint32 band;
    nitf_Uint32 *bandList;
    NITF_BOOL success;
    int padded;

    nitf_ImageSource *imgSrc;  /* Image source object */
    nitf_BandSource *bandSrc;   /* Current band source object */


    /* clone the imageIO */
    ioClone = nitf_ImageIO_clone(segment->imageIO, &error);
    if (!ioClone)
    {
        nitf_Error_print(&error, stderr, "Clone failed");
        goto CATCH_ERROR;
    }

    /* get IO offset, and set the offset for the ImageIO */
    offset = nitf_IOHandle_tell(output_io, &error);
    if (!NITF_IO_SUCCESS(offset)) goto CATCH_ERROR;

    if (!nitf_ImageIO_setFileOffset(segment->imageIO, offset, &error))
    {
        goto CATCH_ERROR;
    }

    /*   Read image */

    GET_UINT32(segment->subheader->numBitsPerPixel, &nBits, &error);
    GET_UINT32(segment->subheader->numImageBands, &nBands, &error);
    GET_UINT32(segment->subheader->numMultispectralImageBands, &xBands, &error);
    nBands += xBands;
    GET_UINT32(segment->subheader->numRows, &nRows, &error);
    GET_UINT32(segment->subheader->numCols, &nColumns, &error);
    subimageSize = nRows * nColumns * NITF_NBPP_TO_BYTES(nBits);


    /* Allcoate buffers */
    buffer = (nitf_Uint8 **)malloc(8 * nBands);
    assert(buffer);

    for (band = 0; band < nBands; band++)
    {
        buffer[band] = (nitf_Uint8*)malloc(subimageSize);
        assert(buffer[band]);
    }

    /*  Set-up band array and subimage */

    bandList = (nitf_Uint32 *)malloc(sizeof(nitf_Uint32 *) * nBands);

    subimage = nitf_SubWindow_construct(&error);
    assert(subimage);

    subimage->startCol = 0;
    subimage->startRow = 0;
    subimage->numRows = nRows;
    subimage->numCols = nColumns;

    for (band = 0; band < nBands; band++)
    {
        bandList[band] = band;
    }
    subimage->bandList = bandList;
    subimage->numBands = nBands;

    /*  Read data */

    if (!nitf_ImageIO_read(ioClone,
                           input_io, subimage, buffer, &padded, &error) )
    {
        nitf_Error_print(&error, stderr, "Read failed");
        return(0);
    }
    free(bandList);

    /* Setup for image source */

    imgSrc = nitf_ImageSource_construct(&error);
    if (imgSrc == NULL)
        return(0);
    for (band = 0; band < nBands; band++)
    {
        bandSrc = nitf_MemorySource_construct(buffer[band], (size_t) subimageSize,
                                              (nitf_Off) 0, NITF_NBPP_TO_BYTES(nBits), 0, &error);
        if (bandSrc == NULL)
            return(0);

        if (!nitf_ImageSource_addBand(imgSrc, bandSrc, &error))
            return(0);
    }

    /* Do write */

    ret = doWrite(segment, imgSrc, output_io, ioClone);

    if (ioClone) nitf_ImageIO_destruct(&ioClone);
    nitf_ImageSource_destruct(&imgSrc);

    /* OK return */
    return ret;

CATCH_ERROR:
    if (ioClone) nitf_ImageIO_destruct(&ioClone);
    printf("ERROR processing\n");
    return 0;
}

/*==========*/

int doWrite(nitf_ImageSegment* segment, nitf_ImageSource *imgSrc,
            nitf_IOHandle output_io, nitf_ImageIO* ioClone)
{
    nitf_Error error;
    nitf_Uint8** user;
    nitf_Uint32 nBits, nBands, xBands, nRows, nColumns, row;
    size_t rowSize;
    int band;
    NITF_BOOL success; /* Used in GET macros */
    nitf_BandSource *bandSrc;   /* Current band source object */
    nitf_ImageSource *imgSrcTmp;
    GET_UINT32(segment->subheader->numBitsPerPixel, &nBits, &error);
    GET_UINT32(segment->subheader->numImageBands, &nBands, &error);
    GET_UINT32(segment->subheader->numMultispectralImageBands, &xBands, &error);
    nBands += xBands;
    GET_UINT32(segment->subheader->numRows, &nRows, &error);
    GET_UINT32(segment->subheader->numCols, &nColumns, &error);
    rowSize = nColumns * NITF_NBPP_TO_BYTES(nBits);



    /*    Allocate buffers */

    user = (nitf_Uint8 **)malloc(8 * nBands);
    assert(user);
    for (band = 0; band < nBands; band++)
    {
        user[band] = (nitf_Uint8*)malloc(rowSize);
        assert(user[band]);
    }


    /* setup for write */
    nitf_ImageIO_writeSequential(segment->imageIO, output_io, &error);

    /* loop over the rows */
    for (row = 0; row < nRows; ++row)
    {
        imgSrcTmp = imgSrc;
        /* set the band pointer */
        for (band = 0; band < nBands; ++band)
        {
            bandSrc = nitf_ImageSource_getBand(imgSrcTmp, band, &error);
            if (bandSrc == NULL)
                return(0);

            (*(bandSrc->iface->read))(bandSrc->data, (char*)user[band],
                                      (size_t) rowSize, &error);
        }

        /* write the row */
        if (!nitf_ImageIO_writeRows(segment->imageIO, output_io, 1, user, &error))
        {
            return(0);
        }
    }

    /* done writing */
    if (!nitf_ImageIO_writeDone(segment->imageIO, output_io, &error))
    {
        return(0);
    }

    free(user);

    return(1);
CATCH_ERROR:
    return(0);    /* Needed for the GET macros */
}

/*==========*/

int main(int argc, char **argv)
{
    nitf_Reader* reader;                /* The reader object */
    nitf_Writer* writer;                /* The writer object */
    nitf_Record* record;                /* a record object */
    nitf_Record* record2;
    nitf_ListIterator iter;             /* current pos iterator */
    nitf_ListIterator end;              /* end of list iterator */
    nitf_ImageSegment* segment;         /* the image segment */

    NITF_BOOL success;                  /* status bool */

    nitf_IOHandle input_io;             /* input IOHandle */
    nitf_IOHandle output_io;            /* output IOHandle */
    nitf_Error error;                   /* error object */

    /*  Check argv and make sure we are happy  */
    if (argc != 3)
    {
        printf("Usage: %s <input-file> <output-file> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    input_io = nitf_IOHandle_create(argv[1],
                                    NITF_ACCESS_READONLY,
                                    NITF_OPEN_EXISTING, &error);
    if ( NITF_INVALID_HANDLE(input_io))
    {
        goto CATCH_ERROR;
    }

    output_io = nitf_IOHandle_create(argv[2],
                                     NITF_ACCESS_WRITEONLY,
                                     NITF_CREATE | NITF_TRUNCATE,
                                     &error);

    if ( NITF_INVALID_HANDLE(output_io))
    {
        goto CATCH_ERROR;
    }

    reader = nitf_Reader_construct(&error);
    if (!reader)
    {
        goto CATCH_ERROR;
    }

    writer = nitf_Writer_construct(&error);
    if (!writer)
    {
        goto CATCH_ERROR;
    }

    record = nitf_Record_construct(&error);
    if (!record)
    {
        goto CATCH_ERROR;
    }

    assert(nitf_Reader_read(reader, input_io, record, &error));
    showFileHeader(record->header);

    /*  Write to the file  */
    success = nitf_Writer_writeHeader(writer, record->header, output_io, &error);
    if (!success) goto CATCH_ERROR;


    /*  ------ IMAGES ------  */
    iter = nitf_List_begin(record->images);
    end  = nitf_List_end(record->images);
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        /*  Cast it to an imageSegment...  */
        segment = (nitf_ImageSegment*)nitf_ListIterator_get(&iter);

        /* write the image subheader */
        if (!nitf_Writer_writeImageSubheader(writer,
                                             segment->subheader,
                                             record->header->NITF_FVER->raw,
                                             output_io,
                                             &error))
        {
            goto CATCH_ERROR;
        }

        /* Now, write the image */
        if (!writeImage(segment, input_io, output_io))
        {
            goto CATCH_ERROR;
        }

        nitf_ListIterator_increment(&iter);
    }

    nitf_IOHandle_close(input_io);
    nitf_IOHandle_close(output_io);
    nitf_Record_destruct(&record);
    nitf_Writer_destruct(&writer);

    /*  Open the file we just wrote to, and dump it to screen */
    input_io = nitf_IOHandle_create(argv[2],
                                    NITF_ACCESS_READONLY,
                                    NITF_OPEN_EXISTING,
                                    &error);
    if (NITF_INVALID_HANDLE(input_io))
    {
        goto CATCH_ERROR;
    }

    record2 = nitf_Record_construct(&error);
    if (!record2)
    {
        goto CATCH_ERROR;
    }

    assert(nitf_Reader_readHeader(reader, input_io, record2, &error));
    showFileHeader(record2->header);

    nitf_IOHandle_close(input_io);
    nitf_Record_destruct(&record2);
    nitf_Reader_destruct(&reader);

    return 0;

CATCH_ERROR:
    if (input_io) nitf_IOHandle_close(input_io);
    if (output_io) nitf_IOHandle_close(output_io);
    if (record2) nitf_Record_destruct(&record2);
    if (reader) nitf_Reader_destruct(&reader);
    if (record) nitf_Record_destruct(&record);
    if (writer) nitf_Writer_destruct(&writer);
    nitf_Error_print(&error, stdout, "Exiting...");
    exit(EXIT_FAILURE);
}

