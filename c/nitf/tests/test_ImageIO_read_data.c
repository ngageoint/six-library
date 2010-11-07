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
*      Test program for nitf_ImageIO_read
*
*  This program reads an image based on two "vectors" (text files). The first
*  defines the layout of the image data section (the binary part of the image
*  segment including the masks). The second defines the read operation
*  including the filename and sub-window specification
*
*  The read data is written as flat binary files, one per requested band
*
*      The first argument is the image vector and the second is the read
* vector. The remaining arguments are the outputs, one per requested band
*
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "test_ImageIO.h"

NITFAPI(void) nitf_ImageIO_print(
    nitf_ImageIO *nitf,/* nitf_ImageIO object */
    FILE *file         /* FILE to use for print */
);

int main(int argc, char *argv[])
{
    nitf_ImageIO *nitf;                     /* The parent nitf_ImageIO object */
    char errorBuf[NITF_MAX_EMESSAGE];       /* Error buffer */
    nitf_Uint8 **user;                      /* User buffer list */
    int padded;                             /* Pad pixel present flag */
    nitf_IOHandle fileHandle;               /* I/O handle for reads */
    int ofd;                                /* File descriptor for writes */
    FILE *vector;                           /* FILE for reading vectors */
    test_nitf_ImageIOConstructArgs *newArgs;/* Arguments for new */
    test_nitf_ImageIOReadArgs *readArgs;    /* Arguments for read */
    nitf_ImageSubheader *subheader;         /* For constructor */
    size_t subWindowSize;                   /* Sub-window pixel count */
    nitf_SubWindow *subWindow;              /* Argument for read */
    nitf_DownSampler* downSampler;          /* Down-sample object */
    char *error;                            /* Error message return */
    nitf_Error errorObjActual;              /* Error object instance */
    nitf_Error *errorObj;                   /* Error object (pointer) */
    int i;

    errorObj = &errorObjActual;

    /*    Usage */

    if (argc < 2)
    {
        fprintf(stderr,
                "Usage: %s imageVector readVector band0Out band1Out ... >result\n", argv[0]);
        exit(0);
    }

    /*      Read image and read parameters from first and second arguments */

    vector = fopen(argv[1], "r");
    if (vector == NULL)
    {
        fprintf(stderr, "Error opening image vector file %s\n", argv[0]);
        exit(-1);
    }

    newArgs = test_nitf_ImageIOReadConstructArgs(vector, &error);
    if (newArgs == NULL)
    {
        fprintf(stderr, "%s\n", error);
        exit(-1);
    }
    fclose(vector);

    vector = fopen(argv[2], "r");
    if (vector == NULL)
    {
        fprintf(stderr, "Error opening read vector file %s\n", argv[0]);
        exit(-1);
    }

    readArgs = test_nitf_ImageIOReadReadArgs(vector, &error);
    if (readArgs == NULL)
    {
        fprintf(stderr, "%s\n", error);
        exit(-1);
    }
    fclose(vector);

    /*      Allocate user buffers */

    subWindowSize = (readArgs->nRows) * (readArgs->nColumns) * (newArgs->nBits / 8);

    user = (nitf_Uint8 **) malloc(sizeof(nitf_Uint8 *) * (readArgs->nBands));
    if (user == NULL)
    {
        fprintf(stderr, "Error allocating user buffer\n");
        exit(-1);
    }

    for (i = 0;i < readArgs->nBands;i++)
    {
        user[i] = (nitf_Uint8 *) malloc(subWindowSize);
        if (user[i] == NULL)
        {
            fprintf(stderr, "Error allocating user buffer\n");
            exit(-1);
        }
    }

    /*    Create a fake image subheader to give to ImageIO constructor */

    subheader = test_nitf_ImageIO_fakeSubheader(newArgs, &error);
    if (subheader == NULL)
    {
        fprintf(stderr, "%s", error);
        exit(-1);
    }

    /*     Create the ImageIO */

    nitf = nitf_ImageIO_construct(subheader, newArgs->offset,
                                  0,/*length, must be correct for decompression */
                                  NULL, NULL, errorObj);
    if (nitf == NULL)
    {
        fprintf(stderr, "NtfBlk new failed: %s\n", errorBuf);
        fprintf(stderr, "Err: %s\n", errorObj->message);
        exit(0);
    }

    nitf_ImageIO_setPadPixel(nitf, newArgs->padValue,
                             NITF_NBPP_TO_BYTES(newArgs->nBits));

    nitf_ImageIO_print(nitf, stdout);

    /*     Open input file */

    fileHandle = nitf_IOHandle_create(readArgs->name,
                                      NITF_ACCESS_READONLY, 0, errorObj);
    if (NITF_INVALID_HANDLE(fileHandle))
    {
        nitf_Error_init(errorObj, "File open failed",
                        NITF_CTXT, NITF_ERR_OPENING_FILE);
        nitf_Error_print(errorObj, stderr, "File open failed");
        exit(0);
    }

    /*    Setup for read (create and initialize sub-window object */

    if ((subWindow = nitf_SubWindow_construct(errorObj)) == NULL)
    {
        nitf_Error_init(errorObj, "Sub-window object construct failed",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        nitf_Error_print(errorObj, stderr, "Sub-window object construct failed");
        nitf_IOHandle_close(fileHandle);
        nitf_ImageIO_destruct(&nitf);
        exit(0);
    }

    subWindow->startRow = readArgs->row;
    subWindow->numRows = readArgs->nRows;
    subWindow->startCol = readArgs->column;
    subWindow->numCols = readArgs->nColumns;
    subWindow->bandList = readArgs->bands;
    subWindow->numBands = readArgs->nBands;

    if ((readArgs->rowSkip != 1) || (readArgs->columnSkip != 1))
    {
        if (strcmp(readArgs->downSample, "pixelSkip") == 0)
        {
            if ((downSampler = nitf_PixelSkip_construct(
                                   readArgs->rowSkip, readArgs->columnSkip, errorObj)) == NULL)
            {
                nitf_Error_init(errorObj, "Down-sampler object construct failed",
                                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
                nitf_Error_print(errorObj, stderr,
                                 "Down-sampler object construct failed");
                nitf_IOHandle_close(fileHandle);
                nitf_ImageIO_destruct(&nitf);
                nitf_SubWindow_destruct(&subWindow);
                exit(0);
            }

            if (nitf_SubWindow_setDownSampler(
                        subWindow, downSampler, errorObj) == NITF_FAILURE)
            {
                nitf_Error_init(errorObj, "Read failed",
                                NITF_CTXT, NITF_ERR_READING_FROM_FILE);
                nitf_Error_print(errorObj, stderr, "Read failed");
                nitf_IOHandle_close(fileHandle);
                nitf_ImageIO_destruct(&nitf);
                nitf_SubWindow_destruct(&subWindow);
                nitf_DownSampler_destruct(&downSampler);
                exit(0);
            }
        }
        else if (strcmp(readArgs->downSample, "max") == 0)
        {

            if ((downSampler = nitf_MaxDownSample_construct(
                                   readArgs->rowSkip, readArgs->columnSkip, errorObj)) == NULL)
            {
                nitf_Error_init(errorObj, "Down-sampler object construct failed",
                                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
                nitf_Error_print(errorObj, stderr,
                                 "Down-sampler object construct failed");
                nitf_IOHandle_close(fileHandle);
                nitf_ImageIO_destruct(&nitf);
                nitf_SubWindow_destruct(&subWindow);
                exit(0);
            }

            if (nitf_SubWindow_setDownSampler(
                        subWindow, downSampler, errorObj) == NITF_FAILURE)
            {
                nitf_Error_init(errorObj, "Read failed",
                                NITF_CTXT, NITF_ERR_READING_FROM_FILE);
                nitf_Error_print(errorObj, stderr, "Read failed");
                nitf_IOHandle_close(fileHandle);
                nitf_ImageIO_destruct(&nitf);
                nitf_SubWindow_destruct(&subWindow);
                nitf_DownSampler_destruct(&downSampler);
                exit(0);
            }
        }
        else if (strcmp(readArgs->downSample, "sumSq2") == 0)
        {

            if ((downSampler = nitf_SumSq2DownSample_construct(
                                   readArgs->rowSkip, readArgs->columnSkip, errorObj)) == NULL)
            {
                nitf_Error_init(errorObj, "Down-sampler object construct failed",
                                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
                nitf_Error_print(errorObj, stderr,
                                 "Down-sampler object construct failed");
                nitf_IOHandle_close(fileHandle);
                nitf_ImageIO_destruct(&nitf);
                nitf_SubWindow_destruct(&subWindow);
                exit(0);
            }

            if (nitf_SubWindow_setDownSampler(
                        subWindow, downSampler, errorObj) == NITF_FAILURE)
            {
                nitf_Error_init(errorObj, "Read failed",
                                NITF_CTXT, NITF_ERR_READING_FROM_FILE);
                nitf_Error_print(errorObj, stderr, "Read failed");
                nitf_IOHandle_close(fileHandle);
                nitf_ImageIO_destruct(&nitf);
                nitf_SubWindow_destruct(&subWindow);
                nitf_DownSampler_destruct(&downSampler);
                exit(0);
            }
        }
        else if (strcmp(readArgs->downSample, "select2") == 0)
        {

            if ((downSampler = nitf_Select2DownSample_construct(
                                   readArgs->rowSkip, readArgs->columnSkip, errorObj)) == NULL)
            {
                nitf_Error_init(errorObj, "Down-sampler object construct failed",
                                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
                nitf_Error_print(errorObj, stderr,
                                 "Down-sampler object construct failed");
                nitf_IOHandle_close(fileHandle);
                nitf_ImageIO_destruct(&nitf);
                nitf_SubWindow_destruct(&subWindow);
                exit(0);
            }

            if (nitf_SubWindow_setDownSampler(
                        subWindow, downSampler, errorObj) == NITF_FAILURE)
            {
                nitf_Error_init(errorObj, "Read failed",
                                NITF_CTXT, NITF_ERR_READING_FROM_FILE);
                nitf_Error_print(errorObj, stderr, "Read failed");
                nitf_IOHandle_close(fileHandle);
                nitf_ImageIO_destruct(&nitf);
                nitf_SubWindow_destruct(&subWindow);
                nitf_DownSampler_destruct(&downSampler);
                exit(0);
            }
        }
        else
        {
            nitf_Error_init(errorObj, "Invalid down-sample method",
                            NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
            nitf_Error_fprintf(errorObj, stderr,
                               "Invalid down-sample method: %s\n", readArgs->downSample);
            nitf_IOHandle_close(fileHandle);
            nitf_ImageIO_destruct(&nitf);
            nitf_SubWindow_destruct(&subWindow);
            exit(0);
        }
    }

    /*     Read sub-window */

    if (!nitf_ImageIO_read(nitf, fileHandle, subWindow, user, &padded, errorObj))
    {
        nitf_Error_print(errorObj, stderr, "Read failed");
        nitf_IOHandle_close(fileHandle);
        nitf_ImageIO_destruct(&nitf);
        nitf_SubWindow_destruct(&subWindow);
        exit(0);
    }
    nitf_SubWindow_destruct(&subWindow);

    nitf_IOHandle_close(fileHandle);

    printf("Padded = %d\n", padded);

    /*      Write result */

    for (i = 0;i < readArgs->nBands;i++)
    {
        if ((ofd = open(argv[3+i], O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1)
        {
            fprintf(stderr, "Output file %d open failed\n", i);
            exit(0);
        }

        write(ofd, user[i], subWindowSize);
        close(ofd);
    }

    return 0;
}
