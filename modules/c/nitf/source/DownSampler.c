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

#include "nitf/DownSampler.h"

NITFAPI(void) nitf_DownSampler_destruct(nitf_DownSampler ** downsampler)
{
    if (*downsampler)
    {
        if ((*downsampler)->data != NULL)
            (*downsampler)->iface->destruct((*downsampler)->data);
        NITF_FREE(*downsampler);
        *downsampler = NULL;
    }

}

NITFPRIV(NITF_BOOL) PixelSkip_apply(nitf_DownSampler * object,
                                    NITF_DATA ** inputWindows,
                                    NITF_DATA ** outputWindows,
                                    uint32_t numBands,
                                    uint32_t numWindowRows,
                                    uint32_t numWindowCols,
                                    uint32_t numInputCols,
                                    uint32_t numCols,
                                    uint32_t pixelType,
                                    uint32_t pixelSize,
                                    uint32_t rowsInLastWindow,
                                    uint32_t colsInLastWindow,
                                    nitf_Error * error)
{
    (void)pixelType;
    (void)rowsInLastWindow;
    (void)colsInLastWindow;
    (void)error;

    uint32_t row;            /* Current row */
    uint32_t column;         /* Current column */
    uint32_t colInc;         /* Column increment */
    uint32_t rowInc;         /* Pointer increment for end of row */
    uint32_t outRowInc;      /* Output pointer increment for end of row */
    uint32_t band;           /* Current band */

    /*
     *  Note: The output buffer is at down-sampled resolution and the
     *  part being created may not span the entire user window.
     *  Therefore a separate row increment must be calculated.
     *
     *  Since the data is being copied, not interpreted. any type of the right
     *  size will work.
     */

    colInc = object->colSkip;
    rowInc = numInputCols * (object->colSkip - 1);
    outRowInc = numCols - numWindowCols;

    switch (pixelSize)
    {
        case 1:
        {
            uint8_t *inp;    /* Pointer into input */
            uint8_t *outp;   /* Pointer into output */

            for (band = 0; band < numBands; band++)
            {
                inp = (uint8_t *) inputWindows[band];
                outp = (uint8_t *) outputWindows[band];

                for (row = 0; row < numWindowRows; row++)
                {
                    for (column = 0; column < numWindowCols; column++)
                    {
                        *(outp++) = *inp;
                        inp += colInc;
                    }
                    inp += rowInc;
                    outp += outRowInc;
                }
            }
        }
        break;
        case 2:
        {
            uint16_t *inp;   /* Pointer into input */
            uint16_t *outp;  /* Pointer into output */

            for (band = 0; band < numBands; band++)
            {
                inp = (uint16_t *) inputWindows[band];
                outp = (uint16_t *) outputWindows[band];

                for (row = 0; row < numWindowRows; row++)
                {
                    for (column = 0; column < numWindowCols; column++)
                    {
                        *(outp++) = *inp;
                        inp += colInc;
                    }
                    inp += rowInc;
                    outp += outRowInc;
                }
            }
        }
        break;
        case 4:
        {
            uint32_t *inp;   /* Pointer into input */
            uint32_t *outp;  /* Pointer into output */

            for (band = 0; band < numBands; band++)
            {
                inp = (uint32_t *) inputWindows[band];
                outp = (uint32_t *) outputWindows[band];
                for (row = 0; row < numWindowRows; row++)
                {
                    for (column = 0; column < numWindowCols; column++)
                    {
                        *(outp++) = *inp;
                        inp += colInc;
                    }
                    inp += rowInc;
                    outp += outRowInc;
                }
            }
        }
        break;
        case 8:
        {
            uint64_t *inp;   /* Pointer into input */
            uint64_t *outp;  /* Pointer into output */

            for (band = 0; band < numBands; band++)
            {
                inp = (uint64_t *) inputWindows[band];
                outp = (uint64_t *) outputWindows[band];

                for (row = 0; row < numWindowRows; row++)
                {
                    for (column = 0; column < numWindowCols; column++)
                    {
                        *(outp++) = *inp;
                        inp += colInc;
                    }
                    inp += rowInc;
                    outp += outRowInc;
                }
            }
        }
        break;
        case 16:                   /* It's not clear if this case is actually possible */
        {
            uint64_t *inp;   /* Pointer into input */
            uint64_t *outp;  /* Pointer into output */

            colInc *= 2;
            rowInc *= 2;
            outRowInc *= 2;

            for (band = 0; band < numBands; band++)
            {
                inp = (uint64_t *) inputWindows[band];
                outp = (uint64_t *) outputWindows[band];

                for (row = 0; row < numWindowRows; row++)
                {
                    for (column = 0; column < numWindowCols; column++)
                    {
                        *(outp++) = *inp;
                        *(outp++) = *inp;
                        inp += colInc;
                    }
                    inp += rowInc;
                    outp += outRowInc;
                }
            }
        }
        break;

    }

    return NITF_SUCCESS;
}



NITFPRIV(void) PixelSkip_destruct(NITF_DATA * data)
{
    (void)data;
    return;                     /* There is no implementation data */
}

NITFAPI(nitf_DownSampler *) nitf_PixelSkip_construct(uint32_t rowSkip,
        uint32_t colSkip,
        nitf_Error * error)
{

    static nitf_IDownSampler iPixelSkip =
        {
            &PixelSkip_apply,
            &PixelSkip_destruct
        };

    nitf_DownSampler *downsampler;

    downsampler =
        (nitf_DownSampler *) NITF_MALLOC(sizeof(nitf_DownSampler));
    if (!downsampler)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    downsampler->rowSkip = rowSkip;
    downsampler->colSkip = colSkip;
    downsampler->multiBand = 0;
    downsampler->minBands = 1;
    downsampler->maxBands = 0;
    downsampler->types = NITF_DOWNSAMPLER_TYPE_ALL;
    downsampler->data = NULL;

    downsampler->iface = &iPixelSkip;
    return downsampler;
}

/*      Max down-sample method */

/*  In the following, the row and column indexes are the row in the array of
* windows, the winRow and winColumn are the row and column within the current
* sample window
*
*  The complex case calculates the max of the absolute value
*/

#define MAX_DOWN_SAMPLE(type) \
    { \
        uint32_t colSkip;     /* Column skip */ \
        uint32_t rowSkip;     /* Row skip */ \
        uint32_t colInc;      /* Column increment */ \
        uint32_t rowInc;      /* Pointer increment for end of row */ \
        uint32_t outRowInc;   /* Output pointer increment for end of row */ \
        uint32_t winRowInc;   /* Row increment, current window */ \
        uint32_t row;         /* Current row */ \
        uint32_t column;      /* Current column */ \
        uint32_t winRow;      /* Current row in current window */ \
        uint32_t winCol;      /* Current column current window */ \
        uint32_t rowWinLimit; /* Number of rows in current window */ \
        type *currentRowPtr;     /* Pointer to the current window row */ \
        type *currentPtr;        /* Pointer to the current window UL corner */ \
        type *pixel;             /* Pointer to the current pixel */ \
        type maxValue;           /* Current maximum value */ \
        type *outp;              /* Pointer into output */ \
        \
        colSkip = object->colSkip; \
        rowSkip = object->rowSkip; \
        colInc = colSkip; \
        rowInc = numInputCols*colSkip*rowSkip; \
        winRowInc = numInputCols-colSkip; \
        outRowInc = numCols - numWindowCols; \
        \
        for(band=0;band<numBands;band++) \
        { \
            currentRowPtr = (type *) inputWindows[band]; \
            outp = (type *) outputWindows[band]; \
            for(row=0;row<numWindowRows;row++) \
            { \
                currentPtr = currentRowPtr; \
                for(column=0;column<numWindowCols;column++) \
                { \
                    pixel = currentPtr; \
                    maxValue = *pixel; \
                    if(row < (numWindowRows-1)) \
                        rowWinLimit = rowSkip; \
                    else \
                        rowWinLimit = rowsInLastWindow; \
                    for(winRow=0;winRow<rowWinLimit;winRow++) \
                    { \
                        if(column < (numWindowCols-1)) \
                            for(winCol=0;winCol<colSkip;winCol++) \
                            { \
                                if(maxValue < *pixel) \
                                    maxValue = *pixel; \
                                pixel += 1; \
                            } \
                        else \
                        { \
                            for(winCol=0;winCol<colsInLastWindow;winCol++) \
                            { \
                                if(maxValue < *pixel) \
                                    maxValue = *pixel; \
                                pixel += 1; \
                            } \
                            pixel += colSkip - colsInLastWindow; \
                        } \
                        pixel += winRowInc; \
                    } \
                    currentPtr += colInc; \
                    *(outp++) = maxValue; \
                } \
                currentRowPtr += rowInc; \
                outp += outRowInc; \
            } \
        } \
        \
        return(1); \
    }

/*
*    Complex cases
*
*  The square of the absolute value is used for the compares since sqrt
*  preserves order it is not necessary to do it
*/

#define MAX_DOWN_SAMPLE_CMPX_(type) \
    { \
        uint32_t colSkip;     /* Column skip */ \
        uint32_t rowSkip;     /* Row skip */ \
        uint32_t colInc;      /* Column increment */ \
        uint32_t rowInc;      /* Pointer increment for end of row */ \
        uint32_t outRowInc;   /* Output pointer increment for end of row */ \
        uint32_t winRowInc;   /* Row increment, current window */ \
        uint32_t row;         /* Current row */ \
        uint32_t column;      /* Current column */ \
        uint32_t winRow;      /* Current row in current window */ \
        uint32_t winCol;      /* Current column current window */ \
        uint32_t rowWinLimit; /* Number of rows in current window */ \
        type *currentRowPtr;     /* Pointer to the current window row */ \
        type *currentPtr;        /* Pointer to the current window UL corner */ \
        type *pixelReal;         /* Pointer to the current pixel */ \
        type *pixelImg;          /* Pointer to the current pixel */ \
        type maxValueSq;         /* Current maximum absolute value squared */ \
        type maxValueSqTest;     /* Test maximum absolute value squared */ \
        type maxReal;            /* Current maximum value, real part */ \
        type maxImg;             /* Current maximum value, complex part */ \
        type testReal;           /* Current test value, real part */ \
        type testImg;            /* Current test value, complex part */ \
        type *outp;              /* Pointer into output */ \
        \
        maxReal = 0.0; \
        maxImg = 0.0; \
        colSkip = object->colSkip; \
        rowSkip = object->rowSkip; \
        colInc = colSkip*2; \
        rowInc = (numWindowCols*colSkip*rowSkip)*2; \
        winRowInc = (numCols - colSkip)*2; \
        outRowInc = (numCols - numWindowCols)*2; \
        \
        for(band=0;band<numBands;band++) \
        { \
            currentRowPtr = (type *) inputWindows[band]; \
            outp = (type *) outputWindows[band]; \
            for(row=0;row<numWindowRows;row++) \
            { \
                currentPtr = currentRowPtr; \
                for(column=0;column<numWindowCols;column++) \
                { \
                    pixelReal = currentPtr; \
                    pixelImg = currentPtr + 1; \
                    maxValueSq = 0.0; \
                    if(row < (numWindowRows-1)) \
                        rowWinLimit = rowSkip; \
                    else \
                        rowWinLimit = rowsInLastWindow; \
                    for(winRow=0;winRow<rowWinLimit;winRow++) \
                    { \
                        if(column < (numWindowCols-1)) \
                            for(winCol=0;winCol<colSkip;winCol++) \
                            { \
                                testReal = *pixelReal; \
                                testImg = *pixelImg; \
                                maxValueSqTest = testReal*testReal + testImg*testImg; \
                                if(maxValueSq < maxValueSqTest) \
                                { \
                                    maxValueSq = maxValueSqTest; \
                                    maxReal = testReal; \
                                    maxImg = testImg; \
                                } \
                                pixelReal += 2; \
                                pixelImg += 2; \
                            } \
                        else \
                        { \
                            for(winCol=0;winCol<colsInLastWindow;winCol++) \
                            { \
                                testReal = *pixelReal; \
                                testImg = *pixelImg; \
                                maxValueSqTest = testReal*testReal + testImg*testImg; \
                                if(maxValueSq < maxValueSqTest) \
                                { \
                                    maxValueSq = maxValueSqTest; \
                                    maxReal = testReal; \
                                    maxImg = testImg; \
                                } \
                                pixelReal += 2; \
                                pixelImg += 2; \
                            } \
                            pixelReal += 2*(((size_t)colSkip) - colsInLastWindow); \
                            pixelImg += 2*(((size_t)colSkip) - colsInLastWindow); \
                        } \
                        pixelReal += winRowInc; \
                        pixelImg += winRowInc; \
                    } \
                    currentPtr += colInc; \
                    *(outp++) = maxReal; \
                    *(outp++) = maxImg; \
                } \
                currentRowPtr += rowInc; \
                outp += outRowInc; \
            } \
        } \
        \
        return(1); \
    }

static int max_down_sample_cmpx_float(nitf_DownSampler* object,
    NITF_DATA** inputWindows,
    NITF_DATA** outputWindows,
    uint32_t numBands,
    uint32_t numWindowRows,
    uint32_t numWindowCols, uint32_t numCols,
    uint32_t rowsInLastWindow,
    uint32_t colsInLastWindow)
{
    uint32_t band;           /* Current band */
    MAX_DOWN_SAMPLE_CMPX_(float);
}
static int max_down_sample_cmpx_double(nitf_DownSampler* object,
    NITF_DATA** inputWindows,
    NITF_DATA** outputWindows,
    uint32_t numBands,
    uint32_t numWindowRows,
    uint32_t numWindowCols, uint32_t numCols,
    uint32_t rowsInLastWindow,
    uint32_t colsInLastWindow)
{
    uint32_t band;           /* Current band */
    MAX_DOWN_SAMPLE_CMPX_(double);
}
#define MAX_DOWN_SAMPLE_CMPX(type) max_down_sample_cmpx_ ## type (\
    object, inputWindows, outputWindows, numBands, numWindowRows, numWindowCols, numCols, rowsInLastWindow, colsInLastWindow)



NITFPRIV(NITF_BOOL) MaxDownSample_apply(nitf_DownSampler * object,
                                        NITF_DATA ** inputWindows,
                                        NITF_DATA ** outputWindows,
                                        uint32_t numBands,
                                        uint32_t numWindowRows,
                                        uint32_t numWindowCols,
                                        uint32_t numInputCols,
                                        uint32_t numCols,
                                        uint32_t pixelType,
                                        uint32_t pixelSize,
                                        uint32_t rowsInLastWindow,
                                        uint32_t colsInLastWindow,
                                        nitf_Error * error)
{
    uint32_t band;           /* Current band */

    if (pixelType == NITF_PIXEL_TYPE_INT)
    {
        switch (pixelSize)
        {
            case 1:
		MAX_DOWN_SAMPLE(uint8_t)
            case 2:
                MAX_DOWN_SAMPLE(uint16_t)
            case 4:
                MAX_DOWN_SAMPLE(uint32_t)
            case 8:
                MAX_DOWN_SAMPLE(uint64_t)
            default:
                nitf_Error_init(error, "Invalid pixel type",
                                NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
                return (0);
        }
    }
    else if (pixelType == NITF_PIXEL_TYPE_B)
    {
        MAX_DOWN_SAMPLE(uint8_t)
    }
    else if (pixelType == NITF_PIXEL_TYPE_SI)
    {
        switch (pixelSize)
        {
            case 1:
                MAX_DOWN_SAMPLE(int8_t)
            case 2:
                MAX_DOWN_SAMPLE(int16_t)
            case 4:
                MAX_DOWN_SAMPLE(int32_t)
            case 8:
                MAX_DOWN_SAMPLE(int64_t)

            default:
                nitf_Error_init(error, "Invalid pixel type",
                                NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
                return (0);
        }
    }
    else if (pixelType == NITF_PIXEL_TYPE_R)
    {
        switch (pixelSize)
        {
            case 4:
                MAX_DOWN_SAMPLE(float)
            case 8:
                MAX_DOWN_SAMPLE(double)
            default:
                nitf_Error_init(error, "Invalid pixel type",
                                NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
                return (0);
        }
    }
    else if (pixelType == NITF_PIXEL_TYPE_C)
    {
        switch (pixelSize)
        {
            case 8:
                MAX_DOWN_SAMPLE_CMPX(float);
            case 16:               /* This case may not be possible */
                MAX_DOWN_SAMPLE_CMPX(double);
            default:
                nitf_Error_init(error, "Invalid pixel type",
                                NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
                return (0);
        }
    }
    else
    {
        nitf_Error_init(error, "Invalid pixel type",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return (0);
    }
}

NITFPRIV(void) MaxDownSample_destruct(NITF_DATA * data)
{
    (void)data;
    return;                     /* There is no instance data */
}

NITFAPI(nitf_DownSampler *) nitf_MaxDownSample_construct(uint32_t
        rowSkip,
        uint32_t
        colSkip,
        nitf_Error *
        error)
{

    static nitf_IDownSampler iMaxDownSample =
        {
            &MaxDownSample_apply,
            &MaxDownSample_destruct
        };

    nitf_DownSampler *downsampler;

    downsampler =
        (nitf_DownSampler *) NITF_MALLOC(sizeof(nitf_DownSampler));
    if (!downsampler)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    downsampler->rowSkip = rowSkip;
    downsampler->colSkip = colSkip;
    downsampler->multiBand = 0;
    downsampler->minBands = 1;
    downsampler->maxBands = 0;
    downsampler->types = NITF_DOWNSAMPLER_TYPE_ALL;
    downsampler->data = NULL;

    downsampler->iface = &iMaxDownSample;
    return downsampler;
}

/*
*      Sum of square, two band, down-sample method
*
*   The maximum is calculated as the sum of the sum of squares of two bands.
* The caller must supply exactly two bands. The complex pixel type as the
* individual band pixel type is not supported
*/

/*  In the following, the row and column indexes are the row in the array of
* windows, the winRow and winColumn are the row and column within the current
* sample window
*
*/

#define SUM_SQ_2_DOWN_SAMPLE(type) \
    { \
        uint32_t colSkip;     /* Column skip */ \
        uint32_t rowSkip;     /* Row skip */ \
        uint32_t colInc;      /* Column increment */ \
        uint32_t rowInc;      /* Pointer increment for end of row */ \
        uint32_t outRowInc;   /* Output pointer increment for end of row */ \
        uint32_t winRowInc;   /* Row increment, current window */ \
        uint32_t row;         /* Current row */ \
        uint32_t column;      /* Current column */ \
        uint32_t winRow;      /* Current row in current window */ \
        uint32_t winCol;      /* Current column current window */ \
        uint32_t rowWinLimit; /* Number of rows in current window */ \
        type *outp0;             /* Pointer into output, band 0 */ \
        type *outp1;             /* Pointer into output, band 0 */ \
        type *currentRowPtr0;    /* Pointer to the current window row, band 0 */ \
        type *currentRowPtr1;    /* Pointer to the current window row, band 1 */ \
        type *currentPtr0;       /* Pointer to the current window UL \
        corner, band 0 */ \
        type *currentPtr1;       /* Pointer to the current window UL \
        corner, band 1 */ \
        type *pixel0;            /* Pointer to the current pixel, band 0 */ \
        type *pixel1;            /* Pointer to the current pixel, band 1 */ \
        float maxValue;          /* Current maximum test value */ \
        float maxTest;           /* Test maximum value */ \
        type value0;             /* Max value pixel band 0 value */ \
        type value1;             /* Max value pixel band 1 value */ \
        \
        colSkip = object->colSkip; \
        rowSkip = object->rowSkip; \
        colInc = colSkip; \
        rowInc = numInputCols*colSkip*rowSkip; \
        winRowInc = numInputCols-colSkip; \
        outRowInc = numCols - numWindowCols; \
        \
        currentRowPtr0 = (type *) inputWindows[0]; \
        currentRowPtr1 = (type *) inputWindows[1]; \
        outp0 = (type *) outputWindows[0]; \
        outp1 = (type *) outputWindows[1]; \
        for(row=0;row<numWindowRows;row++) \
        { \
            currentPtr0 = currentRowPtr0; \
            currentPtr1 = currentRowPtr1; \
            for(column=0;column<numWindowCols;column++) \
            { \
                pixel0 = currentPtr0; \
                pixel1 = currentPtr1; \
                maxValue = (float)*pixel0 * (float)*pixel0 + (float)*pixel1 * (float)*pixel1; \
                value0 = *pixel0; \
                value1 = *pixel1; \
                if(row < (numWindowRows-1)) \
                    rowWinLimit = rowSkip; \
                else \
                    rowWinLimit = rowsInLastWindow; \
                for(winRow=0;winRow<rowWinLimit;winRow++) \
                { \
                    if(column < (numWindowCols-1)) \
                        for(winCol=0;winCol<colSkip;winCol++) \
                        { \
                            maxTest = (float)*pixel0 * (float)*pixel0 + (float)*pixel1 * (float)*pixel1; \
                            if(maxValue < maxTest) \
                            { \
                                maxValue = maxTest; \
                                value0 = *pixel0; \
                                value1 = *pixel1; \
                            } \
                            ++pixel0; \
                            ++pixel1; \
                        } \
                    else \
                    { \
                        for(winCol=0;winCol<colsInLastWindow;winCol++) \
                        { \
                            maxTest = (float)*pixel0 * (float)*pixel0 + (float)*pixel1 * (float)*pixel1; \
                            if(maxValue < maxTest) \
                            { \
                                maxValue = maxTest; \
                                value0 = *pixel0; \
                                value1 = *pixel1; \
                            } \
                            ++pixel0; \
                            ++pixel1; \
                        } \
                        pixel0 += colSkip - colsInLastWindow; \
                        pixel1 += colSkip - colsInLastWindow; \
                    } \
                    pixel0 += winRowInc; \
                    pixel1 += winRowInc; \
                } \
                currentPtr0 += colInc; \
                currentPtr1 += colInc; \
                *(outp0++) = value0; \
                *(outp1++) = value1; \
            } \
            currentRowPtr0 += rowInc; \
            currentRowPtr1 += rowInc; \
            outp0 += outRowInc; \
            outp1 += outRowInc; \
        } \
        \
        return(1); \
    } \

    NITFPRIV(NITF_BOOL) SumSq2DownSample_apply(nitf_DownSampler * object,
NITF_DATA ** inputWindows,
NITF_DATA ** outputWindows,
uint32_t numBands,
uint32_t numWindowRows,
uint32_t numWindowCols,
uint32_t numInputCols,
uint32_t numCols,
uint32_t pixelType,
uint32_t pixelSize,
uint32_t rowsInLastWindow,
uint32_t colsInLastWindow,
nitf_Error * error)
{

    if (numBands != 2)
    {
        nitf_Error_init(error, "Read request must be exactly 2 bands",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return (0);
    }

    if (pixelType == NITF_PIXEL_TYPE_INT)
    {
        switch (pixelSize)
        {
            case 1:
                SUM_SQ_2_DOWN_SAMPLE(uint8_t)
            case 2:
                SUM_SQ_2_DOWN_SAMPLE(uint16_t)
            case 4:
                SUM_SQ_2_DOWN_SAMPLE(uint32_t)
            case 8:
                SUM_SQ_2_DOWN_SAMPLE(uint64_t)
            default:
                nitf_Error_init(error, "Invalid pixel type",
                                NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
                return (0);
        }
    }
    else if (pixelType == NITF_PIXEL_TYPE_B)
    {
        SUM_SQ_2_DOWN_SAMPLE(uint8_t)
    }
    else if (pixelType == NITF_PIXEL_TYPE_SI)
    {
        switch (pixelSize)
        {
            case 1:
                SUM_SQ_2_DOWN_SAMPLE(int8_t)
            case 2:
                SUM_SQ_2_DOWN_SAMPLE(int16_t)
            case 4:
                SUM_SQ_2_DOWN_SAMPLE(int32_t)
            case 8:
                SUM_SQ_2_DOWN_SAMPLE(int64_t)
            default:
                nitf_Error_init(error, "Invalid pixel type",
                                NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
                return (0);
        }
    }
    else if (pixelType == NITF_PIXEL_TYPE_R)
    {
        switch (pixelSize)
        {
            case 4:
                SUM_SQ_2_DOWN_SAMPLE(float)
            case 8:
                SUM_SQ_2_DOWN_SAMPLE(double)
            default:
                nitf_Error_init(error, "Invalid pixel type",
                                NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
                return (0);
        }
    }
    else if (pixelType == NITF_PIXEL_TYPE_C)
    {
        nitf_Error_init(error, "Unsupported pixel type",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return (0);
    }
    else
    {
        nitf_Error_init(error, "Invalid pixel type",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return (0);
    }
}

NITFPRIV(void) SumSq2DownSample_destruct(NITF_DATA * data)
{
    (void)data;
    return;                     /* There is no instance data */
}

NITFAPI(nitf_DownSampler *) nitf_SumSq2DownSample_construct(uint32_t
        rowSkip,
        uint32_t
        colSkip,
        nitf_Error *
        error)
{

    static nitf_IDownSampler iSumSq2DownSample =
        {
            &SumSq2DownSample_apply,
            &SumSq2DownSample_destruct
        };

    nitf_DownSampler *downsampler;

    downsampler =
        (nitf_DownSampler *) NITF_MALLOC(sizeof(nitf_DownSampler));
    if (!downsampler)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    downsampler->rowSkip = rowSkip;
    downsampler->colSkip = colSkip;
    downsampler->multiBand = 1;
    downsampler->minBands = 2;
    downsampler->maxBands = 2;
    downsampler->types = NITF_DOWNSAMPLER_TYPE_ALL_BUT_COMPLEX;
    downsampler->data = NULL;

    downsampler->iface = &iSumSq2DownSample;
    return downsampler;
}


/*
*      First band select, two band, down-sample method
*
*   The maximum is calculated as the value of the first band. The caller must
* supply exactly two bands. The complex pixel type as the
* individual band pixel type is not supported
*/

/*  In the following, the row and column indexes are the row in the array of
* windows, the winRow and winColumn are the row and column within the current
* sample window
*
*/

#define SELECT_2_DOWN_SAMPLE(type) \
    { \
        uint32_t colSkip;     /* Column skip */ \
        uint32_t rowSkip;     /* Row skip */ \
        uint32_t colInc;      /* Column increment */ \
        uint32_t rowInc;      /* Pointer increment for end of row */ \
        uint32_t outRowInc;   /* Output pointer increment for end of row */ \
        uint32_t winRowInc;   /* Row increment, current window */ \
        uint32_t row;         /* Current row */ \
        uint32_t column;      /* Current column */ \
        uint32_t winRow;      /* Current row in current window */ \
        uint32_t winCol;      /* Current column current window */ \
        uint32_t rowWinLimit; /* Number of rows in current window */ \
        type *outp0;             /* Pointer into output, band 0 */ \
        type *outp1;             /* Pointer into output, band 0 */ \
        type *currentRowPtr0;    /* Pointer to the current window row, band 0 */ \
        type *currentRowPtr1;    /* Pointer to the current window row, band 1 */ \
        type *currentPtr0;       /* Pointer to the current window UL \
        corner, band 0 */ \
        type *currentPtr1;       /* Pointer to the current window UL \
        corner, band 1 */ \
        type *pixel0;            /* Pointer to the current pixel, band 0 */ \
        type *pixel1;            /* Pointer to the current pixel, band 1 */ \
        float maxValue;          /* Current maximum test value */ \
        type value0;             /* Max value pixel band 0 value */ \
        type value1;             /* Max value pixel band 1 value */ \
        \
        colSkip = object->colSkip; \
        rowSkip = object->rowSkip; \
        colInc = colSkip; \
        rowInc = numInputCols*colSkip*rowSkip; \
        winRowInc = numInputCols-colSkip; \
        outRowInc = numCols - numWindowCols; \
        \
        currentRowPtr0 = (type *) inputWindows[0]; \
        currentRowPtr1 = (type *) inputWindows[1]; \
        outp0 = (type *) outputWindows[0]; \
        outp1 = (type *) outputWindows[1]; \
        for(row=0;row<numWindowRows;row++) \
        { \
            currentPtr0 = currentRowPtr0; \
            currentPtr1 = currentRowPtr1; \
            for(column=0;column<numWindowCols;column++) \
            { \
                pixel0 = currentPtr0; \
                pixel1 = currentPtr1; \
                maxValue = (float)*pixel0; \
                value0 = *pixel0; \
                value1 = *pixel1; \
                if(row < (numWindowRows-1)) \
                    rowWinLimit = rowSkip; \
                else \
                    rowWinLimit = rowsInLastWindow; \
                for(winRow=0;winRow<rowWinLimit;winRow++) \
                { \
                    if(column < (numWindowCols-1)) \
                        for(winCol=0;winCol<colSkip;winCol++) \
                        { \
                            if(maxValue < *pixel0) \
                            { \
                                maxValue = (float)*pixel0; \
                                value0 = *pixel0; \
                                value1 = *pixel1; \
                            } \
                            ++pixel0; \
                            ++pixel1; \
                        } \
                    else \
                    { \
                        for(winCol=0;winCol<colsInLastWindow;winCol++) \
                        { \
                            if(maxValue < *pixel0) \
                            { \
                                maxValue = (float)*pixel0; \
                                value0 = *pixel0; \
                                value1 = *pixel1; \
                            } \
                            ++pixel0; \
                            ++pixel1; \
                        } \
                        pixel0 += colSkip - colsInLastWindow; \
                        pixel1 += colSkip - colsInLastWindow; \
                    } \
                    pixel0 += winRowInc; \
                    pixel1 += winRowInc; \
                } \
                currentPtr0 += colInc; \
                currentPtr1 += colInc; \
                *(outp0++) = value0; \
                *(outp1++) = value1; \
            } \
            currentRowPtr0 += rowInc; \
            currentRowPtr1 += rowInc; \
            outp0 += outRowInc; \
            outp1 += outRowInc; \
        } \
        \
        return(1); \
    } \

/*
 * VS 15.9 hits a bug in the compiler's optimizer here.
 * If we're using VS 2017, turn off optimizations for this function
 * so we can still compile.
 * TODO: Update upper bound once VS 2019 is released
*/
#if defined _MSC_VER
#if _MSC_VER >= 1910
#pragma optimize("", off)
#endif
#endif
    NITFPRIV(NITF_BOOL) Select2DownSample_apply(nitf_DownSampler * object,
NITF_DATA ** inputWindows,
NITF_DATA ** outputWindows,
uint32_t numBands,
uint32_t numWindowRows,
uint32_t numWindowCols,
uint32_t numInputCols,
uint32_t numCols,
uint32_t pixelType,
uint32_t pixelSize,
uint32_t rowsInLastWindow,
uint32_t colsInLastWindow,
nitf_Error * error)
{

    if (numBands != 2)
    {
        nitf_Error_init(error, "Read request must be exactly 2 bands",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return (0);
    }

    if (pixelType == NITF_PIXEL_TYPE_INT)
    {
        switch (pixelSize)
        {
            case 1:
                SELECT_2_DOWN_SAMPLE(uint8_t)
            case 2:
                SELECT_2_DOWN_SAMPLE(uint16_t)
            case 4:
                SELECT_2_DOWN_SAMPLE(uint32_t)
            case 8:
                SELECT_2_DOWN_SAMPLE(uint64_t)
            default:
                nitf_Error_init(error, "Invalid pixel type",
                                NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
                return (0);
        }
    }
    else if (pixelType == NITF_PIXEL_TYPE_B)
    {
        SELECT_2_DOWN_SAMPLE(uint8_t)
    }
    else if (pixelType == NITF_PIXEL_TYPE_SI)
    {
        switch (pixelSize)
        {
            case 1:
                SELECT_2_DOWN_SAMPLE(int8_t)
            case 2:
                SELECT_2_DOWN_SAMPLE(int16_t)
            case 4:
                SELECT_2_DOWN_SAMPLE(int32_t)
            case 8:
                SELECT_2_DOWN_SAMPLE(int64_t)
            default:
                nitf_Error_init(error, "Invalid pixel type",
                                NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
                return (0);
        }
    }
    else if (pixelType == NITF_PIXEL_TYPE_R)
    {
        switch (pixelSize)
        {
            case 4:
                SELECT_2_DOWN_SAMPLE(float)
            case 8:
                SELECT_2_DOWN_SAMPLE(double)
            default:
                nitf_Error_init(error, "Invalid pixel type",
                                NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
                return (0);
        }
    }
    else if (pixelType == NITF_PIXEL_TYPE_C)
    {
        nitf_Error_init(error, "Unsupported pixel type",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return (0);
    }
    else
    {
        nitf_Error_init(error, "Invalid pixel type",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return (0);
    }
}

NITFPRIV(void) Select2DownSample_destruct(NITF_DATA * data)
{
    (void)data;
    return;                     /* There is no instance data */
}

NITFAPI(nitf_DownSampler *) nitf_Select2DownSample_construct(uint32_t
        rowSkip,
        uint32_t
        colSkip,
        nitf_Error *
        error)
{

    static nitf_IDownSampler iSelect2DownSample =
        {
            &Select2DownSample_apply,
            &Select2DownSample_destruct
        };

    nitf_DownSampler *downsampler;

    downsampler =
        (nitf_DownSampler *) NITF_MALLOC(sizeof(nitf_DownSampler));
    if (!downsampler)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    downsampler->rowSkip = rowSkip;
    downsampler->colSkip = colSkip;
    downsampler->multiBand = 1;
    downsampler->minBands = 2;
    downsampler->maxBands = 2;
    downsampler->types = NITF_DOWNSAMPLER_TYPE_ALL_BUT_COMPLEX;
    downsampler->data = NULL;

    downsampler->iface = &iSelect2DownSample;
    return downsampler;
}
