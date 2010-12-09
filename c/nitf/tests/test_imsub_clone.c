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

#define SHOW(X) printf("%s=[%s]\n", #X, X)
#define SHOWI(X) printf("%s=[%ld]\n", #X, X)
#define SHOWLL(X) printf("%s=[%lld]\n", #X, X)
#define SHOW_IMSUB(X) printf("Image Subheader [%s]:\n", #X); showImageSubheader(X)

#define SHOW_VAL(X) printf("%s=[%.*s]\n", #X, ((X==0)?8:((X->raw==0)?5:(int)X->length)), ((X==0)?"(nulptr)":((X->raw==0)?"(nul)":X->raw)))

void showFileHeader(nitf_FileHeader* header)
{
    unsigned int i;
    nitf_Uint32 num;
    nitf_Error error;
    nitf_Uint32 len;
    nitf_Uint64 dataLen;

    SHOW_VAL( header->fileHeader );
    SHOW_VAL( header->fileVersion );
    SHOW_VAL( header->complianceLevel );
    SHOW_VAL( header->systemType );
    SHOW_VAL( header->originStationID );
    SHOW_VAL( header->fileDateTime );
    SHOW_VAL( header->fileTitle );
    SHOW_VAL( header->classification );
    SHOW_VAL( header->messageCopyNum );
    SHOW_VAL( header->messageNumCopies );
    SHOW_VAL( header->encrypted );
    SHOW_VAL( header->backgroundColor );
    SHOW_VAL( header->originatorName );
    SHOW_VAL( header->originatorPhone );

    SHOW_VAL( header->fileLength );
    SHOW_VAL( header->headerLength );

    /*  Attention: If the classification is U, the security group  */
    /*  section should be empty!  For that reason, we wont print   */
    /*  The security group for now                                 */

    SHOW_VAL( header->securityGroup->classificationSystem );
    SHOW_VAL( header->securityGroup->codewords );
    SHOW_VAL( header->securityGroup->controlAndHandling );
    SHOW_VAL( header->securityGroup->releasingInstructions );
    SHOW_VAL( header->securityGroup->declassificationType );
    SHOW_VAL( header->securityGroup->declassificationDate );
    SHOW_VAL( header->securityGroup->declassificationExemption );
    SHOW_VAL( header->securityGroup->downgrade );
    SHOW_VAL( header->securityGroup->downgradeDateTime );
    SHOW_VAL( header->securityGroup->classificationText );
    SHOW_VAL( header->securityGroup->classificationAuthorityType );
    SHOW_VAL( header->securityGroup->classificationAuthority );
    SHOW_VAL( header->securityGroup->classificationReason );
    SHOW_VAL( header->securityGroup->securitySourceDate );
    SHOW_VAL( header->securityGroup->securityControlNumber );


    NITF_TRY_GET_UINT32(header->numImages, &num, &error);
    printf("The number of IMAGES contained in this file [%ld]\n", (long)num);
    for (i = 0; i < num; i++)
    {
        NITF_TRY_GET_UINT32(header->imageInfo[i]->lengthSubheader, &len, &error);
        NITF_TRY_GET_UINT64(header->imageInfo[i]->lengthData, &dataLen, &error);
        printf("\tThe length of IMAGE subheader [%d]: %ld bytes\n",
               i, (long)len);
        printf("\tThe length of the IMAGE data: %lu bytes\n\n",
               dataLen);
    }

    return;

CATCH_ERROR:
    printf("Error processing\n");
}

void showImageSubheader(nitf_ImageSubheader* imsub)
{
    int q; /* iterator */
    int nbands, xbands;
    nitf_Error error;
    nitf_ListIterator iter, end;

    printf("Read image into imsub\n");
    SHOW_VAL( imsub->filePartType );
    SHOW_VAL( imsub->imageId );
    SHOW_VAL( imsub->imageDateAndTime );
    SHOW_VAL( imsub->targetId );
    SHOW_VAL( imsub->imageTitle );
    SHOW_VAL( imsub->imageSecurityClass );
    SHOW_VAL( imsub->encrypted );
    SHOW_VAL( imsub->imageSource );
    SHOW_VAL( imsub->numRows );
    SHOW_VAL( imsub->numCols );
    SHOW_VAL( imsub->pixelValueType );
    SHOW_VAL( imsub->imageRepresentation );
    SHOW_VAL( imsub->imageCategory );
    SHOW_VAL( imsub->actualBitsPerPixel );
    SHOW_VAL( imsub->pixelJustification );
    SHOW_VAL( imsub->imageCoordinateSystem );
    SHOW_VAL( imsub->cornerCoordinates );

    SHOW_VAL( imsub->numImageComments );
    iter = nitf_List_begin(imsub->imageComments);
    end = nitf_List_end(imsub->imageComments);
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        nitf_Field* commentField = (nitf_Field*) nitf_ListIterator_get(&iter);
        SHOW_VAL(commentField);
        nitf_ListIterator_increment(&iter);
    }

    SHOW_VAL( imsub->imageCompression );
    SHOW_VAL( imsub->compressionRate );

    SHOW_VAL( imsub->numImageBands );
    SHOW_VAL( imsub->numMultispectralImageBands );

    NITF_TRY_GET_UINT32(imsub->numImageBands, &nbands, &error);
    NITF_TRY_GET_UINT32(imsub->numMultispectralImageBands, &xbands, &error);
    nbands += xbands;
    for (q = 0; q < nbands; q++)
    {
        SHOW_VAL( imsub->bandInfo[q]->representation);
        SHOW_VAL( imsub->bandInfo[q]->subcategory );
        SHOW_VAL( imsub->bandInfo[q]->imageFilterCondition );
        SHOW_VAL( imsub->bandInfo[q]->imageFilterCode );
        SHOW_VAL( imsub->bandInfo[q]->numLUTs );
        SHOW_VAL( imsub->bandInfo[q]->bandEntriesPerLUT );
    }

    /*  Skip band stuff for now  */
    SHOW_VAL( imsub->imageSyncCode );
    SHOW_VAL( imsub->imageMode );
    SHOW_VAL( imsub->numBlocksPerRow );
    SHOW_VAL( imsub->numBlocksPerCol );
    SHOW_VAL( imsub->numPixelsPerHorizBlock );
    SHOW_VAL( imsub->numPixelsPerVertBlock );
    SHOW_VAL( imsub->numBitsPerPixel );
    SHOW_VAL( imsub->imageDisplayLevel );
    SHOW_VAL( imsub->imageAttachmentLevel );
    SHOW_VAL( imsub->imageLocation );
    SHOW_VAL( imsub->imageMagnification );
    SHOW_VAL( imsub->userDefinedImageDataLength );
    SHOW_VAL( imsub->userDefinedOverflow );
    SHOW_VAL( imsub->extendedHeaderLength );
    SHOW_VAL( imsub->extendedHeaderOverflow );

    return;

CATCH_ERROR:
    printf("Error processing\n");

}

int main(int argc, char **argv)
{
    /*  This is the reader object  */
    nitf_Reader* reader;
    nitf_Record*   record;
    /*  The IO handle  */
    nitf_IOHandle io;
    /*  Get the error object       */
    nitf_Error     error;

    /*  Check argv and make sure we are happy  */
    if ( argc != 2 )
    {
        printf("Usage: %s <nitf-file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    io = nitf_IOHandle_create(argv[1],
                              NITF_ACCESS_READONLY,
                              NITF_OPEN_EXISTING,
                              &error);
    if ( NITF_INVALID_HANDLE( io ) )
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit( EXIT_FAILURE );
    }

    reader = nitf_Reader_construct(&error);
    if (!reader)
    {
        nitf_Error_print(&error, stdout, "Exiting (1) ...");
        exit( EXIT_FAILURE );
    }

    printf("Here are the loaded handlers\n");
    printf("* * * * * * * * * * * * * * * *\n");
    {
        nitf_PluginRegistry* reg = nitf_PluginRegistry_getInstance(&error);
        nitf_HashTable_print( reg->treHandlers);
    }
    printf("* * * * * * * * * * * * * * * *\n");

    record = nitf_Reader_read(reader, io, &error);
    if (!record)
    {
        nitf_Error_print(&error, stderr, "Failed to read the file");
        exit(EXIT_FAILURE);

    }

    /* Now show the header */
    showFileHeader(record->header);

    /* And now show the image information */
    if (record->header->numImages)
    {

        /*  Walk each image and show  */
        nitf_ListIterator iter = nitf_List_begin(record->images);
        nitf_ListIterator end  = nitf_List_end(record->images);

        while ( nitf_ListIterator_notEqualTo(&iter, &end) )
        {
            nitf_ImageSegment* segment =
                (nitf_ImageSegment*)nitf_ListIterator_get(&iter);

            nitf_ImageSubheader* dolly =
                nitf_ImageSubheader_clone(segment->subheader, &error);
            if (!dolly)
            {
                nitf_Error_print(&error, stdout, "During cloning!");
                exit(EXIT_FAILURE);
            }

            SHOW_IMSUB(segment->subheader);
            SHOW_IMSUB(dolly);

            nitf_ImageSubheader_destruct(&dolly);

            nitf_ListIterator_increment(&iter);

        }
    }
    else
    {
        printf("No image in file!\n");
    }

    nitf_IOHandle_close(io);
    nitf_Record_destruct(&record);
    nitf_Reader_destruct(&reader);

    return 0;
}
