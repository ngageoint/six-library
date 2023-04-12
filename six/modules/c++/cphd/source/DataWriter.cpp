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
void DataWriterLittleEndian::operator()(const void* pData, size_t numElements, size_t elementSize)
{
    const auto data = static_cast<const sys::ubyte*>(pData);

    size_t dataProcessed = 0;
    const size_t dataSize = numElements * elementSize;
    while (dataProcessed < dataSize)
    {
        const size_t dataToProcess =
                std::min(mScratch.size(), dataSize - dataProcessed);

        memcpy(mScratch.data(), data + dataProcessed, dataToProcess);

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
void DataWriterBigEndian::operator()(const void* data, size_t numElements, size_t elementSize)
{
    mStream.write(data, numElements * elementSize);
}

}
