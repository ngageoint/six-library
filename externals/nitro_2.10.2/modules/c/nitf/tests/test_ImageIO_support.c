/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
*     This file provides support functions for test_ImageIO_read_data and
*  test_ImageIO_writePattern
*/

#include "test_ImageIO.h"

/*      Buffer for error messages */

static char buffer[TEST_NITF_IMAGE_IO_MAX_STRING+2];

/*====================== test_nitf_ImageIOReadConstructArgs ==================*/

NITFAPI(test_nitf_ImageIOConstructArgs *) test_nitf_ImageIOReadConstructArgs(
    FILE *file, char **error)
{
    test_nitf_ImageIOConstructArgs *args;   /* The result */
    long padValues[8];                      /* Temp for pad value bytes */
    long tmpLong;                           /* Temp for longs */
    uint32_t i;

    args = (test_nitf_ImageIOConstructArgs *)
           malloc(sizeof(test_nitf_ImageIOConstructArgs));
    if (args == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error allocating arguments structure");
        *error = buffer;
        return(NULL);
    }

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file, nRows argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%ld", &tmpLong) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, nRows argument");
        *error = buffer;
        return(NULL);
    }
    args->nRows = tmpLong;

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file, nColumns argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%ld", &tmpLong) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, nColumns argument");
        *error = buffer;
        return(NULL);
    }
    args->nColumns = tmpLong;

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file, nBands argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%ld", &tmpLong) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, nBands argument");
        *error = buffer;
        return(NULL);
    }
    args->nBands = tmpLong;

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file, nMultiBands argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%ld", &tmpLong) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, nMultiBands argument");
        *error = buffer;
        return(NULL);
    }
    args->nMultiBands = tmpLong;

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file, pixelType argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%s", args->pixelType) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, pixelType argument");
        *error = buffer;
        return(NULL);
    }

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file, nBits argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%ld", &tmpLong) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, nBits argument");
        *error = buffer;
        return(NULL);
    }
    args->nBits = tmpLong;
    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file, nBitsActual argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%ld", &tmpLong) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, nBitsActual argument");
        *error = buffer;
        return(NULL);
    }
    args->nBitsActual = tmpLong;

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file, justify argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%s", args->justify) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, justify argument");
        *error = buffer;
        return(NULL);
    }

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file, nBlksPerRow argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%ld", &tmpLong) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, nBlksPerRow argument");
        *error = buffer;
        return(NULL);
    }
    args->nBlksPerRow = tmpLong;

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file, nBlksPerColumn argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%ld", &tmpLong) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, nBlksPerColumn argument");
        *error = buffer;
        return(NULL);
    }
    args->nBlksPerColumn = tmpLong;

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file, nRowsPerBlk argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%ld", &tmpLong) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, nRowsPerBlk argument");
        *error = buffer;
        return(NULL);
    }
    args->nRowsPerBlk = tmpLong;

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file, nColumnsPerBlk argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%ld", &tmpLong) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, nColumnsPerBlk argument");
        *error = buffer;
        return(NULL);
    }
    args->nColumnsPerBlk = tmpLong;

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file, mode argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%s", args->mode) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, mode argument");
        *error = buffer;
        return(NULL);
    }

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file, compression argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%s", args->compression) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, compression argument");
        *error = buffer;
        return(NULL);
    }

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file, pad value argument");
        *error = buffer;
        return(NULL);
    }

    if (sscanf(buffer, "%lx %lx %lx %lx %lx %lx %lx %lx",
               padValues, padValues + 1, padValues + 2, padValues + 3,
               padValues + 4, padValues + 5, padValues + 6, padValues + 7) != 8)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, pad value argument");
        *error = buffer;
        return(NULL);
    }

    for (i = 0;i < 8;i++)
        args->padValue[i] = padValues[i];

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file, offset argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%ld", &tmpLong) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, offset argument");
        *error = buffer;
        return(NULL);
    }
    args->offset = tmpLong;

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file pattern gen argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%s", args->dataPattern) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, pattern gen argument");
        *error = buffer;
        return(NULL);
    }

    return(args);
}

/*========================= test_nitf_ImageIOReadReadArgs =====================*/

NITFAPI(test_nitf_ImageIOReadArgs *)test_nitf_ImageIOReadReadArgs(
    FILE *file, char **error)
{
    test_nitf_ImageIOReadArgs *args;   /* The result */
    uint32_t band;                  /* Current band */
    long tmpLong;                           /* Temp for longs */

    args = (test_nitf_ImageIOReadArgs *)
           malloc(sizeof(test_nitf_ImageIOReadArgs));
    if (args == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error allocating arguments structure");
        *error = buffer;
        return(NULL);
    }

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading read argument file, input name argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%s", args->name) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding read argument file, input name argument");
        *error = buffer;
        return(NULL);
    }

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading read argument file, row argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%ld", &tmpLong) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding read argument file, row argument");
        *error = buffer;
        return(NULL);
    }
    args->row = tmpLong;

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading read argument file, nRows argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%ld", &tmpLong) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding read argument file, nRows argument");
        *error = buffer;
        return(NULL);
    }
    args->nRows = tmpLong;
    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading read argument file, rowSkip argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%ld", &tmpLong) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding read argument file, rowSkip argument");
        *error = buffer;
        return(NULL);
    }
    args->rowSkip = tmpLong;

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading read argument file, column argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%ld", &tmpLong) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding read argument file, column argument");
        *error = buffer;
        return(NULL);
    }
    args->column = tmpLong;

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file, nColumns argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%ld", &tmpLong) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, nColumns argument");
        *error = buffer;
        return(NULL);
    }
    args->nColumns = tmpLong;

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file, column skip argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%ld", &tmpLong) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, columnSkip argument");
        *error = buffer;
        return(NULL);
    }
    args->columnSkip = tmpLong;

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading read argument file, down-sample method argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%s", args->downSample) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding read argument file, down-sample method argument");
        *error = buffer;
        return(NULL);
    }

    if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error reading new argument file, nBands argument");
        *error = buffer;
        return(NULL);
    }
    if (sscanf(buffer, "%ld", &tmpLong) != 1)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error decoding new argument file, nBands argument");
        *error = buffer;
        return(NULL);
    }
    args->nBands = tmpLong;

    for (band = 0;band < args->nBands;band++)
    {
        if (fgets(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, file) == NULL)
        {
            snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                     "Error reading new argument file, bands argument");
            *error = buffer;
            return(NULL);
        }
        if (sscanf(buffer, "%ld", &tmpLong) != 1)
        {
            snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                     "Error decoding new argument file, bands argument");
            *error = buffer;
            return(NULL);
        }
        args->bands[band] = tmpLong;
    }

    return(args);
}


/*========================= test_nitf_ImageIO_mkArray ========================*/

NITFAPI(void *) test_nitf_ImageIO_mkArray(
    test_nitf_ImageIOConstructArgs *args, char **error)
{
    uint8_t ***data;     /* The result */
    uint32_t nBands;     /* Actual number of bands */
    uint32_t band;       /* The current band */
    uint8_t **rows;      /* The row dimension */
    uint32_t row;        /* The current row */
    uint32_t bytes;      /* Number of bytes per pixel */
    uint8_t *cols;       /* The column dimension */

    nBands = args->nBands;
    if (nBands == 0)
        nBands = args->nMultiBands;
    if (nBands == 0)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, "Invalid band count\n");
        *error = buffer;
        return(NULL);
    }

    /*    Band dimension */

    data = (uint8_t ***) NITF_MALLOC(nBands * sizeof(uint8_t **));
    if (data == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error allocating data buffer\n");
        *error = buffer;
        return(NULL);
    }

    /*    Row dimension */

    rows = (uint8_t **) NITF_MALLOC(nBands * (args->nRows) * sizeof(uint8_t *));
    if (rows == NULL)
    {
        NITF_FREE(data);
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error allocating data buffer\n");
        *error = buffer;
        return(NULL);
    }

    for (band = 0;band < nBands;band++)
    {
        data[band] = rows + band * (args->nRows);
    }

    /*    Column dimension */

    bytes = NITF_NBPP_TO_BYTES(args->nBits);
    cols = (uint8_t *) NITF_MALLOC(
               nBands * (args->nRows) * (args->nColumns) * bytes);
    if (data == NULL)
    {
        NITF_FREE(data);
        NITF_FREE(rows);
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error allocating data buffer\n");
        *error = buffer;
        return(NULL);
    }

    for (band = 0;band < nBands;band++)
        for (row = 0;row < args->nRows;row++)
        {
            data[band][row] = cols;
            cols += args->nColumns * bytes;
        }

    return((void *) data);
}

/*========================= test_nitf_ImageIO_freeArray ======================*/

NITFAPI(void) test_nitf_ImageIO_freeArray(uint8_t ***data)
{
    NITF_FREE(data[0][0]);   /* Columns */
    NITF_FREE(data[0]);      /* Rows */
    NITF_FREE(data);         /* Bands */
    return;
}

/*========================= test_nitf_ImageIO_brcI4 ==========================*/

NITFAPI(void *) test_nitf_ImageIO_brcI4(
    test_nitf_ImageIOConstructArgs *args, char **error)
{
    uint32_t ***data;    /* The result */
    uint32_t nBands;     /* Number of bands */
    uint32_t band;       /* The current band */
    uint32_t row;        /* The current row */
    uint32_t col;        /* The current column */

    data = (uint32_t ***) test_nitf_ImageIO_mkArray(args, error);
    if (data == NULL)
        return(NULL);

    nBands = args->nBands;
    if (nBands == 0)
        nBands = args->nMultiBands;
    if (nBands == 0)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, "Invalid band count\n");
        *error = buffer;
        return(NULL);
    }

    for (band = 0;band < nBands;band++)
        for (row = 0;row < args->nRows;row++)
            for (col = 0;col < args->nColumns;col++)
                data[band][row][col] = (band << 16) + (row << 8) + col;

    return((uint8_t ***) data);
}

/*========================= test_nitf_ImageIO_brcI4 ==========================*/
/*
*   Single precision complex. The real part is band*100+row and the imaginary
* is band*100 + column. The data is allocated and indexed as doublw but loaded
* as two consecutive floats
*/

NITFAPI(void *) test_nitf_ImageIO_brcC8(
    test_nitf_ImageIOConstructArgs *args, char **error)
{
    double ***data;       /* The result */
    float *fdp;          /* Float pointer to current pixel */
    float nBands;        /* Number of bands */
    uint32_t band;    /* The current band */
    uint32_t row;     /* The current row */
    uint32_t col;     /* The current column */

    data = (double ***) test_nitf_ImageIO_mkArray(args, error);
    if (data == NULL)
        return(NULL);

    nBands = args->nBands;
    if (nBands == 0)
        nBands = args->nMultiBands;
    if (nBands == 0)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, "Invalid band count\n");
        *error = buffer;
        return(NULL);
    }

    for (band = 0;band < nBands;band++)
        for (row = 0;row < args->nRows;row++)
            for (col = 0;col < args->nColumns;col++)
            {
                fdp = (float *) & (data[band][row][col]);
                *(fdp++) = band * 100.0 + row;
                *fdp = band * 100.0 + col;
            }

    return((uint8_t ***) data);
}

/*========================= test_nitf_ImageIO_bigEndian ======================*/

NITFAPI(int) test_nitf_ImageIO_bigEndian(void)
{
    uint8_t p8[2] = {1, 2};    /* For big-endian test */
    uint16_t *p16;            /* For big-endian test */

    p16 = (uint16_t *) p8;
    return((*p16 == 0x102) ? 1 : 0); /* 0x102 => big-endian */
}

/*========================= test_nitf_ImageIO_fakeSubheader ==================*/

NITFAPI(nitf_ImageSubheader *) test_nitf_ImageIO_fakeSubheader(
    test_nitf_ImageIOConstructArgs *args, char **error)
{
    nitf_ImageSubheader *subheader;         /* For constructor */
    nitf_Error errorObj;                    /* Error object instance */

    subheader = nitf_ImageSubheader_construct(&errorObj);
    if (subheader == NULL)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING,
                 "Error allocating image subheader\n");
        *error = buffer;
        return(NULL);
    }

    nitf_Field_setUint32(subheader->numRows, args->nRows, &errorObj);
    nitf_Field_setUint32(subheader->numCols, args->nColumns, &errorObj);
    nitf_Field_setUint32(subheader->numImageBands, args->nBands, &errorObj);
    nitf_Field_setUint32(subheader->numMultispectralImageBands,
                         args->nMultiBands, &errorObj);
    nitf_Field_setString(subheader->pixelValueType, args->pixelType, &errorObj);
    nitf_Field_setUint32(subheader->numBitsPerPixel, args->nBits, &errorObj);
    nitf_Field_setUint32(subheader->actualBitsPerPixel,
                         args->nBitsActual, &errorObj);
    nitf_Field_setString(subheader->pixelJustification, args->justify, &errorObj);
    nitf_Field_setUint32(subheader->numBlocksPerRow,
                         args->nBlksPerRow, &errorObj);
    nitf_Field_setUint32(subheader->numBlocksPerCol,
                         args->nBlksPerColumn, &errorObj);
    nitf_Field_setUint32(subheader->numPixelsPerVertBlock,
                         args->nRowsPerBlk, &errorObj);
    nitf_Field_setUint32(subheader->numPixelsPerHorizBlock,
                         args->nColumnsPerBlk, &errorObj);
    nitf_Field_setString(subheader->imageMode, args->mode, &errorObj);
    nitf_Field_setString(subheader->imageCompression,
                         args->compression, &errorObj);

    return(subheader);
}

/*========================= makeBlockPattern =================================*/
/*
  makeBlockPattern - Local function to make a pattern array from a blocking
  specification string

  The pattern array is an array of strings describing the missing block
  pattern to be generated by the "blocks" pattern option. The pixel value
  is always 1 and the pad pixel value 0

  The array is an array of strings, one of each row of blocks. Each string
  contains on character for each block in the row. If the character is "M"
  the block is missing, if it is "P" the block is present, if it is "T" the
  block is present but has pad (T as in transition),

  for transition blocks the upper flef corner is set to be a pad pixel
  regardless of the pattern.

  There is not a lot of checking for correctness in the spec

  The result can be NITF_FREE'd

*/

char **makeBlockPattern(test_nitf_ImageIOConstructArgs *args,
                        char *spec, char **error)
{
    char **array;             /* The result */
    uint32_t nBlkRows;     /* Number of rows of blocks */
    uint32_t nBlkCols;     /* Number of columns of blocks */
    size_t specLen;           /* Length of the spec string */
    uint32_t i;

    specLen = strlen(spec);
    nBlkRows = args->nBlksPerColumn;
    nBlkCols = args->nBlksPerRow;

    const size_t array_sz = sizeof(char*) * nBlkRows + specLen + 1;
    array = NITF_MALLOC(array_sz);
    if (array == NULL)
    {
        *error = "Error allocating pattern array\n";
        return(NULL);
    }
    nrt_strcpy_s((char *) (&(array[nBlkRows])), array_sz, spec);
    for (i = 0;i < nBlkRows;i++)
    {
        array[i] = ((char *) & (array[nBlkRows])) + (nBlkCols + 1) * i;
        array[i][nBlkCols] = 0;
    }

    return(array);
}

/*========================= test_nitf_ImageIO_block ==========================*/
/*
*  Block pattern generator. The data is generated based on the blocking
*  structure and a mask. The mask specifies which blocks are present and
*  which are missing. For missing blocks the data is 0 for present blocks
*  the data is the band number
*
*  The mask is determined by the data generator tag. The tag always starts
*  with "block_". The rest of the tag is the specification
*
*
*  The specification is a single string composed of a value type string and the
*  row strings of the desired result. The value type and each row sub-string is *  seperated by an _. Each row string contains a character for each block in
*  the row. If the character is "M" the block is missing, if it is "P" the
*  block is present
*
*  The value type string is three characters long and specifies how the value
*  is generated. The strings are:
*
*      BND    - Value is band number + 1
*      BLK    - Value is block number + band number + 1
*/

/*  Value type codes */

#define VALUE_BND 1
#define VALUE_BLK 2

/*  Macro to do pocessing for types (wants to be a template but this is C++ */

#define PATTERN_PROCESS(type) \
    { \
        type ***dataType = (type ***) data; \
        for(band=0;band<nBands;band++) \
        { \
            blkRow = 0; \
            rowsLeft = rowsPerBlk; \
            for(row=0;row<args->nRows;row++) \
            { \
                if(rowsLeft == 0) \
                { \
                    blkRow += 1; \
                    rowsLeft = rowsPerBlk; \
                } \
                blkCol = 0; \
                colsLeft = colsPerBlk; \
                transient = (pattern[blkRow][0] == 'T'); \
                present = transient || (pattern[blkRow][0] == 'P'); \
                for(col=0;col<args->nColumns;col++) \
                { \
                    if(colsLeft == 0) \
                    { \
                        blkCol += 1; \
                        colsLeft = colsPerBlk; \
                        transient = (pattern[blkRow][blkCol] == 'T'); \
                        present = transient || (pattern[blkRow][blkCol] == 'P'); \
                    } \
                    if(present) \
                    { \
                        if(transient && \
                                ((rowsLeft == rowsPerBlk) && (colsLeft == colsPerBlk))) \
                        { \
                            dataType[band][row][col] = *((type *) (args->padValue)); \
                        } \
                        else \
                        { \
                            switch(valueCode) \
                            { \
                                default: \
                                case VALUE_BND: \
                                    dataType[band][row][col] = band + 1; \
                                    break; \
                                case VALUE_BLK: \
                                    dataType[band][row][col] = \
                                                               blkCol + blkRow*blksPerRow + band + 1; \
                                    break; \
                            } \
                        } \
                    } \
                    else \
                        dataType[band][row][col] = *((type *) (args->padValue)); \
                    colsLeft -= 1; \
                } \
                rowsLeft -= 1; \
            } \
        } \
    }

NITFAPI(void *) test_nitf_ImageIO_block(
    test_nitf_ImageIOConstructArgs *args, char **error)
{
    uint8_t ***data;     /* The result */
    char **pattern;         /* Block mask pattern */
    uint32_t nBands;     /* Number of bands */
    uint32_t band;       /* The current band */
    uint32_t bytes;      /* Number of bytes per pixel */
    uint32_t blksPerRow; /* Number of blocks/row */
    uint32_t rowsPerBlk; /* Number of rows/block */
    uint32_t colsPerBlk; /* Number of columns/block */
    uint32_t valueCode;  /* Output pixel value type code */
    char valueTypeStr[4];   /* Value type string from specification */
    uint32_t rowsLeft;   /* Number of rows left in this block */
    uint32_t colsLeft;   /* Number of columnsleft in this block */
    uint32_t blkRow;     /* The current block row */
    uint32_t blkCol;     /* The current block column */
    int present;            /* Current block present if true */
    int transient;          /* Current block transient if true */
    uint32_t row;        /* The current row */
    uint32_t col;        /* The current column */

    data = (uint8_t ***) test_nitf_ImageIO_mkArray(args, error);
    if (data == NULL)
        return(NULL);

    pattern = makeBlockPattern(args, &(args->dataPattern[11]), error);
    memmove(valueTypeStr, &(args->dataPattern[7]), 3);
    valueTypeStr[3] = 0;
    if (pattern == NULL)
        return(NULL);

    nBands = args->nBands;
    if (nBands == 0)
        nBands = args->nMultiBands;
    if (nBands == 0)
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, "Invalid band count\n");
        *error = buffer;
        return(NULL);
    }

    blksPerRow = args->nBlksPerRow;
    rowsPerBlk = args->nRowsPerBlk;
    colsPerBlk = args->nColumnsPerBlk;

    bytes = NITF_NBPP_TO_BYTES(args->nBits);
    if (strcmp(valueTypeStr, "BND") == 0)
        valueCode = VALUE_BND;
    else if (strcmp(valueTypeStr, "BLK") == 0)
        valueCode = VALUE_BLK;
    else
    {
        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, "Invalid value type\n");
        *error = buffer;
        return(NULL);
    }

    if (bytes == 1)
        PATTERN_PROCESS(uint8_t)
        else if (bytes == 2)
            PATTERN_PROCESS(uint16_t)
            else if (bytes == 4)
                PATTERN_PROCESS(uint32_t)
                else if (bytes == 8)
                    PATTERN_PROCESS(uint64_t)
                    else
                    {
                        snprintf(buffer, TEST_NITF_IMAGE_IO_MAX_STRING, "Invalid byte count\n");
                        *error = buffer;
                        return(NULL);
                    }
    return(data);
}
