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
#include "nitf/TREUtils.h"
#define SHOW(X) printf("%s=[%s]\n", #X, ((X==0)?"(nul)":X))
#define SHOWI(X) printf("%s=[%ld]\n", #X, X)
#define SHOWLL(X) printf("%s=[%lld]\n", #X, X)
#define SHOWRGB(X) printf("%s(R,G,B)=[%02x,%02x,%02x]\n", #X, (unsigned char) X[0], (unsigned char) X[1], (unsigned char) X[2])
#define SHOW_VAL(X) printf("%s=[%.*s]\n", #X, ((X==0)?8:((X->raw==0)?5:X->length)), ((X==0)?"(nulptr)":((X->raw==0)?"(nul)":X->raw)))

int showTRE(nitf_HashTable * ht, nitf_Pair * pair, NITF_DATA* userData,
        nitf_Error * error)
{
    if (pair)
    {
        if (pair->key)
        {
            nitf_ListIterator iter, end;

            iter = nitf_List_begin((nitf_List *) pair->data);
            end = nitf_List_end((nitf_List *) pair->data);

            while (nitf_ListIterator_notEqualTo(&iter, &end))
            {
                int i = 0;
                nitf_Uint32 treLength;
                nitf_TREEnumerator* it;
                nitf_TRE *tre = (nitf_TRE *) nitf_ListIterator_get(&iter);
                
                treLength = tre->handler->getCurrentSize(tre, error);
                
                printf("\n--------------- %s TRE (%d) ---------------\n",
                        pair->key, treLength);

                for (it = nitf_TRE_begin(tre, error); it != NULL; it->next(&it,
                        error) )
                {
                    nitf_Pair* fieldPair;
                    i++;

                    fieldPair = it->get(it, error);
                    if (fieldPair)
                    {
                        printf("%s = [", fieldPair->key);
                        nitf_Field_print((nitf_Field *) fieldPair->data);
                        printf("]\n");
                    }
                    else
                    {
                        printf("Warning, no field found!\n");
                    }

                }
                printf("---------------------------------------------\n");
                nitf_ListIterator_increment(&iter);

            }
        }
    }
    else
    {
#if NITF_DEBUG_TRE
        printf("No pair defined at iter pos!\n");
#endif
    }
    return 1;
}

void showSecurityGroup(nitf_FileSecurity * securityGroup)
{
    if (!securityGroup)
    {
        printf("ERROR: security group = (nul)\n");
        return;
    }

    /*  Attention: If the classification is U, the security group  */
    /*  section should be empty!  (boring!)                        */

    SHOW_VAL(securityGroup->classificationSystem);
    SHOW_VAL(securityGroup->codewords);
    SHOW_VAL(securityGroup->controlAndHandling);
    SHOW_VAL(securityGroup->releasingInstructions);
    SHOW_VAL(securityGroup->declassificationType);
    SHOW_VAL(securityGroup->declassificationDate);
    SHOW_VAL(securityGroup->declassificationExemption);
    SHOW_VAL(securityGroup->downgrade);
    SHOW_VAL(securityGroup->downgradeDateTime);
    SHOW_VAL(securityGroup->classificationText);
    SHOW_VAL(securityGroup->classificationAuthorityType);
    SHOW_VAL(securityGroup->classificationAuthority);
    SHOW_VAL(securityGroup->classificationReason);
    SHOW_VAL(securityGroup->securitySourceDate);
    SHOW_VAL(securityGroup->securityControlNumber);
}


void showFileHeader(nitf_FileHeader * header)
{
    unsigned int i;
    nitf_Uint32 num;
    nitf_Error error;
    nitf_Uint32 len;
    nitf_Uint64 dataLen;
    nitf_Uint32 dataLen32;

    if (!header)
    {
        printf("ERROR: file header = (nul)\n");
        return;
    }

    SHOW_VAL(header->fileHeader);
    SHOW_VAL(header->fileVersion);
    SHOW_VAL(header->complianceLevel);
    SHOW_VAL(header->systemType);
    SHOW_VAL(header->originStationID);
    SHOW_VAL(header->fileDateTime);
    SHOW_VAL(header->fileTitle);
    SHOW_VAL(header->classification);

    if (*(header->classification->raw) != 'U')
        showSecurityGroup(header->securityGroup);

    SHOW_VAL(header->messageCopyNum);
    SHOW_VAL(header->messageNumCopies);
    SHOW_VAL(header->encrypted);
    SHOW_VAL(header->backgroundColor);
    SHOW_VAL(header->originatorName);
    SHOW_VAL(header->originatorPhone);
    SHOW_VAL(header->fileLength);
    SHOW_VAL(header->headerLength);

    if (!nitf_Field_get(header->numImages,
                        &num, NITF_CONV_INT, NITF_INT32_SZ, &error))
        goto CATCH_ERROR;


    printf("The number of IMAGES contained in this file [%ld]\n", num);
    for (i = 0; i < num; i++)
    {

        if (!nitf_Field_get(header->imageInfo[i]->lengthSubheader,
                            &len, NITF_CONV_INT, NITF_INT32_SZ, &error))
            goto CATCH_ERROR;

        if (!nitf_Field_get(header->imageInfo[i]->lengthData,
                            &dataLen,
                            NITF_CONV_INT, NITF_INT64_SZ, &error))
            goto CATCH_ERROR;

        printf("\tThe length of IMAGE subheader [%d]: %ld bytes\n",
               i, len);
        printf("\tThe length of the IMAGE data: %lld bytes\n\n", dataLen);
    }

    if (!nitf_Field_get(header->numGraphics,
                        &num, NITF_CONV_INT, NITF_INT32_SZ, &error))
        goto CATCH_ERROR;


    printf("The number of GRAPHICS contained in this file [%ld]\n", num);
    for (i = 0; i < num; i++)
    {

        if (!nitf_Field_get(header->graphicInfo[i]->lengthSubheader,
                            &len, NITF_CONV_INT, NITF_INT32_SZ, &error))
            goto CATCH_ERROR;

        if (!nitf_Field_get(header->graphicInfo[i]->lengthData,
                            &dataLen32,  /*XXX*/
                            NITF_CONV_INT, NITF_INT32_SZ, &error))
            goto CATCH_ERROR;

        printf("\tThe length of GRAPHIC subheader [%d]: %ld bytes\n",
               i, len);
        printf("\tThe length of the GRAPHIC data: %ld bytes\n\n",
               /*XXX               (long) dataLen); */
               (long) dataLen32);
    }

    if (!nitf_Field_get(header->numLabels,
                        &num, NITF_CONV_INT, NITF_INT32_SZ, &error))
        goto CATCH_ERROR;

    printf("The number of LABELS contained in this file [%ld]\n", num);
    for (i = 0; i < num; i++)
    {

        if (!nitf_Field_get(header->labelInfo[i]->lengthSubheader,
                            &len, NITF_CONV_INT, NITF_INT32_SZ, &error))
            goto CATCH_ERROR;

        if (!nitf_Field_get(header->labelInfo[i]->lengthData,
                            /*XXX                            &dataLen, */
                            &dataLen32,
                            NITF_CONV_INT, NITF_INT32_SZ, &error))
            goto CATCH_ERROR;
        printf("\tThe length of LABEL subheader [%d]: %ld bytes\n",
               i, len);
        printf("\tThe length of the LABEL data: %ld bytes\n\n",
               /*XXX               (long) dataLen); */
               (long) dataLen32);
    }

    if (!nitf_Field_get(header->numTexts,
                        &num, NITF_CONV_INT, NITF_INT32_SZ, &error))
        goto CATCH_ERROR;
    printf("The number of TEXTS contained in this file [%ld]\n", num);
    for (i = 0; i < num; i++)
    {

        if (!nitf_Field_get(header->textInfo[i]->lengthSubheader,
                            &len, NITF_CONV_INT, NITF_INT32_SZ, &error))
            goto CATCH_ERROR;

        if (!nitf_Field_get(header->textInfo[i]->lengthData,
                            /*XXX                            &dataLen, */
                            &dataLen32,
                            NITF_CONV_INT, NITF_INT32_SZ, &error))
            goto CATCH_ERROR;

        printf("\tThe length of TEXT subheader [%d]: %ld bytes\n", i, len);
        printf("\tThe length of the TEXT data: %ld bytes\n\n",
               /*XXX               (long) dataLen); */
               (long) dataLen32);
    }

    if (!nitf_Field_get(header->numDataExtensions,
                        &num, NITF_CONV_INT, NITF_INT32_SZ, &error))
        goto CATCH_ERROR;
    printf("The number of DATA EXTENSIONS contained in this file [%ld]\n",
           num);

    for (i = 0; i < num; i++)
    {


        if (!nitf_Field_get(header->dataExtensionInfo[i]->lengthSubheader,
                            &len, NITF_CONV_INT, NITF_INT32_SZ, &error))
            goto CATCH_ERROR;

        if (!nitf_Field_get(header->dataExtensionInfo[i]->lengthData,
                            /*XXX                            &dataLen, */
                            &dataLen32,
                            NITF_CONV_INT, NITF_INT32_SZ, &error))
            goto CATCH_ERROR;

        printf("\tThe length of DATA EXTENSION subheader [%d]: %d bytes\n",
               i, (int) len);
        printf("\tThe length of the DATA EXTENSION data: %ld bytes\n\n",
               /*XXX (long) dataLen); */
               (long) dataLen32);
    }


    if (!nitf_Field_get(header->numReservedExtensions,
                        &num, NITF_CONV_INT, NITF_INT32_SZ, &error))
        goto CATCH_ERROR;

    printf
    ("The number of RESERVED EXTENSIONS contained in this file [%ld]\n",
     num);
    for (i = 0; i < num; i++)
    {


        if (!nitf_Field_get
                (header->reservedExtensionInfo[i]->lengthSubheader, &len,
                 NITF_CONV_INT, NITF_INT32_SZ, &error))
            goto CATCH_ERROR;

        if (!nitf_Field_get(header->reservedExtensionInfo[i]->lengthData,
                            /*XXX                            &dataLen, */
                            &dataLen32,
                            NITF_CONV_INT, NITF_INT32_SZ, &error))
            goto CATCH_ERROR;

        printf
        ("\tThe length of RESERVED EXTENSION subheader [%d]: %d bytes\n",
         i, (int) len);
        printf("\tThe length of the RESERVED EXTENSION data: %ld bytes\n\n",
               /*XXX               (long) dataLen); */
               (long) dataLen32);
    }

    if (!nitf_Field_get(header->userDefinedHeaderLength, &num,
                        NITF_CONV_INT, NITF_INT32_SZ, &error))
        goto CATCH_ERROR;

    printf("The user-defined header length [%ld]\n", num);


    if (header->userDefinedSection)
    {
        nitf_HashTable_foreach(header->userDefinedSection->hash,
                               (NITF_HASH_FUNCTOR) showTRE, NULL, &error);
    }

    if (!nitf_Field_get(header->extendedHeaderLength, &num,
                        NITF_CONV_INT, NITF_INT32_SZ, &error))
        goto CATCH_ERROR;

    printf("The extended header length [%ld]\n", num);

    if (header->extendedSection)
    {
        nitf_HashTable_foreach(header->extendedSection->hash,
                               (NITF_HASH_FUNCTOR) showTRE, NULL, &error);
    }
    return;

CATCH_ERROR:
    printf("Error processing\n");
}

void showImageSubheader(nitf_ImageSubheader * sub)
{
    int q;                      /* iterator */
    int nbands, xbands;
    nitf_Error error;
    int ncomments;
    nitf_ListIterator iter, end;

    if (!sub)
    {
        printf("ERROR: image subheader = (nul)\n");
        return;
    }

    if (!nitf_Field_get(sub->numImageComments,
                        &ncomments, NITF_CONV_INT, NITF_INT32_SZ, &error))
    {
        goto CATCH_ERROR;
    }

    printf("Read image into imsub\n");
    SHOW_VAL(sub->filePartType);
    SHOW_VAL(sub->imageId);
    SHOW_VAL(sub->imageDateAndTime);
    SHOW_VAL(sub->targetId);
    SHOW_VAL(sub->imageTitle);
    SHOW_VAL(sub->imageSecurityClass);

    if (*(sub->imageSecurityClass->raw) != 'U')
        showSecurityGroup(sub->securityGroup);

    SHOW_VAL(sub->encrypted);
    SHOW_VAL(sub->imageSource);
    SHOW_VAL(sub->numRows);
    SHOW_VAL(sub->numCols);
    SHOW_VAL(sub->pixelValueType);
    SHOW_VAL(sub->imageRepresentation);
    SHOW_VAL(sub->imageCategory);
    SHOW_VAL(sub->actualBitsPerPixel);
    SHOW_VAL(sub->pixelJustification);
    SHOW_VAL(sub->imageCoordinateSystem);
    SHOW_VAL(sub->cornerCoordinates);

    SHOW_VAL(sub->numImageComments);

    iter = nitf_List_begin(sub->imageComments);
    end = nitf_List_end(sub->imageComments);
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        nitf_Field* commentField = (nitf_Field*) nitf_ListIterator_get(&iter);
        SHOW_VAL(commentField);
        nitf_ListIterator_increment(&iter);
    }

    SHOW_VAL(sub->imageCompression);
    SHOW_VAL(sub->compressionRate);

    SHOW_VAL(sub->numImageBands);
    SHOW_VAL(sub->numMultispectralImageBands);

    if (!nitf_Field_get(sub->numImageBands, &nbands,
                        NITF_CONV_INT, NITF_INT32_SZ, &error))
        goto CATCH_ERROR;


    if (!nitf_Field_get(sub->numMultispectralImageBands, &xbands,
                        NITF_CONV_INT, NITF_INT32_SZ, &error))
        goto CATCH_ERROR;


    nbands += xbands;
    for (q = 0; q < nbands; q++)
    {
        SHOW_VAL(sub->bandInfo[q]->representation);
        SHOW_VAL(sub->bandInfo[q]->subcategory);
        SHOW_VAL(sub->bandInfo[q]->imageFilterCondition);
        SHOW_VAL(sub->bandInfo[q]->imageFilterCode);
        SHOW_VAL(sub->bandInfo[q]->numLUTs);
        SHOW_VAL(sub->bandInfo[q]->bandEntriesPerLUT);
    }

    /*  Skip band stuff for now  */
    SHOW_VAL(sub->imageSyncCode);
    SHOW_VAL(sub->imageMode);
    SHOW_VAL(sub->numBlocksPerRow);
    SHOW_VAL(sub->numBlocksPerCol);
    SHOW_VAL(sub->numPixelsPerHorizBlock);
    SHOW_VAL(sub->numPixelsPerVertBlock);
    SHOW_VAL(sub->numBitsPerPixel);
    SHOW_VAL(sub->imageDisplayLevel);
    SHOW_VAL(sub->imageAttachmentLevel);
    SHOW_VAL(sub->imageLocation);
    SHOW_VAL(sub->imageMagnification);

    SHOW_VAL(sub->userDefinedImageDataLength);
    SHOW_VAL(sub->userDefinedOverflow);

    if (sub->userDefinedSection)
    {
        nitf_HashTable_foreach(sub->userDefinedSection->hash,
                               (NITF_HASH_FUNCTOR) showTRE, NULL, &error);
    }

    SHOW_VAL(sub->extendedHeaderLength);
    SHOW_VAL(sub->extendedHeaderOverflow);

    if (sub->extendedSection)
    {
        nitf_HashTable_foreach(sub->extendedSection->hash,
                               (NITF_HASH_FUNCTOR) showTRE, NULL, &error);
    }
    return;

CATCH_ERROR:
    printf("Error processing\n");

}

void showGraphicSubheader(nitf_GraphicSubheader * sub)
{
    nitf_Error error;

    if (!sub)
    {
        printf("ERROR: graphic subheader = (nul)\n");
        return;
    }

    printf("Read graphic into sub\n");
    SHOW_VAL(sub->filePartType);
    SHOW_VAL(sub->graphicID);
    SHOW_VAL(sub->name);
    SHOW_VAL(sub->securityClass);

    if (*(sub->securityClass->raw) != 'U')
        showSecurityGroup(sub->securityGroup);

    SHOW_VAL(sub->encrypted);
    SHOW_VAL(sub->stype);
    SHOW_VAL(sub->res1);
    SHOW_VAL(sub->displayLevel);
    SHOW_VAL(sub->attachmentLevel);
    SHOW_VAL(sub->location);
    SHOW_VAL(sub->bound1Loc);
    SHOW_VAL(sub->color);
    SHOW_VAL(sub->bound2Loc);
    SHOW_VAL(sub->res2);
    SHOW_VAL(sub->extendedHeaderLength);
    SHOW_VAL(sub->extendedHeaderOverflow);
    if (sub->extendedSection)
    {
        nitf_HashTable_foreach(sub->extendedSection->hash,
                               (NITF_HASH_FUNCTOR) showTRE, NULL, &error);
    }
}

void showLabelSubheader(nitf_LabelSubheader * sub)
{
    nitf_Error error;

    if (!sub)
    {
        printf("ERROR: label subheader = (nul)\n");
        return;
    }

    printf("Read label into sub\n");
    SHOW_VAL(sub->filePartType);
    SHOW_VAL(sub->labelID);
    SHOW_VAL(sub->securityClass);

    if (*(sub->securityClass->raw) != 'U')
        showSecurityGroup(sub->securityGroup);

    SHOW_VAL(sub->encrypted);
    SHOW_VAL(sub->fontStyle);
    SHOW_VAL(sub->cellWidth);
    SHOW_VAL(sub->cellHeight);
    SHOW_VAL(sub->displayLevel);
    SHOW_VAL(sub->attachmentLevel);
    SHOW_VAL(sub->locationRow);
    SHOW_VAL(sub->locationColumn);
    SHOWRGB(sub->textColor->raw);
    SHOWRGB(sub->backgroundColor->raw);
    SHOW_VAL(sub->extendedHeaderLength);
    SHOW_VAL(sub->extendedHeaderOverflow);
    if (sub->extendedSection)
    {
        nitf_HashTable_foreach(sub->extendedSection->hash,
                               (NITF_HASH_FUNCTOR) showTRE, NULL, &error);
    }
}

void showTextSubheader(nitf_TextSubheader * sub)
{
    nitf_Error error;

    if (!sub)
    {
        printf("ERROR: text subheader = (nul)\n");
        return;
    }

    printf("Read text into sub\n");
    SHOW_VAL(sub->filePartType);
    SHOW_VAL(sub->textID);
    SHOW_VAL(sub->attachmentLevel);
    SHOW_VAL(sub->dateTime);
    SHOW_VAL(sub->title);
    SHOW_VAL(sub->securityClass);

    if (*(sub->securityClass->raw) != 'U')
        showSecurityGroup(sub->securityGroup);

    SHOW_VAL(sub->encrypted);
    SHOW_VAL(sub->format);
    SHOW_VAL(sub->extendedHeaderLength);
    SHOW_VAL(sub->extendedHeaderOverflow);
    if (sub->extendedSection)
    {
        nitf_HashTable_foreach(sub->extendedSection->hash,
                               (NITF_HASH_FUNCTOR) showTRE, NULL, &error);
    }
}

void showDESubheader(nitf_DESubheader * sub)
{
    nitf_Error error;

    if (!sub)
    {
        printf("ERROR: DES subheader = (nul)\n");
        return;
    }

    printf("Read DE into sub\n");
    SHOW_VAL(sub->filePartType);
    SHOW_VAL(sub->typeID);
    SHOW_VAL(sub->version);
    SHOW_VAL(sub->securityClass);

    if (*(sub->securityClass->raw) != 'U')
        showSecurityGroup(sub->securityGroup);

    SHOW_VAL(sub->overflowedHeaderType);
    SHOW_VAL(sub->dataItemOverflowed);
    SHOW_VAL(sub->subheaderFieldsLength);
    nitf_TREUtils_print(sub->subheaderFields, &error);
    SHOWI(sub->dataLength);

    if (sub->userDefinedSection)
    {
        nitf_HashTable_foreach(sub->userDefinedSection->hash,
                               (NITF_HASH_FUNCTOR) showTRE, NULL, &error);
    }
}

void showRESubheader(nitf_RESubheader * sub)
{
    nitf_Error error;

    if (!sub)
    {
        printf("ERROR: RES subheader = (nul)\n");
        return;
    }

    printf("Read RE into sub\n");
    SHOW_VAL(sub->filePartType);
    SHOW_VAL(sub->typeID);
    SHOW_VAL(sub->version);
    SHOW_VAL(sub->securityClass);

    if (*(sub->securityClass->raw) != 'U')
        showSecurityGroup(sub->securityGroup);

    SHOW_VAL(sub->subheaderFieldsLength);
    //nitf_TREUtils_print(sub->, &error);
    SHOWI(sub->dataLength);
}

int main(int argc, char **argv)
{
    /*  Get the error object       */
    nitf_Error error;

    /*  This is the reader object  */
    nitf_Reader *reader;
    nitf_Record *record;
	
    /*  The IO handle  */
    nitf_IOHandle io;
    int num;

    /*  Check argv and make sure we are happy  */
    if (argc != 2)
    {
        printf("Usage: %s <nitf-file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
	
    io = nitf_IOHandle_create(argv[1], NITF_ACCESS_READONLY,
                              NITF_OPEN_EXISTING, &error);
    if (NITF_INVALID_HANDLE(io))
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit(EXIT_FAILURE);
    }

    reader = nitf_Reader_construct(&error);
    if (!reader)
    {
        nitf_Error_print(&error, stdout, "Exiting (1) ...");
        exit(EXIT_FAILURE);
    }
   
#if NITF_VERBOSE_READER
    printf("Here are the loaded handlers\n");
    printf("* * * * * * * * * * * * * * * *\n");
    nitf_HashTable_print(reader->reg->treHandlers);
    printf("* * * * * * * * * * * * * * * *\n");
#endif
    record = nitf_Reader_read(reader, io, &error);

    if (record)
    {
        char nitf_version[NITF_FVER_SZ + 2];
        if (!nitf_Field_get(record->header->fileVersion, nitf_version,
                            NITF_CONV_STRING, NITF_FVER_SZ + 1, &error))
            goto CATCH_ERROR;
        nitf_version[NITF_FVER_SZ] = 0;
        if ((strcmp(nitf_version, "02.00") != 0) &&
                (strcmp(nitf_version, "02.10") != 0)
                && (strncmp(record->header->NITF_FHDR->raw, "NSIF", 4) != 0))
        {
            printf("!!! unhandled NITF version: %3.2f\n", nitf_version);
        }
    }
    else
        goto CATCH_ERROR;

    /* Now show the header */
    showFileHeader(record->header);


    if (!nitf_Field_get(record->header->numImages,
                        &num, NITF_CONV_INT, NITF_INT32_SZ, &error))
        goto CATCH_ERROR;

    /* And now show the image information */
    if (num > 0)
    {

        /*  Walk each image and show  */
        nitf_ListIterator iter = nitf_List_begin(record->images);
        nitf_ListIterator end = nitf_List_end(record->images);

        while (nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_ImageSegment *segment =
                (nitf_ImageSegment *) nitf_ListIterator_get(&iter);
            showImageSubheader(segment->subheader);
            nitf_ListIterator_increment(&iter);
        }
    }
    else
    {
        printf("No image in file!\n");
    }

    if (record->header->numGraphics)
    {
        /*  Walk each graphic and show  */
        nitf_ListIterator iter = nitf_List_begin(record->graphics);
        nitf_ListIterator end = nitf_List_end(record->graphics);

        while (nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_GraphicSegment *segment =
                (nitf_GraphicSegment *) nitf_ListIterator_get(&iter);

            showGraphicSubheader(segment->subheader);
            nitf_ListIterator_increment(&iter);
        }
    }

    if (record->header->numLabels)
    {
        /*  Walk each label and show  */
        nitf_ListIterator iter = nitf_List_begin(record->labels);
        nitf_ListIterator end = nitf_List_end(record->labels);

        while (nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_LabelSegment *segment =
                (nitf_LabelSegment *) nitf_ListIterator_get(&iter);

            showLabelSubheader(segment->subheader);
            nitf_ListIterator_increment(&iter);
        }
    }

    if (record->header->numTexts)
    {
        /*  Walk each text and show  */
        nitf_ListIterator iter = nitf_List_begin(record->texts);
        nitf_ListIterator end = nitf_List_end(record->texts);

        while (nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_TextSegment *segment =
                (nitf_TextSegment *) nitf_ListIterator_get(&iter);

            showTextSubheader(segment->subheader);
            nitf_ListIterator_increment(&iter);
        }
    }

    if (record->header->numDataExtensions)
    {
        /*  Walk each dataExtension and show  */
        nitf_ListIterator iter = nitf_List_begin(record->dataExtensions);
        nitf_ListIterator end = nitf_List_end(record->dataExtensions);

        while (nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_DESegment *segment =
                (nitf_DESegment *) nitf_ListIterator_get(&iter);

            showDESubheader(segment->subheader);
            nitf_ListIterator_increment(&iter);
        }
    }

    if (record->header->numReservedExtensions)
    {
        /*  Walk each reservedextension and show  */
        nitf_ListIterator iter =
            nitf_List_begin(record->reservedExtensions);
        nitf_ListIterator end = nitf_List_end(record->reservedExtensions);

        while (nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_RESegment *segment =
                (nitf_RESegment *) nitf_ListIterator_get(&iter);

            showRESubheader(segment->subheader);
            nitf_ListIterator_increment(&iter);
        }
    }

    nitf_IOHandle_close(io);
    nitf_Record_destruct(&record);

    nitf_Reader_destruct(&reader);

    return 0;

CATCH_ERROR:
    printf("!!! we had a problem reading the file !!!\n");
    nitf_Error_print(&error, stdout, "Exiting...");
    exit(EXIT_FAILURE);
}

