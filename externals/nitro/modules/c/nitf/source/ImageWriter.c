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

#include "nitf/ImageWriter.h"
#include "nitf/ImageIO.h"
#include "nitf/PluginRegistry.h"

/*
 *  Private implementation struct
 */
typedef struct _ImageWriterImpl
{
    uint32_t numBitsPerPixel;
    uint32_t numImageBands;
    uint32_t numMultispectralImageBands;
    uint32_t numRows;
    uint32_t numCols;
    nitf_ImageSource *imageSource;
    nitf_ImageIO *imageBlocker;
    NRT_BOOL directBlockWrite;

} ImageWriterImpl;



NITFPRIV(void) ImageWriter_destruct(NITF_DATA * data)
{
    ImageWriterImpl *impl = (ImageWriterImpl *) data;

    if (impl)
    {
        if (impl->imageBlocker)
            nitf_ImageIO_destruct(&impl->imageBlocker);
        if (impl->imageSource)
            nitf_ImageSource_destruct(&impl->imageSource);
        NITF_FREE(impl);
    }
}

static void free_user_bands(uint8_t** user, size_t numImageBands)
{
    if (user != NULL)
    {
        size_t band = 0;
        for (; band < numImageBands; band++)
        {
            uint8_t* userBand = user[band];
            if (userBand != NULL)
                NITF_FREE(userBand);
        }
        NITF_FREE(user);
    }
}

static uint8_t** malloc_user_bands(size_t numImageBands, size_t rowSize)
{
    uint8_t** user = (uint8_t**)NITF_MALLOC(sizeof(uint8_t*) * numImageBands);
    if (!user)
    {
        return NULL;
    }

    size_t band;
    for (band = 0; band < numImageBands; band++)
    {
        user[band] = NULL; // initialize all to NULL for CLEANUP
    }
    for (band = 0; band < numImageBands; band++)
    {
        user[band] = (uint8_t*)NITF_MALLOC(rowSize);
        if (!user[band])
        {
            free_user_bands(user, numImageBands);
            return NULL;
        }
    }
    return user;
}


NITFPRIV(NITF_BOOL) ImageWriter_write(NITF_DATA * data,
                                      nitf_IOInterface* output,
                                      nitf_Error * error)
{
    uint8_t **user = NULL;
    uint8_t *userContig = NULL;
    uint32_t row, band, block;
    nitf_BandSource *bandSrc = NULL;
    nitf_BlockingInfo* blockInfo = NULL;
    nitf_ImageIO* imageIO = NULL;
    ImageWriterImpl *impl = (ImageWriterImpl *) data;
    NITF_BOOL rc = NITF_SUCCESS;
    int writeComplete = 0;

    const uint32_t numImageBands = impl->numImageBands + impl->numMultispectralImageBands;
    const size_t rowSize = impl->numCols * NITF_NBPP_TO_BYTES(impl->numBitsPerPixel);


    const nitf_Off offset = nitf_IOInterface_tell(output, error);
    if (!NITF_IO_SUCCESS(offset))
        goto CATCH_ERROR;

    if (!nitf_ImageIO_setFileOffset(impl->imageBlocker, offset, error))
        goto CATCH_ERROR;

    if (!nitf_ImageIO_writeSequential(impl->imageBlocker, output, error))
        goto CATCH_ERROR;

    /* Direct block write mode only supported for a single band or single block currently */
    if(impl->directBlockWrite)
    {
        imageIO = impl->imageBlocker;

        blockInfo = nitf_ImageIO_getBlockingInfo(imageIO, output, error);
        if (blockInfo == NULL)
            return NITF_FAILURE;

        const size_t numBlocks = ((size_t)blockInfo->numBlocksPerRow) * blockInfo->numBlocksPerCol;
        const size_t blockSize = blockInfo->length;

        nitf_BlockingInfo_destruct(&blockInfo);

        if (numBlocks == 1 || numImageBands == 1)
        {
            userContig = (nitf_Uint8 *) NITF_MALLOC(sizeof(nitf_Uint8*) * blockSize);
            if (!userContig)
            {
                nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                                NITF_ERR_MEMORY);
                goto CATCH_ERROR;
            }

            band = 0;

            /* For each block, read the data and write out as-is without any re-arranging the data
               Data should be copied into userContig as the underlying block will be discarded
               with each read */
            for(block = 0; block < numBlocks; ++block)
            {
                bandSrc = nitf_ImageSource_getBand(impl->imageSource,
                                                   band, error);
                if (bandSrc == NULL)
                    return NITF_FAILURE;

                /* Assumes this will be reading block number 'block' */
                if (!(*(bandSrc->iface->read)) (bandSrc->data, (char *) userContig,
                                                (size_t) blockSize, error))
                {
                    goto CATCH_ERROR;

                }

                if(!nitf_ImageIO_writeBlockDirect(imageIO,
                                                  output,
                                                  userContig,
                                                  block,
                                                  error))
                    goto CATCH_ERROR;
            }
            writeComplete = 1;
        }
    }
    if (writeComplete == 0)
    {
        user = malloc_user_bands(numImageBands, rowSize);
        if (!user)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                            NITF_ERR_MEMORY);
            goto CATCH_ERROR;
        }

        for (row = 0; row < impl->numRows; ++row)
        {
            for (band = 0; band < numImageBands; ++band)
            {
                bandSrc = nitf_ImageSource_getBand(impl->imageSource,
                                                   band, error);
                if (bandSrc == NULL)
                    return NITF_FAILURE;

                if (!(*(bandSrc->iface->read)) (bandSrc->data, (char *) user[band],
                                                (size_t) rowSize, error))
                {
                    goto CATCH_ERROR;
                }
            }

            if (!nitf_ImageIO_writeRows(impl->imageBlocker, output, 1, user, error))
                goto CATCH_ERROR;
        }
    }

    if (!nitf_ImageIO_writeDone(impl->imageBlocker, output, error))
        goto CATCH_ERROR;

    goto CLEANUP;

CATCH_ERROR:
    rc = NITF_FAILURE;

CLEANUP:
    free_user_bands(user, numImageBands);
    if(userContig != NULL)
        NITF_FREE(userContig);
    return rc;
}


NITFAPI(nitf_ImageWriter *) nitf_ImageWriter_construct(
    nitf_ImageSubheader *subheader,
    nrt_HashTable* options,
    nitf_Error * error)
{
    static nitf_IWriteHandler iWriteHandler =
    {
        &ImageWriter_write,
        &ImageWriter_destruct
    };

    ImageWriterImpl *impl = NULL;
    nitf_ImageWriter *imageWriter = NULL;
    char compBuf[NITF_IC_SZ + 1];       /* holds the compression string */
    nitf_CompressionInterface *compIface = NULL;

    impl = (ImageWriterImpl *) NITF_MALLOC(sizeof(ImageWriterImpl));
    if (!impl)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(impl, 0, sizeof(ImageWriterImpl));

    NITF_TRY_GET_UINT32(subheader->numBitsPerPixel, &impl->numBitsPerPixel, error);
    NITF_TRY_GET_UINT32(subheader->numImageBands, &impl->numImageBands, error);
    NITF_TRY_GET_UINT32(subheader->numMultispectralImageBands, &impl->numMultispectralImageBands, error);
    NITF_TRY_GET_UINT32(subheader->numRows, &impl->numRows, error);
    NITF_TRY_GET_UINT32(subheader->numCols, &impl->numCols, error);

    impl->imageSource = NULL;
    impl->directBlockWrite = 0;


    /* Check for compression and get compression interface */
    /* get the compression string */

    if (!nitf_Field_get(subheader->NITF_IC,
            compBuf, NITF_CONV_STRING, NITF_IC_SZ + 1, error))
   {
       goto CATCH_ERROR;
   }

    if(memcmp(compBuf, "NC", 2) != 0 && memcmp(compBuf, "NM", 2) != 0)
    {
        /* get the compression interface */
        compIface = nitf_PluginRegistry_retrieveCompInterface(compBuf, error);
        if (compIface == NULL)
        {
            goto CATCH_ERROR;
        }
    }

    impl->imageBlocker = nitf_ImageIO_construct(subheader, 0, 0,
                                                compIface,
                                                NULL,
                                                options,
                                                error);
    if (!impl->imageBlocker)
        goto CATCH_ERROR;

    imageWriter = (nitf_ImageWriter *) NITF_MALLOC(sizeof(nitf_ImageWriter));
    if (!imageWriter)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(imageWriter, 0, sizeof(nitf_ImageWriter));

    imageWriter->data = impl;
    imageWriter->iface = &iWriteHandler;
    return imageWriter;

  CATCH_ERROR:
    if (impl && impl->imageBlocker)
        nitf_ImageIO_destruct(&impl->imageBlocker);
    if (impl)
        NITF_FREE(impl);
    return NULL;
}


NITFAPI(NITF_BOOL) nitf_ImageWriter_attachSource(nitf_ImageWriter * imageWriter,
        nitf_ImageSource *imageSource, nitf_Error * error)
{
    ImageWriterImpl *impl = (ImageWriterImpl*)imageWriter->data;

    if (impl->imageSource != NULL)
    {
        nitf_Error_init(error, "Image source already attached",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NITF_FAILURE;
    }

    impl->imageSource = imageSource;
    return NITF_SUCCESS;
}


NITFAPI(int) nitf_ImageWriter_setWriteCaching(nitf_ImageWriter *imageWriter,
        int enable)
{
    ImageWriterImpl *impl = (ImageWriterImpl*)imageWriter->data;
    return(nitf_ImageIO_setWriteCaching(impl->imageBlocker, enable));
}

NITFAPI(NITF_BOOL) nitf_ImageWriter_setDirectBlockWrite(nitf_ImageWriter *imageWriter,
        int enable,
        nitf_Error *error)
{
    ImageWriterImpl *impl;
    uint32_t numImageBands;

    impl = (ImageWriterImpl*)imageWriter->data;
    numImageBands = impl->numImageBands + impl->numMultispectralImageBands;

    if (numImageBands > 1 && enable)
    {
        nitf_Error_init(error, "Direct block write not supported for multi-band images",
            NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NITF_FAILURE;
    }
    impl->directBlockWrite = enable;
    return NITF_SUCCESS;
}

NITFAPI(NITF_BOOL) nitf_ImageWriter_setPadPixel(nitf_ImageWriter* imageWriter,
                                                uint8_t* value,
                                                uint32_t length,
                                                nitf_Error* error)
{
    ImageWriterImpl *impl = (ImageWriterImpl*)imageWriter->data;
    return nitf_ImageIO_setPadPixel(impl->imageBlocker, value, length, error);
}
