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
 *    Implementation of the row source object
 */

#include "nitf/RowSource.h"

/*   The instance data for the rowSource object */

typedef struct _RowSourceImpl
{
    /*          Saved constructor arguments */
    void *algorithm;            /* The algorithm object */
    /* Pointer to the next row function */
    NITF_ROW_SOURCE_NEXT_ROW nextRow;
    uint32_t band;           /* Associate output band */
    uint32_t numRows;        /* Number of rows */
    uint32_t rowLength;      /* Length of each row in bytes (single band) */

    uint8_t *rowBuffer;      /* The row buffer */
    uint8_t *nextPtr;        /* Points to next byte to be transfered */
    uint64_t bytesLeft;      /* Bytes left to be processed */
}
RowSourceImpl;

/*
 *     RowSource_read - Read data function for row source
 */

/* Instance data */
/* Output buffer */
NITFPRIV(NITF_BOOL) RowSource_read(NITF_DATA * data, void* buf, nitf_Off size,    /* Amount to read */
                                   nitf_Error * error)  /* For error returns */
{
    RowSourceImpl *impl;        /* Instance data */
    uint64_t xfrCount;       /* Transfer count */
    uint64_t remainder;      /* Amount left to transfer */
    char *bufPtr;               /* Current location in output buffer */

    impl = (RowSourceImpl *) data;

    remainder = size;
    bufPtr = (char*)buf;
    while (remainder > 0)
    {
        if (impl->bytesLeft == 0)       /* Need more data */
        {
            if (!((*(impl->nextRow)) (impl->algorithm,
                                      impl->band, impl->rowBuffer, error)))
                return (NITF_FAILURE);
            impl->bytesLeft = impl->rowLength;
            impl->nextPtr = impl->rowBuffer;
        }

        xfrCount =
            (remainder <= impl->bytesLeft) ? remainder : impl->bytesLeft;

        /* Transfer data */
        memcpy(bufPtr, impl->nextPtr, xfrCount);

        impl->nextPtr += xfrCount;      /* Update pointers and counts */
        bufPtr += xfrCount;
        impl->bytesLeft -= xfrCount;
        remainder -= xfrCount;
    }

    return (NITF_SUCCESS);
}


/*
 *     RowSource_destruct - Destructor for instance data
 */

/* Instance data to destroy */
NITFPRIV(void) RowSource_destruct(NITF_DATA * data)
{
    RowSourceImpl *impl = (RowSourceImpl *) data;
    if (impl)
    {
        if (impl->rowBuffer != NULL)
            NITF_FREE(impl->rowBuffer);
        NITF_FREE(impl);
    }
}


NITFPRIV(nitf_Off) RowSource_getSize(NITF_DATA * data, nitf_Error *e)
{
    (void)e;

    RowSourceImpl *impl = (RowSourceImpl *) data;
    return (nitf_Off)impl->numRows * (nitf_Off)impl->rowLength;
}

NITFPRIV(NITF_BOOL) RowSource_setSize(NITF_DATA * data, nitf_Off size, nitf_Error *e)
{
    (void)data;
    (void)size;
    (void)e;
    return NITF_SUCCESS;
}


/*   BandSource interface structure, static is ok since this is read-only */

static nitf_IDataSource iRowSource =
    {
        RowSource_read,
        RowSource_destruct,
        RowSource_getSize,
        RowSource_setSize
    };

/*========================= nitf_RowSource_construct =====================*/

NITFAPI(nitf_BandSource *) nitf_RowSource_construct(void *algorithm,
        NITF_ROW_SOURCE_NEXT_ROW
        nextRow,
        uint32_t band,
        uint32_t numRows,
        uint32_t rowLength,
        nitf_Error * error)
{
    nitf_BandSource *source;    /* The result */
    RowSourceImpl *impl;        /* Instance data */

    /*   Allocate and initialize instance data */

    impl = (RowSourceImpl *) NITF_MALLOC(sizeof(RowSourceImpl));
    if (impl == NULL)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    impl->algorithm = algorithm;
    impl->nextRow = nextRow;
    impl->band = band;
    impl->numRows = numRows;
    impl->rowLength = rowLength;

    impl->rowBuffer = (uint8_t*) NITF_MALLOC(rowLength);
    if (impl->rowBuffer == NULL)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        RowSource_destruct(impl);
        return (NULL);
    }

    impl->nextPtr = impl->rowBuffer;
    impl->bytesLeft = 0;

    /*    Allocate and initialize result */

    source = (nitf_BandSource *) NITF_MALLOC(sizeof(nitf_BandSource));
    if (source == NULL)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    source->data = impl;
    source->iface = &iRowSource;
    return (source);
}
