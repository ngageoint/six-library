/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
 *
 * mem-c++ is free software; you can redistribute it and/or modify
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

#include <mem/ScratchMemory.h>

namespace
{
/*!
 * \brief Advance sys::ubyte pointer to next alignment boundary
 */
void align(sys::ubyte*& dataPtr, size_t alignment)
{
    const size_t beyondBoundary = reinterpret_cast<size_t>(dataPtr) % alignment;

    if (beyondBoundary != 0)
    {
        dataPtr += alignment - beyondBoundary;
    }
}
}

namespace mem
{
ScratchMemory::ScratchMemory() :
    mNumBytesNeeded(0),
    mOffset(0)
{
}

ScratchMemory::Segment::Segment(size_t numBytes,
                                size_t numBuffers,
                                size_t alignment,
                                size_t offset) :
    numBytes(numBytes),
    numBuffers(numBuffers),
    alignment(alignment),
    offset(offset)
{
}

void ScratchMemory::release(const std::string& key)
{
    std::map<std::string, Segment>::const_iterator iterSeg = mSegments.find(key);
    mReleasedKeys.insert(key);

    if (mKeyOrder.back() == key)
    {
        const Segment& segment = iterSeg->second;
        mOffset = segment.offset;
    }
    else
    {
        const Segment& segment = iterSeg->second;
        mOffset = segment.offset;

        std::vector<std::string>::iterator keyIter = std::find(mKeyOrder.begin(),
                                                               mKeyOrder.end(),
                                                               key);
        std::vector<std::string>::iterator nextKeyIter = mKeyOrder.erase(keyIter);
        mKeyOrder.push_back(key);

        bool keepGoing = true;
        std::string firstReleasedKey;

        while (keepGoing)
        {
            if (*nextKeyIter == key)
            {
                keepGoing = false;
            }

            //Get data for the segment that will be moved
            std::map<std::string, Segment>::const_iterator mapIter =
                    mSegments.find(*nextKeyIter);
            const Segment& segmentToBeMoved = mapIter->second;

            const size_t numElements = segmentToBeMoved.numBytes;
            const size_t numBuffers = segmentToBeMoved.numBuffers;
            const size_t alignment = segmentToBeMoved.alignment;

            mSegments.erase(*nextKeyIter);
            std::string keyToInsert = *nextKeyIter;
            nextKeyIter = mKeyOrder.erase(nextKeyIter);

            if (mReleasedKeys.find(keyToInsert) != mReleasedKeys.end())
            {
                if (firstReleasedKey.empty())
                {
                    firstReleasedKey = keyToInsert;
                }
            }
            else
            {
                if (!firstReleasedKey.empty())
                {
                    std::map<std::string, Segment>::const_iterator iterSegNew =
                            mSegments.find(firstReleasedKey);
                    const Segment& segmentNew = iterSegNew->second;
                    mOffset = segmentNew.offset;
                }
                firstReleasedKey.clear();
            }
            put<sys::ubyte>(keyToInsert, numElements, numBuffers, alignment);

        }
        std::map<std::string, Segment>::const_iterator iterSegNew =
                mSegments.find(firstReleasedKey);
        const Segment& segmentNew = iterSegNew->second;
        mOffset = segmentNew.offset;
    }
}

void ScratchMemory::setup(const BufferView<sys::ubyte>& scratchBuffer)
{
    if (scratchBuffer.size == 0)
    {
        // allocate the storage internally
        mStorage.resize(mNumBytesNeeded);
        mBuffer = mem::BufferView<sys::ubyte>(mStorage.data(), mStorage.size());
    }
    else
    {
        // use external storage
        if (mNumBytesNeeded > scratchBuffer.size)
        {
            throw except::Exception(Ctxt(
                    "Buffer has insufficient space for scratch memory"));
        }
        if (scratchBuffer.data == NULL)
        {
            throw except::Exception(Ctxt(
                    "Invalid external buffer was provided"));
        }
        mBuffer = scratchBuffer;
    }

    size_t currentOffset = 0;
    for (std::map<std::string, Segment>::iterator iterSeg = mSegments.begin();
         iterSeg != mSegments.end();
         ++iterSeg)
    {
        Segment& segment = iterSeg->second;
        segment.buffers.resize(segment.numBuffers);
        currentOffset = segment.offset;
        for (size_t i = 0; i < segment.numBuffers; ++i)
        {
            segment.buffers[i] = mBuffer.data + currentOffset;
            align(segment.buffers[i], segment.alignment);
            currentOffset = segment.buffers[i] + segment.numBytes -
                    mBuffer.data;
        }
    }
}

const ScratchMemory::Segment& ScratchMemory::lookupSegment(
        const std::string& key,
        size_t indexBuffer) const
{
    if (mBuffer.data == NULL)
    {
        std::ostringstream oss;
        oss << "Tried to get scratch memory for \"" << key
            << "\" before running setup.";
        throw except::Exception(Ctxt(oss.str()));
    }

    std::map<std::string, Segment>::const_iterator iterSeg = mSegments.find(key);
    if (iterSeg == mSegments.end())
    {
        std::ostringstream oss;
        oss << "Scratch memory segment was not found for \"" << key << "\"";
        throw except::Exception(Ctxt(oss.str()));
    }

    const Segment& segment = iterSeg->second;
    if (indexBuffer >= segment.buffers.size())
    {
        std::ostringstream oss;
        oss << "Trying to get buffer index " << indexBuffer << " for \""
            << key << "\", which has only " << segment.buffers.size()
            << " buffers";
        throw except::Exception(Ctxt(oss.str()));
    }
    return segment;
}
}
