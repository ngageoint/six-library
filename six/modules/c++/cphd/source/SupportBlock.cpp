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
                           const mem::ScopedCopyablePtr<cphd::SupportArray> supportArray,
                           const cphd::Data& data,
                           int64_t startSupport,
                           int64_t sizeSupport) :
    SupportBlock(std::make_shared<io::FileInputStream>(pathname), 
                 supportArray,
                 data, 
                 startSupport, 
                 sizeSupport)
{
}
SupportBlock::SupportBlock(std::shared_ptr<io::SeekableInputStream> inStream,
                           const mem::ScopedCopyablePtr<cphd::SupportArray> supportArray,
                           const cphd::Data& data,
                           int64_t startSupport,
                           int64_t sizeSupport) :
    mInStream(inStream),
    mSupportArray(supportArray),
    mData(data),
    mSupportOffset(startSupport),
    mSupportSize(sizeSupport)
{
    //! Initialize mOffsets for each array both for uncompressed and compressed data
    // Trusting data has the right offsets
    for (auto kv : mData.supportArrayMap)
    {
        mOffsets[kv.first] = mSupportOffset + kv.second.arrayByteOffset;
    }
}
SupportBlock::SupportBlock(std::shared_ptr<io::SeekableInputStream> inStream,
                           const mem::ScopedCopyablePtr<cphd::SupportArray> supportArray,
                           const cphd::Data& data, 
                           const cphd::FileHeader& fileHeader):
    SupportBlock(inStream, 
                 supportArray,
                 data,
                 fileHeader.getSupportBlockByteOffset(), 
                 fileHeader.getSupportBlockSize())
{
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
                        const mem::BufferView<sys::ubyte>& data_) const
{
    void* pData = data_.data;
    auto data = sys::make_span<std::byte>(pData, data_.size);
    read(id, numThreads, data);
}
void SupportBlock::read(const std::string& id, size_t numThreads, std::span<std::byte> data) const
{
    const size_t minSize = mData.getSupportArrayById(id).getSize();

    if (data.size() < minSize)
    {
        std::ostringstream ostr;
        ostr << "Need at least " << minSize << " bytes but only got " << data.size();
        throw except::Exception(Ctxt(ostr));
    }

    // Perform the read
    // Compute the byte offset into this SupportArray in the CPHD file
    // First to the start of the first support array we're going to read
    int64_t inOffset = getFileOffset(id);
    auto dataPtr = data.data();
    mInStream->seek(inOffset, io::FileInputStream::START);
    size_t bytes = mData.getSupportArrayById(id).size_bytes();
    mInStream->read(dataPtr, bytes);

    const size_t elemSize = mData.getElementSize(id);
    if ((std::endian::native == std::endian::little) && elemSize > 1)
    {
        if (mSupportArray.get() == nullptr)
        {
            std::ostringstream ostr;
            ostr << "mSupportArray was not populated";
            throw except::Exception(Ctxt(ostr.str()));
        }

        const size_t bytesPerSwap = mSupportArray->getSupportDataBytesPerSwap(id, elemSize);
        cphd::byteSwap(dataPtr, bytesPerSwap, bytes / bytesPerSwap, numThreads);
    }
}

std::vector<std::byte> SupportBlock::readAll(size_t numThreads) const
{
    std::vector<std::byte> retval(mSupportSize);
    for (auto& supportArrayMapPair : mData.supportArrayMap)
    {
        const size_t bufSize = supportArrayMapPair.second.getSize();
        auto pData = &retval[supportArrayMapPair.second.arrayByteOffset];
        read(supportArrayMapPair.first, numThreads, sys::make_span(pData, bufSize));
    }
    return retval;
}

std::vector<std::byte> SupportBlock::read(const std::string& id, size_t numThreads) const
{
    std::vector<std::byte> retval(mData.getSupportArrayById(id).getSize());
    read(id, numThreads, sys::make_span(retval));
    return retval;
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
