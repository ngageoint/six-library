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

#include <string.h>

#include "nitf/SegmentSource.h"

/*
 *  Private implementation struct
 */
typedef struct _MemorySourceImpl
{
    const void* data;
    NITF_BOOL ownData;
    nitf_Off size;
    int sizeSet;
    nitf_Off mark;
    int byteSkip;
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
        nitf_Error * error)
{
    (void)error;

    memcpy(buf,
           (const uint8_t*)memorySource->data + memorySource->mark,
           size);
    memorySource->mark += size;
    return NITF_SUCCESS;
}


NITFPRIV(NITF_BOOL) MemorySource_offsetRead(
        MemorySourceImpl* memorySource,
        void* buf,
        nitf_Off size,
        nitf_Error * error)
{
    (void)error;

    int i = 0;
    const uint8_t* src = (const uint8_t*)memorySource->data;
    uint8_t* dest = (uint8_t*)buf;

    while (i < size)
    {
        dest[i++] = src[memorySource->mark++];
        memorySource->mark += memorySource->byteSkip;
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

    /*  We like the contiguous read case, it's fast  */
    /*  We want to make sure we reward this case     */
    if (memorySource->byteSkip == 0)
    {
        return MemorySource_contigRead(memorySource, buf, size, error);
    }

    return MemorySource_offsetRead(memorySource, buf, size, error);
}


NITFPRIV(void) MemorySource_destruct(NITF_DATA * data)
{
    MemorySourceImpl *memorySource = (MemorySourceImpl *) data;
    if (memorySource)
    {
    	if (memorySource->ownData)
    	{
    		NITF_FREE((void*)memorySource->data);
    	}
        NITF_FREE(memorySource);
    }
}

NITFPRIV(nitf_Off) MemorySource_getSize(NITF_DATA * data, nitf_Error *e)
{
    (void)e;

    MemorySourceImpl *memorySource = (MemorySourceImpl *) data;
    assert(memorySource);
    const int byteSkip = memorySource->byteSkip + 1;
    return memorySource->sizeSet ? (nitf_Off)memorySource->size :
        (nitf_Off)(memorySource->size / byteSkip);
}

NITFPRIV(NITF_BOOL) MemorySource_setSize(NITF_DATA * data, nitf_Off size, nitf_Error *e)
{
    (void)e;

    MemorySourceImpl *memorySource = (MemorySourceImpl *) data;
    assert(memorySource);
    memorySource->size = size;
    memorySource->sizeSet = 1;
    return NITF_SUCCESS;
}


NITFAPI(nitf_SegmentSource *) nitf_SegmentMemorySource_construct
(
    const char* data,
    nitf_Off size,
    nitf_Off start,
    int byteSkip,
    NITF_BOOL copyData,
    nitf_Error* error
)
{
    static nitf_IDataSource iMemorySource =
        {
            &MemorySource_read,
            &MemorySource_destruct,
            &MemorySource_getSize,
            &MemorySource_setSize
        };
    MemorySourceImpl *impl = NULL;
    nitf_SegmentSource *segmentSource = NULL;

    impl = (MemorySourceImpl *) NITF_MALLOC(sizeof(MemorySourceImpl));
    if (!impl)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }

    if (copyData)
    {
    	char* dataCopy = (char*) NITF_MALLOC(size);

    	if (!dataCopy)
    	{
    		NITF_FREE(impl);
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                            NITF_ERR_MEMORY);
            return NULL;
    	}
    	memcpy(dataCopy, data, size);

    	impl->data = dataCopy;
    	impl->ownData = 1;
    }
    else
    {
    	impl->data = data;
    	impl->ownData = 0;
    }
    impl->size = size;
    impl->sizeSet = 0;
    impl->mark = impl->start = (start >= 0 ? start : 0);
    impl->byteSkip = byteSkip >= 0 ? byteSkip : 0;

    segmentSource = (nitf_SegmentSource *) NITF_MALLOC(sizeof(nitf_SegmentSource));
    if (!segmentSource)
    {
    	if (copyData)
    	{
    		NITF_FREE((void*)impl->data);
    	}
    	NITF_FREE(impl);

        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }
    segmentSource->data = impl;
    segmentSource->iface = &iMemorySource;
    return segmentSource;
}


/*
 *  Private implementation struct
 */
typedef struct _FileSourceImpl
{
    nitf_IOInterface *io;
    nitf_Off start;
    nitf_Off size;
    nitf_Off fileSize;
    int byteSkip;
    nitf_Off mark;
}
FileSourceImpl;

NITFPRIV(void) FileSource_destruct(NITF_DATA * data)
{
    nitf_Error error;
    if (data)
    {
        FileSourceImpl *fileSource = (FileSourceImpl *) data;
        if (fileSource->io)
        {
            nrt_IOInterface_close(fileSource->io, &error);
            nrt_IOInterface_destruct(&fileSource->io);
            fileSource->io = NULL;
        }
        NITF_FREE(data);
    }
}

NITFPRIV(nitf_Off) FileSource_getSize(NITF_DATA * data, nitf_Error *e)
{
    (void)e;

    FileSourceImpl *fileSource = (FileSourceImpl *) data;
    assert(fileSource);
    assert(fileSource->fileSize > fileSource->start);
    return fileSource->size;
}

NITFPRIV(NITF_BOOL) FileSource_setSize(NITF_DATA* data, nitf_Off size, nitf_Error *e)
{
    (void)e;

    /* you better know what you're doing if you set the size yourself! */
    FileSourceImpl* fileSource = (FileSourceImpl*)data;
    assert(fileSource);
    assert(fileSource->fileSize >= size);
    fileSource->size = size;
    return NITF_SUCCESS;
}

NITFPRIV(FileSourceImpl *) toFileSource(NITF_DATA * data,
                                        nitf_Error * error)
{
    FileSourceImpl *fileSource = (FileSourceImpl *) data;
    if (fileSource == NULL)
    {
        nitf_Error_init(error, "Null pointer reference",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NULL;
    }
    return fileSource;
}


NITFPRIV(NITF_BOOL) FileSource_contigRead(FileSourceImpl * fileSource,
        void* buf,
        nitf_Off size, nitf_Error * error)
{
    if (!NITF_IO_SUCCESS(nitf_IOInterface_read(fileSource->io, buf, size,
                                               error)))
        return NITF_FAILURE;
    fileSource->mark += size;
    return NITF_SUCCESS;
}


/*
 *  The idea here is we will speed it up by creating a temporary buffer
 *  for reading from the io handle.  Even with the allocation, this should
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
NITFPRIV(NITF_BOOL) FileSource_offsetRead(FileSourceImpl * fileSource,
        void* buf,
        nitf_Off size, nitf_Error * error)
{
    assert(fileSource != NULL);
    const int byteSkip = fileSource->byteSkip + 1;
    nitf_Off tsize = size * byteSkip;

    uint8_t* tbuf;
    uint8_t* bufPtr = (uint8_t*)buf;

    nitf_Off lmark = 0;
    int i = 0;
    if (tsize + fileSource->mark > fileSource->size)
        tsize = fileSource->size - fileSource->mark;

    tbuf = (uint8_t *) NITF_MALLOC(tsize);
    if (!tbuf)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NITF_FAILURE;
    }

    if (!nitf_IOInterface_read(fileSource->io, tbuf, tsize, error))
    {
        NITF_FREE(tbuf);
        return NITF_FAILURE;
    }
    /*  Downsize for buf */
    while (i < size)
    {
        bufPtr[i++] = *(tbuf + lmark++);
        lmark += (fileSource->byteSkip);
    }
    fileSource->mark += lmark;
    NITF_FREE(tbuf);
    return NITF_SUCCESS;
}


/*
 *  Private read implementation for file source.
 */
NITFPRIV(NITF_BOOL) FileSource_read(NITF_DATA * data,
                                    void* buf,
                                    nitf_Off size, nitf_Error * error)
{
    FileSourceImpl *fileSource = toFileSource(data, error);
    if (!fileSource)
        return NITF_FAILURE;

    if (!NITF_IO_SUCCESS(nitf_IOInterface_seek(fileSource->io,
                                               fileSource->mark,
                                               NITF_SEEK_SET, error)))
        return NITF_FAILURE;
    if (fileSource->byteSkip == 0)
        return FileSource_contigRead(fileSource, buf, size, error);
    return FileSource_offsetRead(fileSource, buf, size, error);
}


NITFAPI(nitf_SegmentSource *) nitf_SegmentFileSource_construct
(
    nitf_IOHandle handle,
    nitf_Off start,
    int byteSkip,
    nitf_Error * error
)
{
    static nitf_IDataSource iFileSource =
    {
        &FileSource_read,
        &FileSource_destruct,
        &FileSource_getSize,
        &FileSource_setSize
    };
    FileSourceImpl *impl = NULL;
    nitf_SegmentSource *segmentSource = NULL;

    impl = (FileSourceImpl *) NITF_MALLOC(sizeof(FileSourceImpl));
    if (!impl)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }
    impl->io = nitf_IOHandleAdapter_construct(handle,
        NRT_ACCESS_READONLY,
        error);
    if (!impl->io)
        return NULL;

    impl->byteSkip = byteSkip >= 0 ? byteSkip : 0;
    impl->mark = impl->start = (start >= 0 ? start : 0);
    impl->fileSize = nitf_IOInterface_getSize(impl->io, error);

    if (!NITF_IO_SUCCESS(impl->fileSize))
    {
        NITF_FREE(impl);
        return NULL;
    }

    /* figure out the actual # oif bytes represented by the source */
    const int byteSkip_ = impl->byteSkip + 1;
    impl->size = impl->fileSize / byteSkip_;

    segmentSource = (nitf_SegmentSource *) NITF_MALLOC(sizeof(nitf_SegmentSource));
    if (!segmentSource)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }
    segmentSource->data = impl;
    segmentSource->iface = &iFileSource;
    return segmentSource;
}


NITFAPI(nitf_SegmentSource *) nitf_SegmentFileSource_constructIO
(
    nitf_IOInterface* io,
    nitf_Off start,
    int byteSkip,
    nitf_Error * error
)
{
    static nitf_IDataSource iFileSource =
    {
        &FileSource_read,
        &FileSource_destruct,
        &FileSource_getSize,
        &FileSource_setSize
    };
    FileSourceImpl *impl = NULL;
    nitf_SegmentSource *segmentSource = NULL;

    impl = (FileSourceImpl *) NITF_MALLOC(sizeof(FileSourceImpl));
    if (!impl)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }
    impl->io = io;
    impl->byteSkip = byteSkip >= 0 ? byteSkip : 0;
    impl->mark = impl->start = (start >= 0 ? start : 0);
    impl->fileSize = nitf_IOInterface_getSize(impl->io, error);

    if (!NITF_IO_SUCCESS(impl->fileSize))
    {
        NITF_FREE(impl);
        return NULL;
    }

    /* figure out the actual # oif bytes represented by the source */
    const int byteSkip_ = impl->byteSkip + 1;
    impl->size = impl->fileSize / byteSkip_;

    segmentSource = (nitf_SegmentSource *) NITF_MALLOC(sizeof(nitf_SegmentSource));
    if (!segmentSource)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }
    segmentSource->data = impl;
    segmentSource->iface = &iFileSource;
    return segmentSource;
}


NITFPRIV(void) SegmentReader_destruct(NITF_DATA * data)
{
    (void)data;
    /* nothing... */
}

NITFPRIV(nitf_Off) SegmentReader_getSize(NITF_DATA * data, nitf_Error *e)
{
    (void)e;

    nitf_SegmentReader *reader =  (nitf_SegmentReader*)data;
    assert(reader);
    return reader->dataLength;
}

NITFPRIV(NITF_BOOL) SegmentReader_setSize(NITF_DATA* data, nitf_Off size, nitf_Error *e)
{
    (void)data;
    (void)size;
    (void)e;
    /* does nothing... - should we return an error instead? */
    return NITF_SUCCESS;
}


/*
 *  Private read implementation for file source.
 */
NITFPRIV(NITF_BOOL) SegmentReader_read(NITF_DATA * data,
                                       void *buf,
                                    nitf_Off size, nitf_Error * error)
{
    nitf_SegmentReader *reader =  (nitf_SegmentReader*)data;
    assert(reader);
    return nitf_SegmentReader_read(reader, buf, (size_t)size, error);
}


NITFAPI(nitf_SegmentSource *) nitf_SegmentReaderSource_construct
(
    nitf_SegmentReader *reader,
    nitf_Error * error
)
{
    static nitf_IDataSource iSource =
    {
        &SegmentReader_read,
        &SegmentReader_destruct,
        &SegmentReader_getSize,
        &SegmentReader_setSize
    };
    nitf_SegmentSource *segmentSource = NULL;

    segmentSource = (nitf_SegmentSource *) NITF_MALLOC(sizeof(nitf_SegmentSource));
    if (!segmentSource)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }
    segmentSource->data = reader;
    segmentSource->iface = &iSource;
    return segmentSource;
}
