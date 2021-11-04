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
#include <cphd/SupportBlock.h>

#include <limits>
#include <sstream>
#include <std/memory>

#include <nitf/coda-oss.hpp>
#include <mt/ThreadGroup.h>
#include <mt/ThreadPlanner.h>
#include <except/Exception.h>
#include <io/FileInputStream.h>

#include <six/Init.h>

#include <cphd/ByteSwap.h>
#include <cphd/FileHeader.h>

namespace cphd
{
SupportBlock::SupportBlock(const std::string& pathname,
                           const cphd::Data& data,
                           int64_t startSupport,
                           int64_t sizeSupport) :
    mInStream(std::make_shared<io::FileInputStream>(pathname)),
    mData(data),
    mSupportOffset(startSupport),
    mSupportSize(sizeSupport)
{
    initialize();
}

SupportBlock::SupportBlock(std::shared_ptr<io::SeekableInputStream> inStream,
                           const cphd::Data& data,
                           int64_t startSupport,
                           int64_t sizeSupport) :
    mInStream(inStream),
    mData(data),
    mSupportOffset(startSupport),
    mSupportSize(sizeSupport)
{
    initialize();
}
SupportBlock::SupportBlock(std::shared_ptr<io::SeekableInputStream> inStream,
    const cphd::Data& data, const cphd::FileHeader& fileHeader):
    SupportBlock(inStream, data,
        fileHeader.getSupportBlockByteOffset(), fileHeader.getSupportBlockSize())
{
    initialize();
}

void SupportBlock::initialize()
{
    // Trusting data has the right offsets
    for (auto it = mData.supportArrayMap.begin(); it != mData.supportArrayMap.end(); ++it)
    {
        mOffsets[it->first] = mSupportOffset + it->second.arrayByteOffset;
    }
}

int64_t SupportBlock::getFileOffset(const std::string& id) const
{
    if (mOffsets.count(id) != 1)
    {
        throw except::Exception(Ctxt("Invalid support array identifier number"));
    }
    return mOffsets.find(id)->second;
}

void SupportBlock::read(const std::string& id,
                        size_t numThreads,
                        const mem::BufferView<sys::ubyte>& data) const
{
    const size_t minSize = mData.getSupportArrayById(id).getSize();

    if (data.size < minSize)
    {
        std::ostringstream ostr;
        ostr << "Need at least " << minSize << " bytes but only got "
             << data.size;
        throw except::Exception(Ctxt(ostr.str()));
    }
    // Perform the read
    // Compute the byte offset into this SupportArray in the CPHD file
    // First to the start of the first support array we're going to read
    int64_t inOffset = getFileOffset(id);
    auto dataPtr = data.data;
    mInStream->seek(inOffset, io::FileInputStream::START);
    size_t size = mData.getSupportArrayById(id).getSize();
    mInStream->read(dataPtr, size);

    if ((std::endian::native == std::endian::little) && mData.getElementSize(id) > 1)
    {
        cphd::byteSwap(dataPtr, mData.getElementSize(id),
                       mData.getSupportArrayById(id).numRows *
                       mData.getSupportArrayById(id).numCols,
                       numThreads);
    }
}

void SupportBlock::readAll(size_t numThreads,
                           mem::ScopedArray<sys::ubyte>& data) const
{
    data = std::make_unique<sys::ubyte[]>(mSupportSize);
    for (auto& supportArrayMapPair : mData.supportArrayMap)
    {
        const size_t bufSize = supportArrayMapPair.second.getSize();
        read(supportArrayMapPair.first, numThreads, mem::BufferView<sys::ubyte>(&data[supportArrayMapPair.second.arrayByteOffset], bufSize));
    }
}

void SupportBlock::read(const std::string& id,
                        size_t numThreads,
                        mem::ScopedArray<sys::ubyte>& data) const
{
    const size_t bufSize = mData.getSupportArrayById(id).getSize();
    data = std::make_unique<sys::ubyte[]>(bufSize);
    read(id, numThreads, mem::BufferView<sys::ubyte>(data.get(), bufSize));
}

std::ostream& operator<< (std::ostream& os, const SupportBlock& d)
{
    os << "SupportBlock::\n"
       << "  mData: " << d.mData << "\n"
       << "  mSupportOffset: " << d.mSupportOffset << "\n"
       << "  mSupportSize: " << d.mSupportSize << "\n";

    if (d.mOffsets.empty())
    {
        os << "   mOffsets: (empty)" << "\n";
    }
    else
    {
        for (auto it = d.mOffsets.begin(); it != d.mOffsets.end(); ++it)
        {
            os << "[" << it->first << "] mOffsets: " << it->second << "\n";
        }
    }

    return os;
}
}
