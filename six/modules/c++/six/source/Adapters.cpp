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

#include <assert.h>

#include <std/cstddef>
#include <stdexcept>
#include <gsl/gsl.h>
#include <std/memory>

using namespace six;

template<typename TPImpl>
inline TPImpl cast_data(NITF_DATA* data)
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
static inline void nitf_free(NITF_DATA* data)
{
    auto impl = cast_data<TImpl*>(data);
    if (impl)
        NITF_FREE(impl);
}

static inline void byteSwap(std::vector<std::byte>& buffer, size_t elemSize, size_t numElems)
{
    assert(buffer.size() == elemSize * numElems);
    sys::byteSwap(buffer.data(), elemSize, numElems);
}
template<typename TImpl, typename TWriteFunc>
static NITF_BOOL write(const TImpl* impl, nitf_IOInterface* io, nitf_Error* error, TWriteFunc write_f)
{
    const auto rowSize = impl->pixelSize * impl->numCols;
    std::vector<std::byte> rowCopy(rowSize);

    for (unsigned int i = 0; i < impl->numRows; i++)
    {
        write_f(rowCopy);

        if (impl->doByteSwap)
            byteSwap(rowCopy, impl->pixelSize
                / impl->numChannels, impl->numCols * impl->numChannels);

        // And write it back
        if (!nitf_IOInterface_write(io, rowCopy.data(), rowSize, error))
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

    const auto write_f = [&](std::vector<std::byte>& rowCopy_)
    {
        assert(rowCopy_.size() == rowSize);
        auto rowCopy = rowCopy_.data();

        memcpy(rowCopy, &impl->buffer[off], rowCopy_.size());

        off += rowSize;
    };
    return write(impl, io, error, write_f);
}

static nitf_SegmentWriter* create_SegmentWriter(const NITFSegmentInfo& info,
    const UByte* buffer, size_t firstRow, size_t numCols, size_t numChannels, size_t pixelSize, bool doByteSwap,
    nitf_IWriteHandler& iWriteHandler)
{
    auto impl = nitf_malloc<MemoryWriteHandlerImpl>();
    impl->buffer = buffer;
    impl->firstRow = firstRow;
    impl->numCols = numCols;
    impl->numRows = info.getNumRows();
    impl->numChannels = numChannels;
    impl->pixelSize = pixelSize;
    impl->doByteSwap = doByteSwap;

    return create_SegmentWriter(impl, iWriteHandler);
}

MemoryWriteHandler::MemoryWriteHandler(const NITFSegmentInfo& info,
        const UByte* buffer, size_t firstRow, size_t numCols,
        size_t numChannels, size_t pixelSize, bool doByteSwap)
{
    // Dont do it if we only have a byte!
    if (pixelSize / numChannels == 1)
        doByteSwap = false;

    static nitf_IWriteHandler iWriteHandler = { &six_MemoryWriteHandler_write, &six_MemoryWriteHandler_destruct };
    auto segmentWriter = create_SegmentWriter(info, buffer, firstRow, numCols, numChannels, pixelSize, doByteSwap, iWriteHandler);
    setNative(segmentWriter);

    setManaged(false);
}

inline size_t getBandSize(const NITFSegmentInfo& segmentInfo, const Data& data)
{
    const auto pixelSize = data.getNumBytesPerPixel() / data.getNumChannels();
    const auto numCols = data.getNumCols();
    const auto bandSize = pixelSize * numCols * segmentInfo.getNumRows();
    return bandSize;
}

template<typename T>
inline void validate_bandSize(std::span<T> buffer, const NITFSegmentInfo& info, const Data& data)
{
    const auto bandSize = getBandSize(info, data);
    const auto size_in_bytes = bandSize * data.getNumChannels();
    if (buffer.size() * sizeof(buffer[0]) != size_in_bytes)
    {
        // This is not always correct: the sizes are computed using the values in NITFSegmentInfo/Data
        // but we may be working with other data.  This is the case when we have six::zfloat
        // data that will be converted to AMP8I_PHS8I when written to disk.
        //throw std::invalid_argument("buffer.size()!");    }
    }
}

template<typename T>
inline void validate_buffer(std::span<T> buffer, const NITFSegmentInfo& info, const Data& data)
{
    const auto numChannels = data.getNumChannels();
    const auto pixelSize = data.getNumBytesPerPixel() / numChannels;
    if (sizeof(buffer[0]) != pixelSize * numChannels)
    {
        throw std::invalid_argument("pixelSize is wrong.");
    }
    validate_bandSize(buffer, info, data);
}

struct NewMemoryWriteHandler::Impl final
{
    // This needs to persist beyhond the constructor
    std::vector<std::pair<uint8_t, uint8_t>> ampi8i_phs8i;

    void convertPixels(NewMemoryWriteHandler& instance, const NITFSegmentInfo& info, std::span<const std::complex<float>> buffer, const Data& data)
    {
        ampi8i_phs8i.resize(buffer.size());
        const std::span<std::pair<uint8_t, uint8_t>> ampi8i_phs8i_(ampi8i_phs8i.data(), ampi8i_phs8i.size());
        if (!data.convertPixels(buffer, ampi8i_phs8i_))
        {
            throw std::runtime_error("Unable to convert pixels.");
        }
        validate_buffer(ampi8i_phs8i_, info, data);

        // Everything is kosher, point to the converted data
        void* pHandleData = instance.mHandle->get()->data;
        auto pImpl = static_cast<MemoryWriteHandlerImpl*>(pHandleData);
        const void* pData = ampi8i_phs8i.data();
        pImpl->buffer = static_cast<const UByte*>(pData);
    }
};

NewMemoryWriteHandler::NewMemoryWriteHandler(const NITFSegmentInfo& info,
    const std::byte* buffer_, size_t firstRow, const Data& data, bool doByteSwap)
    : m_pImpl(std::make_unique<Impl>())
{
    const auto numCols = data.getNumCols();
    const auto numChannels = data.getNumChannels();
    const auto pixelSize = data.getNumBytesPerPixel();

    // Dont do it if we only have a byte!
    if (pixelSize / numChannels == 1)
        doByteSwap = false;

    static nitf_IWriteHandler iWriteHandler = { &six_MemoryWriteHandler_write, &six_MemoryWriteHandler_destruct };
    const void* pBuffer = buffer_;
    const auto buffer = static_cast<const UByte*>(pBuffer);
    auto segmentWriter = create_SegmentWriter(info, buffer, firstRow, numCols, numChannels, pixelSize, doByteSwap, iWriteHandler);
    setNative(segmentWriter);

    setManaged(false);
}
NewMemoryWriteHandler::~NewMemoryWriteHandler() = default;

template<typename T>
inline const std::byte* cast(std::span<const T> buffer)
{
    const void* pBuffer = buffer.data();
    return static_cast<const std::byte*>(pBuffer);
}

NewMemoryWriteHandler::NewMemoryWriteHandler(const NITFSegmentInfo& info,
    std::span<const std::byte> buffer, size_t firstRow, const Data& data, bool doByteSwap)
    : NewMemoryWriteHandler(info, cast(buffer), firstRow, data, doByteSwap)
{
    validate_bandSize(buffer, info, data);

    if (data.getPixelType() == six::PixelType::AMP8I_PHS8I)
    {
        // Assume that buffer is really six::zfloat.  If it is something else
        // (e.g., std::pair<uint8_t, uint8_t> -- already converted) a different
        // overload should be used.  Since we've lost the actual buffer type,
        // there not much else to do except hope for the best.
        const void* pBuffer_ = buffer.data();
        const auto pBuffer = static_cast<const std::complex<float>*>(pBuffer_);
        const std::span<const std::complex<float>> buffer_(pBuffer, buffer.size() / sizeof(std::complex<float>));
        m_pImpl->convertPixels(*this, info, buffer_, data);
    }
}

NewMemoryWriteHandler::NewMemoryWriteHandler(const NITFSegmentInfo& info,
    std::span<const std::complex<float>> buffer, size_t firstRow, const Data& data, bool doByteSwap)
    : NewMemoryWriteHandler(info, cast(buffer), firstRow, data, doByteSwap)
{
    if (data.getPixelType() == six::PixelType::AMP8I_PHS8I)
    {
        m_pImpl->convertPixels(*this, info, buffer, data);
    }
    else if (data.getPixelType() != six::PixelType::RE32F_IM32F)
    {
        throw std::invalid_argument("pixelType is wrong.");
    }
    else
    {
        validate_buffer(buffer, info, data);
    }
}

NewMemoryWriteHandler::NewMemoryWriteHandler(const NITFSegmentInfo& info,
    std::span<const std::pair<uint8_t, uint8_t>> buffer, size_t firstRow, const Data& data, bool doByteSwap)
    : NewMemoryWriteHandler(info, cast(buffer), firstRow, data, doByteSwap)
{
    // This is for the uncommon case where the data is already in this format; normally, it is six::zfloat.
    if (data.getPixelType() != six::PixelType::AMP8I_PHS8I)
    {
        throw std::invalid_argument("pixelType is wrong.");
    }
    validate_buffer(buffer, info, data);
}

NewMemoryWriteHandler::NewMemoryWriteHandler(const NITFSegmentInfo& info,
    std::span<const std::complex<short>> buffer, size_t firstRow, const Data& data, bool doByteSwap)
    : NewMemoryWriteHandler(info, cast(buffer), firstRow, data, doByteSwap)
{
    // Each pixel is stored as a pair of numbers that represent the real and imaginary 
    // components. Each component is stored in a 16-bit signed integer in 2’s 
    // complement format (2 bytes per component, 4 bytes per pixel). 
    if (data.getPixelType() != six::PixelType::RE16I_IM16I)
    {
        throw std::invalid_argument("pixelType is wrong.");
    }   
    validate_buffer(buffer, info, data);
}
NewMemoryWriteHandler::NewMemoryWriteHandler(const NITFSegmentInfo& info,
    std::span<const uint8_t> buffer, size_t firstRow, const Data& data, bool doByteSwap)
    : NewMemoryWriteHandler(info, cast(buffer), firstRow, data, doByteSwap)
{
    switch (data.getPixelType())
    {
    case six::PixelType::MONO8I:
    case six::PixelType::MONO8LU:
    case six::PixelType::RGB8LU:
    case six::PixelType::RGB24I: // three 8-bit pixels = 24
        break;
    default:
        throw std::invalid_argument("pixelType is wrong.");
    }
    validate_buffer(buffer, info, data);
}
NewMemoryWriteHandler::NewMemoryWriteHandler(const NITFSegmentInfo& info,
    std::span<const uint16_t> buffer, size_t firstRow, const Data& data, bool doByteSwap)
    : NewMemoryWriteHandler(info, cast(buffer), firstRow, data, doByteSwap)
{
    if (data.getPixelType() != six::PixelType::MONO16I)
    {
        throw std::invalid_argument("pixelType is wrong.");
    }
    validate_buffer(buffer, info, data);
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
    const auto write_f = [&](std::vector<std::byte>& rowCopy)
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

//! TODO: This section of code (unlike the memory section above)
//        does not account for blocked writing or J2K compression.
//        CODA ticket #443 will update support for this.
StreamWriteHandler::StreamWriteHandler(const NITFSegmentInfo& info,
        io::InputStream* is, const Data& data, bool doByteSwap)
    : StreamWriteHandler(info, is,
        data.getNumCols(), data.getNumChannels(), data.getNumBytesPerPixel() / data.getNumChannels(),
        doByteSwap)
{
}
