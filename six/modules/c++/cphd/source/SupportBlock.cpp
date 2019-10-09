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
#include <limits>
#include <sstream>

#include <sys/Conf.h>
#include <mt/ThreadGroup.h>
#include <mt/ThreadPlanner.h>
#include <except/Exception.h>
#include <io/FileInputStream.h>
#include <cphd/ByteSwap.h>
#include <cphd/SupportBlock.h>
#include <six/Init.h>

namespace cphd
{
const std::string SupportBlock::ALL = six::Init::undefined<std::string>();


SupportBlock::SupportBlock(const std::string& pathname,
                   const cphd::Data& data,
                   sys::Off_T startSupport,
                   sys::Off_T sizeSupport) :
    mInStream(new io::FileInputStream(pathname)),
    mData(data),
    mSupportOffset(startSupport),
    mSupportSize(sizeSupport)
{
    initialize();
}

SupportBlock::SupportBlock(mem::SharedPtr<io::SeekableInputStream> inStream,
                   const cphd::Data& data,
                   sys::Off_T startSupport,
                   sys::Off_T sizeSupport) :
    mInStream(inStream),
    mData(data),
    mSupportOffset(startSupport),
    mSupportSize(sizeSupport)
{
    initialize();
}

void SupportBlock::initialize()
{
    // Trusting data has the right offsets 
    mOffsets = mData.sa_IDMap;
    std::map<std::string,sys::Off_T>::const_iterator it;
    for (it = mOffsets.begin(); it != mOffsets.end(); ++it)
    {
        mOffsets.find(it->first)->second += mSupportOffset;
    }
}

sys::Off_T SupportBlock::getFileOffset(std::string id) const
{
    if (mOffsets.count(id) != 1)
    {
        throw(except::Exception(Ctxt("Invalid support array identifier number")));
    }
    return mOffsets.find(id)->second;
}

void SupportBlock::readImpl(std::string id,
                            void* data)
{
    // Compute the byte offset into this SupportArray in the CPHD file
    // First to the start of the first support array we're going to read
    sys::Off_T inOffset = getFileOffset(id);

    sys::byte* dataPtr = static_cast<sys::byte*>(data);
    // Life is easy - can do a single seek and read
    mInStream->seek(inOffset, io::FileInputStream::START);
    size_t size = mData.getSupportArrayById(id).getSize();
    mInStream->read(dataPtr, size);
}

void SupportBlock::read(std::string id,
                        size_t numThreads,
                        const mem::BufferView<sys::ubyte>& data)
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
    readImpl(id, data.data);

    // Can't change endianness of compressed data right?
    if (!sys::isBigEndianSystem() && mData.getElementSize(id) > 1)
    {
        cphd::byteSwap(data.data, mData.getElementSize(id),
                       mData.getSupportArrayById(id).numRows * 
                       mData.getSupportArrayById(id).numCols,
                       numThreads);
    }
}

void SupportBlock::readAll(size_t numThreads,
             mem::ScopedArray<sys::ubyte>& data)
{
    data.reset(new sys::ubyte[mSupportSize]);
    std::map<std::string,sys::Off_T>::const_iterator it;
    for (it = mData.sa_IDMap.begin(); it != mData.sa_IDMap.end(); ++it)
    {
        const size_t bufSize = mData.getSupportArrayById(it->first).getSize();
        read(it->first, numThreads, mem::BufferView<sys::ubyte>(&data[it->second], bufSize));
    }
}

void SupportBlock::read(std::string id,
                        size_t numThreads,
                        mem::ScopedArray<sys::ubyte>& data)
{
    // Sanity checks
    types::RowCol<size_t> dims;
    const size_t bufSize = mData.getSupportArrayById(id).getSize();
    data.reset(new sys::ubyte[bufSize]);
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
        std::map<std::string,sys::Off_T>::const_iterator it;
        for (it = d.mOffsets.begin(); it != d.mOffsets.end(); ++it)
        {
            os << "[" << it->first << "] mOffsets: " << it->second << "\n";
        }
    }

    return os;
}
}
