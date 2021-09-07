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

#include <assert.h>

#include "six/Adapters.h"

#include <gsl/gsl.h>

using namespace six;

template<typename TPImpl>
TPImpl cast_data(NITF_DATA* data)
{
    void* data_ = data;
    return static_cast<TPImpl>(data_);
}

template<typename TImpl>
static TImpl* nitf_malloc()
{
    auto impl = static_cast<TImpl*>(NITF_MALLOC(sizeof(TImpl)));
    if (!impl)
        throw nitf::NITFException(Ctxt("Out of memory"));
    return impl;
}

template<typename TImpl>
static void nitf_free(NITF_DATA* data)
{
    auto impl = cast_data<TImpl*>(data);
    if (impl)
        NITF_FREE(impl);
}

static inline void byteSwap(std::vector<std::byte>& buffer, size_t elemSize, size_t numElems)
{
    assert(buffer.size() == elemSize * numElems);
    sys::byteSwap(buffer.data(), gsl::narrow<unsigned short>(elemSize), numElems);
}
template<typename TImpl, typename TWriteFunc>
static NITF_BOOL write(const TImpl* impl, nitf_IOInterface* io, nitf_Error* error, TWriteFunc write_f)
{
    const auto rowSize = impl->pixelSize * impl->numCols;
    std::vector<std::byte> rowCopy(rowSize);

    for (unsigned int i = 0; i < impl->numRows; i++)
    {
        write_f(impl, rowCopy);

        if (impl->doByteSwap)
            byteSwap(rowCopy, impl->pixelSize
                / impl->numChannels, impl->numCols * impl->numChannels);

        // And write it back
        auto const rowCopy_ = rowCopy.data();
        if (!nitf_IOInterface_write(io, rowCopy_, rowSize, error))
            return NITF_FAILURE;
    }
    return NITF_SUCCESS;
}

template<typename TPImpl>
static nitf_SegmentWriter* create_SegmentWriter(TPImpl impl, nitf_IWriteHandler& iWriteHandler)
{
    auto segmentWriter = nitf_malloc<nitf_SegmentWriter>();
    segmentWriter->data = impl;
    segmentWriter->iface = &iWriteHandler;
    return segmentWriter;
}

struct MemoryWriteHandlerImpl final
{
    const UByte* buffer;
    size_t firstRow;
    types::RowCol<size_t> extent;
    size_t numCols;
    size_t numRows;
    size_t numChannels;
    size_t pixelSize;
    int doByteSwap;
};

static void six_MemoryWriteHandler_destruct(NITF_DATA * data)
{
    nitf_free<MemoryWriteHandlerImpl>(data);
}

static NITF_BOOL six_MemoryWriteHandler_write(NITF_DATA * data,
        nitf_IOInterface* io, nitf_Error * error)
{
    auto const impl = cast_data<const MemoryWriteHandlerImpl*>(data);

    const auto rowSize = impl->pixelSize * impl->numCols;
    uint64_t off = impl->firstRow * rowSize;

    const auto write_f = [&](const MemoryWriteHandlerImpl* impl, std::vector<std::byte>& rowCopy_)
    {
        assert(rowCopy_.size() == rowSize);
        auto rowCopy = rowCopy_.data();

        memcpy(rowCopy, &impl->buffer[off], rowCopy_.size());

        off += rowSize;
    };
    return write(impl, io, error, write_f);
}

MemoryWriteHandler::MemoryWriteHandler(const NITFSegmentInfo& info,
        const UByte* buffer, size_t firstRow, size_t numCols,
        size_t numChannels, size_t pixelSize, bool doByteSwap)
{
    // Dont do it if we only have a byte!
    if (pixelSize / numChannels == 1)
        doByteSwap = false;

    static nitf_IWriteHandler iWriteHandler = { &six_MemoryWriteHandler_write, &six_MemoryWriteHandler_destruct };

    auto impl = nitf_malloc<MemoryWriteHandlerImpl>();
    impl->buffer = buffer;
    impl->firstRow = firstRow;
    impl->numCols = numCols;
    impl->numRows = info.getNumRows();
    impl->numChannels = numChannels;
    impl->pixelSize = pixelSize;
    impl->doByteSwap = doByteSwap;

    auto segmentWriter = create_SegmentWriter(impl, iWriteHandler);
    setNative(segmentWriter);

    setManaged(false);
}
MemoryWriteHandler::MemoryWriteHandler(const NITFSegmentInfo& info,
    const std::byte* buffer, size_t firstRow, size_t numCols,
    size_t numChannels, size_t pixelSize, bool doByteSwap)
    : MemoryWriteHandler(info, static_cast<const UByte*>(static_cast<const void*>(buffer)), firstRow, numCols,
        numChannels, pixelSize, doByteSwap)
{
}


//
// StreamWriteHandler
//
struct StreamWriteHandlerImpl final
{
    io::InputStream* inputStream;
    size_t numCols;
    size_t numRows;
    size_t numChannels;
    size_t pixelSize;
    int doByteSwap;
};

static void six_StreamWriteHandler_destruct(NITF_DATA * data)
{
    nitf_free<StreamWriteHandlerImpl>(data);
}

static NITF_BOOL six_StreamWriteHandler_write(NITF_DATA * data,
        nitf_IOInterface* io, nitf_Error * error)
{
    auto const impl = cast_data<const StreamWriteHandlerImpl*>(data);
    const auto write_f = [](const StreamWriteHandlerImpl* impl, std::vector<std::byte>& rowCopy)
    {
      (void) impl->inputStream->read(rowCopy.data(), rowCopy.size());
    };
    return write(impl, io, error, write_f);
}

StreamWriteHandler::StreamWriteHandler(const NITFSegmentInfo& info,
        io::InputStream* is, size_t numCols, size_t numChannels,
        size_t pixelSize, bool doByteSwap)
{
    // Don't do it if we only have a byte!
    if ((pixelSize / numChannels) == 1)
        doByteSwap = false;

    static nitf_IWriteHandler iWriteHandler = { &six_StreamWriteHandler_write, &six_StreamWriteHandler_destruct };

    auto impl = nitf_malloc<StreamWriteHandlerImpl>();
    impl->inputStream = is;
    impl->numCols = numCols;
    impl->numRows = info.getNumRows();
    impl->numChannels = numChannels;
    impl->pixelSize = pixelSize;
    impl->doByteSwap = doByteSwap;

    auto segmentWriter = create_SegmentWriter(impl, iWriteHandler);
    setNative(segmentWriter);

    setManaged(false);
}
