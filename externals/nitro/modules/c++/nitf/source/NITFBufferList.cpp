/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

#include <stdlib.h>

#include <sys/Conf.h>
#include <except/Exception.h>
#include <nitf/NITFBufferList.hpp>

namespace nitf
{
NITFBuffer::NITFBuffer() :
    mData(NULL),
    mNumBytes(0)
{
}

NITFBuffer::NITFBuffer(const void* data, size_t numBytes) :
    mData(data),
    mNumBytes(numBytes)
{
}

size_t NITFBufferList::getTotalNumBytes() const
{
    size_t numBytes(0);

    for (size_t ii = 0; ii < mBuffers.size(); ++ii)
    {
        numBytes += mBuffers[ii].mNumBytes;
    }

    return numBytes;
}

size_t NITFBufferList::getNumBlocks(size_t blockSize) const
{
    if (blockSize == 0)
    {
        throw except::Exception(Ctxt("Block size must be positive"));
    }

    return getTotalNumBytes() / blockSize;
}

size_t NITFBufferList::getNumBytesInBlock(
        size_t blockSize,
        size_t blockIdx) const
{
    const size_t numBlocks(getNumBlocks(blockSize));
    if (blockIdx >= numBlocks)
    {
        std::ostringstream ostr;
        ostr << "Block index " << blockIdx << " is out of bounds - only "
             << numBlocks << " blocks with a block size of " << blockSize;
        throw except::Exception(Ctxt(ostr.str()));
    }

    const size_t numBytes = (blockIdx == numBlocks - 1) ?
            getTotalNumBytes() - (numBlocks - 1) * blockSize :
            blockSize;

    return numBytes;
}

template<typename TScratch>
const void* getBlock(const NITFBufferList& rThis, size_t blockSize,
                                     size_t blockIdx,
                                     std::vector<TScratch>& scratch,
                                     size_t& numBytes)
{
    auto& mBuffers = rThis.mBuffers;

    const size_t startByte = blockIdx * blockSize;
    numBytes = rThis.getNumBytesInBlock(blockSize, blockIdx);

    size_t byteCount(0);
    for (size_t ii = 0; ii < mBuffers.size(); ++ii)
    {
        const NITFBuffer& buffer(mBuffers[ii]);
        if (byteCount + buffer.mNumBytes >= startByte)
        {
            // We found our first buffer
            const size_t numBytesToSkip = startByte - byteCount;
            const size_t numBytesLeftInBuffer =
                    buffer.mNumBytes - numBytesToSkip;

            auto const startPtr =
                    static_cast<const TScratch*>(buffer.mData) +
                    numBytesToSkip;
            if (numBytesLeftInBuffer >= numBytes)
            {
                // We have contiguous memory in this buffer - we don't need to
                // copy anything
                return startPtr;
            }
            else
            {
                // The bytes we want span 2+ buffers - we'll use scratch space
                // and copy in the bytes we want to that
                scratch.resize(numBytes);
                size_t numBytesCopied(0);
                memcpy(&scratch[0], startPtr, numBytesLeftInBuffer);
                numBytesCopied += numBytesLeftInBuffer;

                for (size_t jj = ii + 1; jj < mBuffers.size(); ++jj)
                {
                    const NITFBuffer& curBuffer(mBuffers[jj]);
                    const size_t numBytesToCopy =
                            std::min(curBuffer.mNumBytes,
                                     numBytes - numBytesCopied);

                    memcpy(&scratch[numBytesCopied],
                           curBuffer.mData,
                           numBytesToCopy);
                    numBytesCopied += numBytesToCopy;
                    if (numBytesCopied == numBytes)
                    {
                        break;
                    }
                }

                return &scratch[0];
            }
        }

        byteCount += buffer.mNumBytes;
    }

    // Should not be possible to get here
    return NULL;
}
const void* NITFBufferList::getBlock(size_t blockSize,
    size_t blockIdx,
    std::vector<sys::byte>& scratch,
    size_t& numBytes) const
{
    return nitf::getBlock(*this, blockSize, blockIdx, scratch, numBytes);
}
const void* NITFBufferList::getBlock(size_t blockSize,
    size_t blockIdx,
    std::vector<std::byte>& scratch,
    size_t& numBytes) const
{
    return nitf::getBlock(*this, blockSize, blockIdx, scratch, numBytes);
}
}
