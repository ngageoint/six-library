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

#include "nitf/BandSource.h"

/*
 *  Private implementation struct
 */
typedef struct _MemorySourceImpl
{
    char *data;
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


NITFPRIV(NITF_BOOL) MemorySource_contigRead(MemorySourceImpl *
        memorySource, char *buf,
        nitf_Off size,
        nitf_Error * error)
{
    memcpy(buf, memorySource->data + memorySource->mark, size);
    memorySource->mark += size;
    return NITF_SUCCESS;
}


NITFPRIV(NITF_BOOL) MemorySource_offsetRead(MemorySourceImpl *
        memorySource, char *buf,
        nitf_Off size,
        nitf_Error * error)
{
    int i = 0;
    int j = 0;

    while (i < size)
    {
        for (j = 0; j < memorySource->numBytesPerPixel; ++j, ++i)
        {
            buf[i] = *(memorySource->data + memorySource->mark++);
        }
        memorySource->mark +=
            (memorySource->pixelSkip * memorySource->numBytesPerPixel);
    }
    return NITF_SUCCESS;
}


/*
 *  Private read implementation for memory source.
 */
NITFPRIV(NITF_BOOL) MemorySource_read(NITF_DATA * data,
                                      char *buf,
                                      nitf_Off size, nitf_Error * error)
{
    MemorySourceImpl *memorySource = toMemorySource(data, error);
    if (!memorySource)
        return NITF_FAILURE;

    /*  We like the contiguous read case, its fast  */
    /*  We want to make sure we reward this case    */
    if (memorySource->pixelSkip == 0)
        return MemorySource_contigRead(memorySource, buf, size, error);

    return MemorySource_offsetRead(memorySource, buf, size, error);

}


NITFPRIV(void) MemorySource_destruct(NITF_DATA * data)
{
    MemorySourceImpl *memorySource = (MemorySourceImpl *) data;
    assert(memorySource);
    NITF_FREE(memorySource);
}


NITFPRIV(nitf_Off) MemorySource_getSize(NITF_DATA * data, nitf_Error *e)
{
    MemorySourceImpl *memorySource = (MemorySourceImpl *) data;
    assert(memorySource);
    return memorySource->size;
}

NITFPRIV(NITF_BOOL) MemorySource_setSize(NITF_DATA * data, nitf_Off size, nitf_Error *e)
{
    MemorySourceImpl *memorySource = (MemorySourceImpl *) data;
    assert(memorySource);
    memorySource->size = size;
    return NITF_SUCCESS;
}



NITFAPI(nitf_BandSource *) nitf_MemorySource_construct(char *data,
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
typedef struct _FileSourceImpl
{
    nitf_IOHandle handle;
    nitf_Off start;
    nitf_Off size;
    int numBytesPerPixel;
    int pixelSkip;
    nitf_Off mark;
}
FileSourceImpl;

NITFPRIV(void) FileSource_destruct(NITF_DATA * data)
{
    NITF_FREE(data);
}

NITFPRIV(nitf_Off) FileSource_getSize(NITF_DATA * data, nitf_Error *e)
{
    FileSourceImpl *fileSource = (FileSourceImpl *) data;
    assert(fileSource);
    return (nitf_Off)fileSource->size;
}


NITFPRIV(NITF_BOOL) FileSource_setSize(NITF_DATA * data, nitf_Off size, nitf_Error *e)
{
    FileSourceImpl *fileSource = (FileSourceImpl *) data;
    assert(fileSource);
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
        char *buf,
        nitf_Off size, nitf_Error * error)
{

    if (!NITF_IO_SUCCESS(nitf_IOHandle_read(fileSource->handle,
                                            buf, size, error)))
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
        char *buf,
        nitf_Off size, nitf_Error * error)
{

    /* we do not multiply the pixelSkip by numBytesPerPixel, b/c this
     * read method takes in size as number of bytes, not number of pixels */

    /* TODO - this *could* be smaller, but this should be ok for now */
    nitf_Off tsize = size * (fileSource->pixelSkip + 1);

    char *tbuf;
    nitf_Off lmark = 0;
    int i = 0;
    int j = 0;
    if (tsize + fileSource->mark > fileSource->size)
        tsize = fileSource->size - fileSource->mark;

    tbuf = (char *) NITF_MALLOC(tsize);
    if (!tbuf)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NITF_FAILURE;
    }

    if (!nitf_IOHandle_read(fileSource->handle, tbuf, tsize, error))
    {
        NITF_FREE(tbuf);
        return NITF_FAILURE;
    }
    /*  Downsize for buf */
    while (i < size)
    {
        for (j = 0; j < fileSource->numBytesPerPixel; ++j, ++i)
        {
            buf[i] = *(tbuf + lmark++);
        }
        lmark += (fileSource->pixelSkip * fileSource->numBytesPerPixel);
    }
    fileSource->mark += lmark;
    NITF_FREE(tbuf);
    return NITF_SUCCESS;
}


/*
 *  Private read implementation for file source.
 */
NITFPRIV(NITF_BOOL) FileSource_read(NITF_DATA * data,
                                    char *buf,
                                    nitf_Off size, nitf_Error * error)
{
    FileSourceImpl *fileSource = toFileSource(data, error);
    if (!fileSource)
        return NITF_FAILURE;

    if (!NITF_IO_SUCCESS(nitf_IOHandle_seek(fileSource->handle,
                                            fileSource->mark,
                                            NITF_SEEK_SET, error)))
        return NITF_FAILURE;
    if (fileSource->pixelSkip == 0)
        return FileSource_contigRead(fileSource, buf, size, error);
    return FileSource_offsetRead(fileSource, buf, size, error);
}


NITFAPI(nitf_BandSource *) nitf_FileSource_construct(nitf_IOHandle handle,
        nitf_Off start,
        int numBytesPerPixel,
        int pixelSkip,
        nitf_Error * error)
{
    static nitf_IDataSource iFileSource =
        {
            &FileSource_read,
            &FileSource_destruct,
            &FileSource_getSize,
            &FileSource_setSize
        };
    FileSourceImpl *impl;
    nitf_BandSource *bandSource;

    impl = (FileSourceImpl *) NITF_MALLOC(sizeof(FileSourceImpl));
    if (!impl)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }
    impl->handle = handle;
    impl->numBytesPerPixel = numBytesPerPixel > 0 ? numBytesPerPixel : 1;
    impl->pixelSkip = pixelSkip >= 0 ? pixelSkip : 0;
    impl->mark = impl->start = (start >= 0 ? start : 0);
    impl->size = nitf_IOHandle_getSize(handle, error);

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
    bandSource->iface = &iFileSource;
    return bandSource;
}
