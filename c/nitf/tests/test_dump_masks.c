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
    Test program to print information relating to block masks

    This program only looks at the first image segment

    Call:

   test_dump_masks inputFile

   results go to stdout
*/

#include <import/nitf.h>

NITF_BOOL nitf_ImageIO_getMaskInfo(nitf_ImageIO *nitf,
                                   nitf_Uint32 *imageDataOffset, nitf_Uint32 *blockRecordLength,
                                   nitf_Uint32 *padRecordLength, nitf_Uint32 *padPixelValueLength,
                                   nitf_Uint8 **padValue, nitf_Uint64 **blockMask, nitf_Uint64 **padMask);

int main(int argc, char *argv[])
{
    nitf_Reader *reader;         /* Reader object */
    nitf_Record *record;         /* Record used for input and output */
    nitf_IOHandle in;            /* Input I/O handle */
    nitf_ListIterator imgIter;   /* Image segment list iterator */
    nitf_ImageSegment *seg;      /* Image segment object */
    nitf_ImageSubheader *subhdr; /* Image subheader object */
    nitf_ImageReader *iReader;   /* Image reader */
    nitf_BlockingInfo *blkInfo;  /* Blocking information */

    static nitf_Error errorObj;  /* Error object for messages */
    nitf_Error *error;           /* Pointer to the error object */

    /*  Image information */

    char imageMode[NITF_IMODE_SZ+1]; /* Image (blocking) mode */

    /*  Mask structure and mask components */

    nitf_Uint32 imageDataOffset;    /* Offset to actual image data past masks */
    nitf_Uint32 blockRecordLength;  /* Block mask record length */
    nitf_Uint32 padRecordLength;    /* Pad mask record length */
    nitf_Uint32 padPixelValueLength; /* Pad pixel value length in bytes */
    nitf_Uint8 *padValue;           /* Pad value */
    nitf_Uint64 *blockMask;         /* Block mask array */
    nitf_Uint64 *padMask;           /* Pad mask array */

    error = &errorObj;
    if (argc != 2)
    {
        fprintf(stderr, "Usage %s inputFile\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /*    Get the input record */

    in = nitf_IOHandle_create(argv[1],
                              NITF_ACCESS_READONLY, NITF_OPEN_EXISTING, error);
    if (NITF_INVALID_HANDLE(in))
    {
        nitf_Error_print(error, stderr, "Error opening input ");
        exit(EXIT_FAILURE);
    }

    reader = nitf_Reader_construct(error);
    if (reader == NULL)
    {
        nitf_IOHandle_close(in);
        nitf_Error_print(error, stderr, "Error creating reader ");
        exit(EXIT_FAILURE);
    }

    record = nitf_Reader_read(reader, in, error);
    if (record == NULL)
    {
        nitf_Reader_destruct(&reader);
        nitf_IOHandle_close(in);
        nitf_Error_print(error, stderr, "Error reading input ");
        exit(EXIT_FAILURE);
    }

    /*  Get information from the image subheader */

    imgIter = nitf_List_at(record->images, 0);
    seg = (nitf_ImageSegment *) nitf_ListIterator_get(&imgIter);
    if (seg == NULL)
    {
        fprintf(stderr, "No Image segment\n");
        nitf_Reader_destruct(&reader);
        nitf_IOHandle_close(in);
        exit(EXIT_FAILURE);
    }
    subhdr = seg->subheader;

    nitf_Field_get(subhdr->imageMode,
                   imageMode, NITF_CONV_STRING, NITF_IMODE_SZ + 1, error);
    imageMode[NITF_IMODE_SZ] = 0;

    /*  Get an image reader which creates the nitf_ImageIO were the masks are */

    iReader = nitf_Reader_newImageReader(reader, 0, error);
    if (iReader == NULL)
    {
        nitf_Reader_destruct(&reader);
        nitf_IOHandle_close(in);
        nitf_Record_destruct(&record);
        nitf_Error_print(error, stderr, "Error reading input ");
        exit(EXIT_FAILURE);
    }

    blkInfo = nitf_ImageReader_getBlockingInfo(iReader, error);
    if (blkInfo == NULL)
    {
        nitf_ImageReader_destruct(&iReader);
        nitf_Reader_destruct(&reader);
        nitf_IOHandle_close(in);
        nitf_Record_destruct(&record);
        nitf_Error_print(error, stderr, "Error reading input ");
        exit(EXIT_FAILURE);
    }

    /* Print the blocking information */

    printf("Image %s:\n", argv[1]);
    printf("  Blocking (mode is %s):\n", imageMode);
    printf("    Block array dimensions (r,c) = %d %d\n",
           blkInfo->numBlocksPerRow, blkInfo->numBlocksPerCol);
    printf("    Block dimensions (r,c) = %d,%d\n",
           blkInfo->numRowsPerBlock, blkInfo->numColsPerBlock);
    printf("    Block length in bytes %zu\n", blkInfo->length);

    /*  Get the actual information */

    if (nitf_ImageIO_getMaskInfo(iReader->imageDeblocker,
                                 &imageDataOffset, &blockRecordLength,
                                 &padRecordLength, &padPixelValueLength,
                                 &padValue, &blockMask, &padMask))
    {
        nitf_Uint64 *blkPtr;  /* Pointer into block mask */
        nitf_Uint64 *padPtr;  /* Pointer into pad mask */
        nitf_Uint32 i;
        nitf_Uint32 j;

        printf("  Masked image:\n");
        printf("    Image data offset = %d\n", imageDataOffset);
        printf("    Block and pad mask record lengths = %d %d\n",
               blockRecordLength, padRecordLength);
        printf("    Pad value length = %d\n", padPixelValueLength);
        printf("    Pad value = ");
        for (i = 0;i < padPixelValueLength;i++)
            printf("%x ", padValue[i]);
        printf("\n");

        if (blockRecordLength != 0)
        {
            blkPtr = blockMask;

            printf("  Block mask:\n");
            for (i = 0;i < blkInfo->numBlocksPerRow;i++)
            {
                printf("        ");
                for (j = 0;j < blkInfo->numBlocksPerCol;j++)
                    if (*(blkPtr++) == 0xffffffff)
                        printf("*");
                    else
                        printf("+");
                printf("\n");
            }
        }
        if (padRecordLength != 0)
        {
            padPtr = padMask;

            printf("  Pad mask:\n");
            for (i = 0;i < blkInfo->numBlocksPerRow;i++)
            {
                printf("        ");
                for (j = 0;j < blkInfo->numBlocksPerCol;j++)
                    if (*(padPtr++) == 0xffffffff)
                        printf("+");
                    else
                        printf("*");
                printf("\n");
            }
        }
    }
    else
        printf("Not a masked image\n");

    /*    Clean-up */

    nitf_ImageReader_destruct(&iReader);
    nitf_Reader_destruct(&reader);
    nitf_IOHandle_close(in);
    nitf_Record_destruct(&record);
    return(0);
}
