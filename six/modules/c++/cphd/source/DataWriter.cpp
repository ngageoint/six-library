/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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
#include <cphd/DataWriter.h>

#include <thread>
#include <std/memory>

#include <except/Exception.h>
#include <sys/Span.h>

#include <cphd/ByteSwap.h>
#undef min
#undef max


namespace cphd
{
DataWriter::DataWriter(io::OutputStream& stream, size_t numThreads) :
    mStream(stream),
    mNumThreads(numThreads == 0 ? std::thread::hardware_concurrency() : numThreads)
{
}

DataWriterLittleEndian::DataWriterLittleEndian(
        io::OutputStream& stream,
        size_t numThreads,
        size_t scratchSize) :
    DataWriter(stream, numThreads),
    mScratch(scratchSize)
{
}
DataWriterLittleEndian::DataWriterLittleEndian(
	std::shared_ptr<io::SeekableOutputStream>& stream,
        size_t numThreads,
        size_t scratchSize) : DataWriterLittleEndian(*stream, numThreads, scratchSize)
{
}

static auto adjust_span(std::span<const std::byte> data, size_t dataProcessed)
{
    const auto pData = data.data() + dataProcessed;
    const auto size = data.size() - dataProcessed;
    return sys::make_span(pData, size);
}

void DataWriterLittleEndian::operator()(std::span<const std::byte> pData, size_t elementSize)
{
    size_t dataProcessed = 0;
    const auto dataSize = pData.size();
    while (dataProcessed < dataSize)
    {
        // `capacity()`, not `size()`; https://en.cppreference.com/w/cpp/container/vector/capacity
        // "Returns the number of elements that the container has currently allocated space for."
        // `assign()` (below) will alter `size()` to the number of current elements.
        const size_t dataToProcess =
                std::min(mScratch.capacity(), dataSize - dataProcessed);

        const auto data = adjust_span(pData, dataProcessed);
        const auto begin = data.begin();
        const auto end = begin + dataToProcess;
        mScratch.assign(begin, end); // see above; changes `size()`.

        cphd::byteSwap(mScratch.data(),
                       elementSize,
                       dataToProcess / elementSize,
                       mNumThreads);

        mStream.write(mScratch.data(), dataToProcess);

        dataProcessed += dataToProcess;
    }
}

DataWriterBigEndian::DataWriterBigEndian(io::OutputStream& stream, size_t numThreads) :
    DataWriter(stream, numThreads)
{
}
DataWriterBigEndian::DataWriterBigEndian(
	std::shared_ptr<io::SeekableOutputStream>& stream,
        size_t numThreads) : DataWriterBigEndian(*stream, numThreads)
{
}
void DataWriterBigEndian::operator()(std::span<const std::byte> pData, size_t /*elementSize*/)
{
    mStream.write(pData.data(), pData.size());
}

}
