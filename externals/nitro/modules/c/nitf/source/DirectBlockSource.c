/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2013 - 2014, MDA Information Systems LLC
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
 *    Implementation of the direct block source object
 */

#include <inttypes.h>

#include "nitf/DirectBlockSource.h"

/*   The instance data for the rowSource object */

typedef struct _DirectBlockSourceImpl
{
    /*          Saved constructor arguments */
    void *algorithm;            /* The algorithm object */
    /* Pointer to the next row function */
    NITF_DIRECT_BLOCK_SOURCE_NEXT_BLOCK nextBlock;
    nitf_ImageReader* imageReader;
    uint32_t blockNumber;
    size_t numBlocks;
}
DirectBlockSourceImpl;

NITFPRIV(DirectBlockSourceImpl *) toDirectBlockSource(NITF_DATA * data,
                                                      nitf_Error * error)
{
    DirectBlockSourceImpl *directBlockSource = (DirectBlockSourceImpl *) data;
    if (directBlockSource == NULL)
    {
        nitf_Error_init(error, "Null pointer reference",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NULL;
    }
    return directBlockSource;
}


NITFPRIV(NITF_BOOL) DirectBlockSource_read(NITF_DATA * data, void *buf,
                                           nitf_Off size,
                                           nitf_Error * error)
{
    DirectBlockSourceImpl *directBlockSource = toDirectBlockSource(data, error);
    const void* block;
    uint64_t blockSize;

    if (!directBlockSource)
        return NITF_FAILURE;

    block = nitf_ImageIO_readBlockDirect(directBlockSource->imageReader->imageDeblocker,
                                         directBlockSource->imageReader->input,
                                         directBlockSource->blockNumber++,
                                         &blockSize, error);
    if(!block)
        return NITF_FAILURE;

    if (blockSize != (uint64_t)size)
    {
        nitf_Error_initf(error, NITF_CTXT,
                         NITF_ERR_READING_FROM_FILE,
                         "Expected %lu bytes, but read %"PRIu64"",
                         size, blockSize);
        return NITF_FAILURE;
    }

    if(!directBlockSource->nextBlock(directBlockSource->algorithm,
                                     buf,
                                     block,
                                     directBlockSource->blockNumber-1,
                                     blockSize,
                                     error))
    {
        return NITF_FAILURE;
    }
    return NITF_SUCCESS;
}


/*
 *     DirectBlockSource_destruct - Destructor for instance data
 */

/* Instance data to destroy */
NITFPRIV(void) DirectBlockSource_destruct(NITF_DATA * data)
{
    DirectBlockSourceImpl *impl = (DirectBlockSourceImpl *) data;
    if (impl)
    {
        NITF_FREE(impl);
    }
}

NITFPRIV(nitf_Off) DirectBlockSource_getSize(NITF_DATA * data, nitf_Error *e)
{
    (void)e;
    DirectBlockSourceImpl *directBlockSource = (DirectBlockSourceImpl *) data;
    return directBlockSource ? directBlockSource->numBlocks : 0;
}

NITFPRIV(NITF_BOOL) DirectBlockSource_setSize(NITF_DATA * data, nitf_Off numBlocks, nitf_Error *e)
{
    DirectBlockSourceImpl *directBlockSource = (DirectBlockSourceImpl *) data;
    if (!directBlockSource)
    {
        nitf_Error_init(e, "Null pointer reference",
                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }
    directBlockSource->numBlocks = numBlocks;
    return NITF_SUCCESS;
}

NITFAPI(nitf_BandSource *) nitf_DirectBlockSource_construct(void * algorithm,
                                                            NITF_DIRECT_BLOCK_SOURCE_NEXT_BLOCK
                                                            nextBlock,
                                                            nitf_ImageReader* imageReader,
                                                            uint32_t numBands,
                                                            nitf_Error * error)
{
    static nitf_IDataSource iDirectBlockSource =
    {
        &DirectBlockSource_read,
        &DirectBlockSource_destruct,
        &DirectBlockSource_getSize,
        &DirectBlockSource_setSize
    };
    DirectBlockSourceImpl *impl;
    nitf_BandSource *bandSource;
    nitf_BlockingInfo* blockInfo;

    impl = (DirectBlockSourceImpl *) NITF_MALLOC(sizeof(DirectBlockSourceImpl));
    if (!impl)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }

    if(!nitf_ImageIO_setupDirectBlockRead(imageReader->imageDeblocker,
                                          imageReader->input,
                                          numBands,
                                          error))
        return NITF_FAILURE;

    blockInfo = nitf_ImageReader_getBlockingInfo(imageReader, error);
    if (blockInfo == NULL)
        return NITF_FAILURE;

    const size_t numBlocks = ((size_t)blockInfo->numBlocksPerRow) * blockInfo->numBlocksPerCol;

    nitf_BlockingInfo_destruct(&blockInfo);

    impl->algorithm = algorithm;
    impl->nextBlock = nextBlock;
    impl->imageReader = imageReader;
    impl->blockNumber = 0;
    impl->numBlocks = numBlocks;

    bandSource = (nitf_BandSource *) NITF_MALLOC(sizeof(nitf_BandSource));
    if (!bandSource)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }
    bandSource->data = impl;
    bandSource->iface = &iDirectBlockSource;
    return bandSource;
}

