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

#include "nitf/BandSource.h"

/*
 *  Private implementation struct
 */
typedef struct _MemorySourceImpl
{
    const void* data;
    nitf_Off size;
    nitf_Off mark;
    int numBytesPerPixel;
    int pixelSkip;
    nitf_Off start;
}
MemorySourceImpl;


NITFPRIV(MemorySourceImpl *) toMemorySource(NITF_DATA * data,
        nitf_Error * error)
{
    MemorySourceImpl *memorySource = (MemorySourceImpl *) data;
    if (memorySource == NULL)
    {
        nitf_Error_init(error, "Null pointer reference",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NULL;
    }
    return memorySource;
}


NITFPRIV(NITF_BOOL) MemorySource_contigRead(
        MemorySourceImpl* memorySource,
        void* buf,
        nitf_Off size,
        nitf_Error* error)
{
    (void) error; /* Suppresses a warning. Param seems to exist for consistency purposes. Not used. */
    memcpy(buf,
           (const uint8_t*)memorySource->data + memorySource->mark,
           (size_t)size);
    memorySource->mark += size;
    return NITF_SUCCESS;
}


NITFPRIV(NITF_BOOL) MemorySource_offsetRead(
        MemorySourceImpl* memorySource,
        void* buf,
        nitf_Off size,
        nitf_Error* error)
{
    size_t destOffset = 0;
    const uint8_t* const src = (const uint8_t*)memorySource->data;
    uint8_t* const dest = (uint8_t*)buf;
    (void) error;

    while (destOffset < (size_t)size)
    {
        memcpy(dest + destOffset,
               src + memorySource->mark,
               memorySource->numBytesPerPixel);

        destOffset += memorySource->numBytesPerPixel;

        /* Bump up mark by what we just memcpy'd + what we want to skip */
        memorySource->mark += memorySource->numBytesPerPixel +
                ((nitf_Off)memorySource->pixelSkip) * memorySource->numBytesPerPixel;
    }
    return NITF_SUCCESS;
}


/*
 *  Private read implementation for memory source.
 */
NITFPRIV(NITF_BOOL) MemorySource_read(NITF_DATA * data,
                                      void* buf,
                                      nitf_Off size, nitf_Error * error)
{
    MemorySourceImpl *memorySource = toMemorySource(data, error);
    if (!memorySource)
    {
        return NITF_FAILURE;
    }

    /*  We like the contiguous read case, its fast  */
    /*  We want to make sure we reward this case    */
    if (memorySource->pixelSkip == 0)
    {
        return MemorySource_contigRead(memorySource, buf, size, error);
    }

    return MemorySource_offsetRead(memorySource, buf, size, error);
}


NITFPRIV(void) MemorySource_destruct(NITF_DATA * data)
{
    NITF_FREE(data);
}


NITFPRIV(nitf_Off) MemorySource_getSize(NITF_DATA * data, nitf_Error* error)
{
    MemorySourceImpl *memorySource = (MemorySourceImpl *) data;
    (void) error;
    return memorySource ? memorySource->size : 0;
}

NITFPRIV(NITF_BOOL) MemorySource_setSize(NITF_DATA * data, nitf_Off size, nitf_Error *error)
{
    MemorySourceImpl *memorySource = (MemorySourceImpl *) data;
    if (!memorySource)
    {
        nitf_Error_init(error, "Null pointer reference",
                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }
    memorySource->size = size;
    return NITF_SUCCESS;
}



NITFAPI(nitf_BandSource *) nitf_MemorySource_construct(const void* data,
        nitf_Off size,
        nitf_Off start,
        int numBytesPerPixel,
        int pixelSkip,
        nitf_Error * error)
{
    static nitf_IDataSource iMemorySource =
        {
            &MemorySource_read,
            &MemorySource_destruct,
            &MemorySource_getSize,
            &MemorySource_setSize
        };
    MemorySourceImpl *impl;
    nitf_BandSource *bandSource;

    impl = (MemorySourceImpl *) NITF_MALLOC(sizeof(MemorySourceImpl));
    if (!impl)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }

    impl->data = data;
    impl->size = size;
    impl->numBytesPerPixel = numBytesPerPixel > 0 ? numBytesPerPixel : 1;
    impl->mark = impl->start = (start >= 0 ? start : 0);
    impl->pixelSkip = pixelSkip >= 0 ? pixelSkip : 0;

    bandSource = (nitf_BandSource *) NITF_MALLOC(sizeof(nitf_BandSource));
    if (!bandSource)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }
    bandSource->data = impl;
    bandSource->iface = &iMemorySource;
    return bandSource;
}


/*
 *  Private implementation struct
 */
typedef struct _IOSourceImpl
{
    nitf_IOInterface *io;
    nitf_Off start;
    nitf_Off size;
    int numBytesPerPixel;
    int pixelSkip;
    nitf_Off mark;
}
IOSourceImpl;

NITFPRIV(void) IOSource_destruct(NITF_DATA * data)
{
    NITF_FREE(data);
}

NITFPRIV(void) FileSource_destruct(NITF_DATA * data)
{
    nitf_Error error;
    if (data)
    {
        IOSourceImpl* const source = (IOSourceImpl*)data;
        if (source->io)
        {
            nrt_IOInterface_close(source->io, &error);
            nrt_IOInterface_destruct(&source->io);
            source->io = NULL;
        }
        NITF_FREE(data);
    }
}

NITFPRIV(nitf_Off) IOSource_getSize(NITF_DATA * data, nitf_Error* error)
{
    IOSourceImpl *source = (IOSourceImpl *) data;
    (void) error;
    return source ? (nitf_Off)source->size : 0;
}


NITFPRIV(NITF_BOOL) IOSource_setSize(NITF_DATA * data, nitf_Off size, nitf_Error *error)
{
    IOSourceImpl *source = (IOSourceImpl *) data;
    if (!source)
    {
        nitf_Error_init(error, "Null pointer reference",
                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }
    source->size = size;
    return NITF_SUCCESS;
}


NITFPRIV(IOSourceImpl *) toIOSource(NITF_DATA * data, nitf_Error *error)
{
    IOSourceImpl *source = (IOSourceImpl *) data;
    if (source == NULL)
    {
        nitf_Error_init(error, "Null pointer reference",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NULL;
    }
    return source;
}


NITFPRIV(NITF_BOOL) IOSource_contigRead(IOSourceImpl * source,
                                        void *buf,
                                        nitf_Off size,
                                        nitf_Error * error)
{
    if (!NITF_IO_SUCCESS(nitf_IOInterface_read(source->io,
                                               buf,
                                               (size_t)size,
                                               error)))
    {
        return NITF_FAILURE;
    }
    source->mark += size;
    return NITF_SUCCESS;
}


/*
 *  The idea here is we will speed it up by creating a temporary buffer
 *  for reading from the io interface.  Even with the allocation, this should
 *  be much faster than seeking every time.
 *
 *  The basic idea is that we allocate the temporary buffer to the request
 *  size * the skip factor.  It should be noted that the tradeoff here is that,
 *  for very large read values, this may be really undesirable, especially for
 *  large skip factors.
 *
 *  If this proves to be a problem, I will revert it back to a seek/read paradigm
 *  -DP
 */
NITFPRIV(NITF_BOOL) IOSource_offsetRead(IOSourceImpl * source,
        void* buf,
        nitf_Off size, nitf_Error * error)
{

    /* we do not multiply the pixelSkip by numBytesPerPixel, b/c this
     * read method takes in size as number of bytes, not number of pixels */

    /* TODO - this *could* be smaller, but this should be ok for now */
    nitf_Off tsize = size * (((nitf_Off)source->pixelSkip) + 1);

    uint8_t* tbuf;
    uint8_t* bufPtr = (uint8_t*)buf;
    nitf_Off lmark = 0;
    int i = 0;
    int j = 0;
    if (tsize + source->mark > source->size)
        tsize = source->size - source->mark;

    tbuf = (uint8_t *) NITF_MALLOC((size_t)tsize);
    if (!tbuf)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NITF_FAILURE;
    }

    if (!nitf_IOInterface_read(source->io, tbuf, (size_t)tsize, error))
    {
        NITF_FREE(tbuf);
        return NITF_FAILURE;
    }
    /*  Downsize for buf */
    while (i < size)
    {
        for (j = 0; j < source->numBytesPerPixel; ++j, ++i, ++lmark)
        {
            bufPtr[i] = tbuf[lmark];
        }
        lmark += (((nitf_Off)source->pixelSkip) * source->numBytesPerPixel);
    }
    source->mark += lmark;
    NITF_FREE(tbuf);
    return NITF_SUCCESS;
}


/*
 *  Private read implementation for file source.
 */
NITFPRIV(NITF_BOOL) IOSource_read(NITF_DATA * data,
                                  void* buf,
                                  nitf_Off size,
                                  nitf_Error * error)
{
    IOSourceImpl *source = toIOSource(data, error);
    if (!source)
        return NITF_FAILURE;

    if (!NITF_IO_SUCCESS(nitf_IOInterface_seek(source->io,
                                               source->mark,
                                               NITF_SEEK_SET, error)))
        return NITF_FAILURE;
    if (source->pixelSkip == 0)
        return IOSource_contigRead(source, buf, size, error);
    return IOSource_offsetRead(source, buf, size, error);
}


NITFAPI(nitf_BandSource *) nitf_IOSource_construct(nitf_IOInterface *io,
        nitf_Off start,
        int numBytesPerPixel,
        int pixelSkip,
        nitf_Error * error)
{
    static nitf_IDataSource iIOSource =
    {
        &IOSource_read,
        &IOSource_destruct,
        &IOSource_getSize,
        &IOSource_setSize
    };
    IOSourceImpl *impl;
    nitf_BandSource *bandSource;

    impl = (IOSourceImpl *) NITF_MALLOC(sizeof(IOSourceImpl));
    if (!impl)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }
    impl->io = io;
    impl->numBytesPerPixel = numBytesPerPixel > 0 ? numBytesPerPixel : 1;
    impl->pixelSkip = pixelSkip >= 0 ? pixelSkip : 0;
    impl->mark = impl->start = (start >= 0 ? start : 0);
    impl->size = nitf_IOInterface_getSize(io, error);

    if (!NITF_IO_SUCCESS(impl->size))
    {
        NITF_FREE(impl);
        return NULL;
    }

    bandSource = (nitf_BandSource *) NITF_MALLOC(sizeof(nitf_BandSource));
    if (!bandSource)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }
    bandSource->data = impl;
    bandSource->iface = &iIOSource;
    return bandSource;
}

NITFAPI(nitf_BandSource *) nitf_FileSource_construct(nitf_IOHandle handle,
                                                     nitf_Off start,
                                                     int numBytesPerPixel,
                                                     int pixelSkip,
                                                     nitf_Error * error)
{
    /* We need a way to free 'interface' when we destruct - the IOSource
     * doesn't take ownership of it.
     */
    static nitf_IDataSource iFileSource =
    {
        &IOSource_read,
        &FileSource_destruct,
        &IOSource_getSize,
        &IOSource_setSize
    };

    nitf_IOInterface* ioInterface = NULL;
    nitf_BandSource* bandSource = NULL;

    ioInterface = nitf_IOHandleAdapter_construct(handle, NRT_ACCESS_READONLY,
                                                 error);
    if (ioInterface == NULL)
    {
        return NULL;
    }

    bandSource = nitf_IOSource_construct(ioInterface, start, numBytesPerPixel,
                                         pixelSkip, error);
    if (bandSource == NULL)
    {
        return NULL;
    }

    bandSource->iface = &iFileSource;
    return bandSource;
}

NITFAPI(nitf_BandSource *) nitf_FileSource_constructFile(const char* fname,
                                                         nitf_Off start,
                                                         int numBytesPerPixel,
                                                         int pixelSkip,
                                                         nitf_Error* error)
{
    /* We need a way to free 'interface' when we destruct - the IOSource
     * doesn't take ownership of it.
     */
    static nitf_IDataSource iFileSource =
    {
        &IOSource_read,
        &FileSource_destruct,
        &IOSource_getSize,
        &IOSource_setSize
    };

    nitf_IOInterface* ioInterface = NULL;
    nitf_BandSource* bandSource = NULL;

    ioInterface = nitf_IOHandleAdapter_open(fname, NRT_ACCESS_READONLY,
                                            NRT_OPEN_EXISTING, error);
    if (ioInterface == NULL)
    {
        return NULL;
    }

    bandSource = nitf_IOSource_construct(ioInterface, start, numBytesPerPixel,
                                         pixelSkip, error);
    if (bandSource == NULL)
    {
        return NULL;
    }

    bandSource->iface = &iFileSource;
    return bandSource;
}

