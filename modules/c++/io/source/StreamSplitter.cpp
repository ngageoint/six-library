/* =========================================================================
 * This file is part of io-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * io-c++ is free software; you can redistribute it and/or modify
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

#include <algorithm>
#include <sstream>

#include <io/StreamSplitter.h>
#include <except/Exception.h>
#include <io/InputStream.h>

namespace io
{
StreamSplitter::StreamSplitter(io::InputStream& inputStream,
                               const std::string& delimiter,
                               size_t bufferSize) :
    mDelimiter(delimiter),
    mBufferValidBegin(0),
    mBufferValidEnd(0),
    mNumSubstringsReturned(0),
    mNumBytesReturned(0),
    mNumDelimitersProcessed(0),
    mBufferStorage(bufferSize),
    mBufferCapacity(mBufferStorage.size()),
    mBuffer(mBufferStorage.empty() ? nullptr : &mBufferStorage[0]),
    mInputStream(inputStream),
    mStreamEmpty(false)
{
    if (delimiter.empty())
    {
        throw except::InvalidArgumentException(
                Ctxt("delimiter must be a string with size > 0"));
    }

    if (static_cast<size_t>(mBufferCapacity) < delimiter.size() * 2 + 1)
    {
        std::ostringstream os;
        os << "bufferSize must be >= twice the delimiter size + 1 byte. "
           << "Normally it should be much larger for good performance.";
        throw except::InvalidArgumentException(Ctxt(os));
    }
}

bool StreamSplitter::getNext(std::string& substring)
{
    if (isEnd())
    {
        return false;
    }

    if (mNumDelimitersProcessed > 0)
    {
        // discard the delimiter before the start of the next substring
        mBufferValidBegin += mDelimiter.size();
    }

    size_t substringSize = 0;
    while (true)
    {
        handleStreamRead();

        // search for delimiter in buffer
        for (sys::SSize_T ii = mBufferValidBegin;
             ii < mBufferValidEnd - static_cast<sys::SSize_T>(mDelimiter.size() - 1);
             ++ii)
        {
            if (0 == mDelimiter.compare(0,
                                        mDelimiter.size(),
                                        mBuffer + ii,
                                        mDelimiter.size()))
            {
                // delimiter found starting at buffer position ii
                // append the buffer contents preceding that point to output
                transferBufferSegmentToSubstring(substring, substringSize, ii);
                mNumDelimitersProcessed++;
                mNumSubstringsReturned++;
                mNumBytesReturned += substringSize;
                return true;
            }
        }

        // no delimiter found in buffer
        // append the current buffer contents to output
        const sys::SSize_T segmentEnd = mStreamEmpty ?
                mBufferValidEnd
                :
                mBufferValidEnd - static_cast<sys::SSize_T>(mDelimiter.size() - 1);
        transferBufferSegmentToSubstring(substring, substringSize, segmentEnd);

        // if no bytes remain in stream or buffer, we are done
        if (isEnd())
        {
            mNumSubstringsReturned++;
            mNumBytesReturned += substringSize;
            return true;
        }

        // still have some bytes in stream and/or buffer
    }
}

bool StreamSplitter::isEnd() const
{
    return mStreamEmpty && mBufferValidBegin >= mBufferValidEnd;
}

size_t StreamSplitter::getNumSubstringsReturned() const
{
    return mNumSubstringsReturned;
}

size_t StreamSplitter::getNumBytesReturned() const
{
    return mNumBytesReturned;
}

size_t StreamSplitter::getNumBytesProcessed() const
{
    return getNumBytesReturned() + mNumDelimitersProcessed * mDelimiter.size();
}

void StreamSplitter::transferBufferSegmentToSubstring(
        std::string& substring,
        size_t& substringSize,
        sys::SSize_T bufferSegmentEnd)
{
    const sys::SSize_T segmentSize = bufferSegmentEnd - mBufferValidBegin;
    if (segmentSize >= 0)
    {
        substring.resize(substringSize + segmentSize);
        substring.replace(substringSize,
                          segmentSize,
                          mBuffer + mBufferValidBegin,
                          segmentSize);
        substringSize += segmentSize;
        mBufferValidBegin += segmentSize;
    }
}

void StreamSplitter::handleStreamRead()
{
    if (mBufferValidBegin > mBufferCapacity / 2)
    {
        // first half of buffer is no longer needed, shift the rest
        // down to make space for reading in more
        std::copy(mBuffer + mBufferValidBegin,
                  mBuffer + mBufferValidEnd,
                  mBuffer);
        mBufferValidEnd = mBufferValidEnd - mBufferValidBegin;
        mBufferValidBegin = 0;
    }

    // read more from stream if buffer has space
    if (!mStreamEmpty && (mBufferValidEnd < mBufferCapacity))
    {
        const sys::SSize_T numRead =
                mInputStream.read(mBuffer + mBufferValidEnd,
                                  mBufferCapacity - mBufferValidEnd);
        if (numRead > 0)
        {
            mBufferValidEnd += numRead;
        }
        else
        {
            mStreamEmpty = true;
        }
    }
}
}
