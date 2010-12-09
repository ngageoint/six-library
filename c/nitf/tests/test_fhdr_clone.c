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

#define TRY_IT(X) if (!X) goto CATCH
#define SHOW(X) printf("%s=[%s]\n", #X, X)
#define SHOWI(X) printf("%s=[%ld]\n", #X, X)
#define PRINT_HDR(X) printf("%s:\n", #X); printHdr(X)
#define SHOW_VAL(X) printf("%s=[%.*s]\n", #X, ((X==0)?8:((X->raw==0)?5:(int)X->length)), ((X==0)?"(nulptr)":((X->raw==0)?"(nul)":X->raw)))

void printHdr(nitf_FileHeader* header)
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

    NITF_TRY_GET_UINT32(header->numGraphics, &num, &error);
    printf("The number of GRAPHICS contained in this file [%ld]\n", (long)num);
    for (i = 0; i < num; i++)
    {
        NITF_TRY_GET_UINT32(header->graphicInfo[i]->lengthSubheader, &len, &error);
        NITF_TRY_GET_UINT64(header->graphicInfo[i]->lengthData, &dataLen, &error);
        printf("\tThe length of GRAPHIC subheader [%d]: %ld bytes\n",
               i, (long)len);
        printf("\tThe length of the GRAPHIC data: %d bytes\n\n",
               (int)dataLen);
    }

    NITF_TRY_GET_UINT32(header->numLabels, &num, &error);
    printf("The number of LABELS contained in this file [%ld]\n", (long)num);
    for (i = 0; i < num; i++)
    {
        NITF_TRY_GET_UINT32(header->labelInfo[i]->lengthSubheader, &len, &error);
        NITF_TRY_GET_UINT64(header->labelInfo[i]->lengthData, &dataLen, &error);
        printf("\tThe length of LABEL subheader [%d]: %ld bytes\n",
               i, (long)len);
        printf("\tThe length of the LABEL data: %d bytes\n\n",
               (int)dataLen);
    }

    NITF_TRY_GET_UINT32(header->numTexts, &num, &error);
    printf("The number of TEXTS contained in this file [%ld]\n", (long)num);
    for (i = 0; i < num; i++)
    {
        NITF_TRY_GET_UINT32(header->textInfo[i]->lengthSubheader, &len, &error);
        NITF_TRY_GET_UINT64(header->textInfo[i]->lengthData, &dataLen, &error);
        printf("\tThe length of TEXT subheader [%d]: %ld bytes\n",
               i, (long)len);
        printf("\tThe length of the TEXT data: %d bytes\n\n",
               (int)dataLen);
    }

    NITF_TRY_GET_UINT32(header->numDataExtensions, &num, &error);
    printf("The number of DATA EXTENSIONS contained in this file [%ld]\n", (long)num);
    for (i = 0; i < num; i++)
    {
        NITF_TRY_GET_UINT32(header->dataExtensionInfo[i]->lengthSubheader, &len, &error);
        NITF_TRY_GET_UINT64(header->dataExtensionInfo[i]->lengthData, &dataLen, &error);
        printf("\tThe length of DATA EXTENSION subheader [%d]: %d bytes\n",
               i, (int)len);
        printf("\tThe length of the DATA EXTENSION data: %d bytes\n\n",
               (int)dataLen);
    }

    NITF_TRY_GET_UINT32(header->numReservedExtensions, &num, &error);
    printf("The number of RESERVED EXTENSIONS contained in this file [%ld]\n", (long)num);
    for (i = 0; i < num; i++)
    {
        NITF_TRY_GET_UINT32(header->reservedExtensionInfo[i]->lengthSubheader, &len, &error);
        NITF_TRY_GET_UINT64(header->reservedExtensionInfo[i]->lengthData, &dataLen, &error);
        printf("\tThe length of RESERVED EXTENSION subheader [%d]: %d bytes\n",
               i, (int)len);
        printf("\tThe length of the RESERVED EXTENSION data: %d bytes\n\n",
               (int)dataLen);
    }

    NITF_TRY_GET_UINT32(header->userDefinedHeaderLength, &num, &error);
    printf("The user-defined header length [%ld]\n", (long)num);

    NITF_TRY_GET_UINT32(header->extendedHeaderLength, &num, &error);
    printf("The extended header length [%ld]\n", (long)num);

    return;

CATCH_ERROR:
    printf("Error processing\n");
}



int main(int argc, char** argv)
{
    nitf_IOHandle io;
    nitf_Reader* reader;
    nitf_Record* record = NULL;
    nitf_FileHeader* twin;
    nitf_Error error;

    if (argc != 2)
    {
        printf("Usage: %s <nitf-file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    io = nitf_IOHandle_create(argv[1],
                              NITF_ACCESS_READONLY,
                              NITF_OPEN_EXISTING,
                              &error);
    TRY_IT(  (NITF_INVALID_HANDLE( io ))  == 0);

    reader = nitf_Reader_construct(&error);
    TRY_IT(  reader  );


    record = nitf_Reader_read(reader, io, &error);
    if (!record)
        goto CATCH;

    twin = nitf_FileHeader_clone(record->header, &error);
    TRY_IT( twin );

    PRINT_HDR( record->header );
    PRINT_HDR( twin );
    return 0;

CATCH:
    nitf_Error_print(&error, stdout, "main()");
    exit(EXIT_FAILURE);

}
