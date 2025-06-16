/* =========================================================================
 * This file is part of io-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include "io/FileInputStreamOS.h"

#include "mt/ThreadGroup.h"
#include "mt/ThreadPlanner.h"

#if !defined(USE_IO_STREAMS)

namespace
{
class ChunkReadRunnable : public sys::Runnable
{
public:
    ChunkReadRunnable(sys::File& file,
                      size_t offset,
                      size_t len,
                      void* buffer) :
        mFile(file), mOffset(offset), mLen(len), mBuffer(buffer)
    {
    }

    void run() override
    {
        // No need to clear buffer because the readInto call will write every
        // byte
        //::memset(mBuffer, 0, mLen);
        mFile.readAtInto(mOffset, mBuffer, mLen);
    }

private:
    sys::File& mFile;
    size_t mOffset;
    size_t mLen;
    void* mBuffer;
};
}

/*!
 * Returns the number of bytes that can be read
 * without blocking by the next caller of a method for this input
 *
 * \throw except::IOException
 * \return number of bytes which are readable
 *
 */
sys::Off_T io::FileInputStreamOS::available()
{
    sys::Off_T where = mFile.getCurrentOffset();
    mFile.seekTo(0, sys::File::FROM_END);
    sys::Off_T until = mFile.getCurrentOffset();
    mFile.seekTo(where, sys::File::FROM_START);

    return until - where;
}


sys::SSize_T io::FileInputStreamOS::readImpl(void* buffer, size_t len)
{
    sys::Off_T avail = available();
    sys::byte* bufferPtr = static_cast<sys::byte*>(buffer);
    if (!avail)
    {
        // Clear the buffer to preserve existing (undocumented) behavior
        ::memset(buffer, 0, len);
        return io::InputStream::IS_EOF;
    }
    if (len > static_cast<sys::Size_T>(avail))
    {
        // Clear just the tail end of the buffer to preserve existing
        // (undocumented) behavior
        ::memset(bufferPtr + avail, 0, len - avail);
        len = static_cast<sys::Size_T>(avail);
    }

    if (mMaxReadThreads <= 1 ||
        len <= mParallelChunkSize * mMinChunksForThreading)
    {
        // No need to clear buffer because the readInto call will write every
        // byte
        //::memset(buffer, 0, len);
        mFile.readInto(buffer, len);
        return static_cast<sys::SSize_T>(len);
    }

    size_t baseLocation = tell();

    size_t chunks = len / mParallelChunkSize;
    const mt::ThreadPlanner planner(chunks, mMaxReadThreads);
    mt::ThreadGroup threadGroup;

    size_t threadNum(0);
    size_t threadOffset;
    size_t threadNumChunks;
    while (planner.getThreadInfo(threadNum++, threadOffset, threadNumChunks))
    {
        size_t bufferOffset = threadOffset * mParallelChunkSize;
        threadGroup.createThread(
                new ChunkReadRunnable(mFile,
                                      baseLocation + bufferOffset,
                                      threadNumChunks * mParallelChunkSize,
                                      bufferPtr + bufferOffset));
    }

    threadGroup.joinAll();

    size_t threadedRead = chunks * mParallelChunkSize;
    seek(baseLocation + threadedRead, START);
    mFile.readInto(bufferPtr + threadedRead, len - threadedRead);
    return static_cast<sys::SSize_T>(len);
}

#endif
