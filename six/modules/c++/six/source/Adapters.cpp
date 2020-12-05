/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#include "six/Adapters.h"

using namespace six;

extern "C"
{
void __six_StreamWriteHandler_destruct(NITF_DATA * data);
NITF_BOOL __six_StreamWriteHandler_write(NITF_DATA * data,
        nitf_IOInterface* io, nitf_Error * error);

void __six_MemoryWriteHandler_destruct(NITF_DATA * data);
NITF_BOOL __six_MemoryWriteHandler_write(NITF_DATA * data,
        nitf_IOInterface* io, nitf_Error * error);
}

typedef struct _MemoryWriteHandlerImpl
{
    const UByte* buffer;
    size_t firstRow;
    size_t numCols;
    size_t numRows;
    size_t numChannels;
    size_t pixelSize;
    int doByteSwap;
} MemoryWriteHandlerImpl;

extern "C" void __six_MemoryWriteHandler_destruct(NITF_DATA * data)
{
    MemoryWriteHandlerImpl *impl = (MemoryWriteHandlerImpl *) data;
    if (impl)
        NITF_FREE(impl);
}

extern "C" NITF_BOOL __six_MemoryWriteHandler_write(NITF_DATA * data,
        nitf_IOInterface* io, nitf_Error * error)
{
    size_t rowSize;
    uint64_t off;
    UByte* rowCopy;
    unsigned int i;

    MemoryWriteHandlerImpl *impl = (MemoryWriteHandlerImpl *) data;

    rowSize = impl->pixelSize * impl->numCols;
    off = impl->firstRow * rowSize;
    rowCopy = new UByte[rowSize];

    for (i = 0; i < impl->numRows; i++)
    {

        memcpy(rowCopy, (const sys::byte*) &impl->buffer[off], rowSize);

        if (impl->doByteSwap)
            sys::byteSwap((sys::byte*) rowCopy, impl->pixelSize
                    / impl->numChannels, impl->numCols * impl->numChannels);
        // And write it back
        if (!nitf_IOInterface_write(io, (const sys::byte*) rowCopy, rowSize,
                                    error))
            goto CATCH_ERROR;
        off += rowSize;
    }
    delete[] rowCopy;
    return NITF_SUCCESS;

    CATCH_ERROR: delete[] rowCopy;
    return NITF_FAILURE;
}

MemoryWriteHandler::MemoryWriteHandler(const NITFSegmentInfo& info,
        const UByte* buffer, size_t firstRow, size_t numCols,
        size_t numChannels, size_t pixelSize, bool doByteSwap)
{
    // Dont do it if we only have a byte!
    if (pixelSize / numChannels == 1)
        doByteSwap = false;

    static nitf_IWriteHandler iWriteHandler =
            { &__six_MemoryWriteHandler_write,
              &__six_MemoryWriteHandler_destruct };

    MemoryWriteHandlerImpl *impl =
            (MemoryWriteHandlerImpl *) NITF_MALLOC(
                    sizeof(MemoryWriteHandlerImpl));
    if (!impl)
        throw nitf::NITFException(Ctxt("Out of memory"));
    impl->buffer = buffer;
    impl->firstRow = firstRow;
    impl->numCols = numCols;
    impl->numRows = info.numRows;
    impl->numChannels = numChannels;
    impl->pixelSize = pixelSize;
    impl->doByteSwap = doByteSwap;

    nitf_SegmentWriter *segmentWriter =
            (nitf_SegmentWriter *) NITF_MALLOC(sizeof(nitf_SegmentWriter));
    if (!segmentWriter)
        throw nitf::NITFException(Ctxt("Out of memory"));
    segmentWriter->data = impl;
    segmentWriter->iface = &iWriteHandler;

    setNative(segmentWriter);
    setManaged(false);
}

//
// StreamWriteHandler
//


typedef struct _StreamWriteHandlerImpl
{
    io::InputStream* inputStream;
    size_t numCols;
    size_t numRows;
    size_t numChannels;
    size_t pixelSize;
    int doByteSwap;
} StreamWriteHandlerImpl;

extern "C" void __six_StreamWriteHandler_destruct(NITF_DATA * data)
{
    StreamWriteHandlerImpl *impl = (StreamWriteHandlerImpl *) data;
    if (impl)
        NITF_FREE(impl);
}

extern "C" NITF_BOOL __six_StreamWriteHandler_write(NITF_DATA * data,
        nitf_IOInterface* io, nitf_Error * error)
{
    size_t rowSize;
    UByte* rowCopy;
    unsigned int i;

    StreamWriteHandlerImpl *impl = (StreamWriteHandlerImpl *) data;

    rowSize = impl->pixelSize * impl->numCols;
    rowCopy = new UByte[rowSize];

    for (i = 0; i < impl->numRows; i++)
    {

        impl->inputStream->read((sys::byte*) rowCopy, rowSize);

        if (impl->doByteSwap)
            sys::byteSwap((sys::byte*) rowCopy, impl->pixelSize
                    / impl->numChannels, impl->numCols * impl->numChannels);

        // And write it back
        if (!nitf_IOInterface_write(io, (const sys::byte*) rowCopy, rowSize,
                                    error))
            goto CATCH_ERROR;
    }
    delete[] rowCopy;
    return NITF_SUCCESS;

    CATCH_ERROR: delete[] rowCopy;
    return NITF_FAILURE;
}

StreamWriteHandler::StreamWriteHandler(const NITFSegmentInfo& info,
        io::InputStream* is, size_t numCols, size_t numChannels,
        size_t pixelSize, bool doByteSwap)
{
    // Don't do it if we only have a byte!
    if ((pixelSize / numChannels) == 1)
        doByteSwap = false;

    static nitf_IWriteHandler iWriteHandler =
            { &__six_StreamWriteHandler_write,
              &__six_StreamWriteHandler_destruct };

    StreamWriteHandlerImpl
            *impl =
                    (StreamWriteHandlerImpl *) NITF_MALLOC(
                                                           sizeof(StreamWriteHandlerImpl));
    if (!impl)
        throw nitf::NITFException(Ctxt("Out of memory"));

    impl->inputStream = is;
    impl->numCols = numCols;
    impl->numRows = info.numRows;
    impl->numChannels = numChannels;
    impl->pixelSize = pixelSize;
    impl->doByteSwap = doByteSwap;

    nitf_SegmentWriter *segmentWriter =
            (nitf_SegmentWriter *) NITF_MALLOC(sizeof(nitf_SegmentWriter));
    if (!segmentWriter)
        throw nitf::NITFException(Ctxt("Out of memory"));

    segmentWriter->data = impl;
    segmentWriter->iface = &iWriteHandler;

    setNative( segmentWriter);
    setManaged(false);
}


