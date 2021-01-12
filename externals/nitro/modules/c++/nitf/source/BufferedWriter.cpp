/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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

#include "nitf/BufferedWriter.hpp"

#include <stdio.h>
#include <chrono>

#include "gsl/gsl.h"

namespace nitf
{
BufferedWriter::BufferedWriter(const std::string& file, size_t bufferSize) :
    mBufferSize(bufferSize),
    mScopedBuffer(new char[bufferSize]),
    mBuffer(mScopedBuffer.get()),
    mPosition(0),
    mTotalWritten(0),
    mBlocksWritten(0),
    mPartialBlocks(0),
    mFile(file, sys::File::WRITE_ONLY, sys::File::CREATE | sys::File::TRUNCATE)
{
    if (mBufferSize == 0)
    {
        throw except::Exception(Ctxt(
            "BufferedWriters must have a buffer size greater than zero"));
    }
}

BufferedWriter::BufferedWriter(const std::string& file,
                               char* buffer,
                               size_t size,
                               bool adopt) :
    mBufferSize(size),
    mScopedBuffer(adopt ? buffer : nullptr),
    mBuffer(buffer),
    mPosition(0),
    mTotalWritten(0),
    mBlocksWritten(0),
    mPartialBlocks(0),
    mFile(file, sys::File::WRITE_ONLY, sys::File::CREATE)
{
    if (mBufferSize == 0)
    {
        throw except::Exception(Ctxt(
            "BufferedWriters must have a buffer size greater than zero"));
    }
}

BufferedWriter::~BufferedWriter()
{
    try
    {
        if (mFile.isOpen())
        {
            // If the file is still open, we may not have flushed out our
            // internal buffer.  Note that we can't just call closeImpl()
            // here because it's virtual.  sys::File's destructor will take
            // care of closing the file but wouldn't flush (i.e. fsync()) so
            // we call this too.
            flushBuffer();
            mFile.flush();
        }
    }
    catch (...)
    {
    }
}

void BufferedWriter::flushBuffer()
{
    flushBuffer(mBuffer);
}

void BufferedWriter::flushBuffer(const char* buf)
{
    if (mPosition > 0)
    {
        const auto mPosition_ = gsl::narrow<size_t>(mPosition);
        const auto start = std::chrono::steady_clock::now();
        mFile.writeFrom(buf, mPosition_);
        const auto end = std::chrono::steady_clock::now();
        const std::chrono::duration<double> diff = end - start; // in seconds
        mElapsedTime += diff.count();

        mTotalWritten += mPosition;

        ++mBlocksWritten;

        if (mPosition_ != mBufferSize)
        {
            ++mPartialBlocks;
        }

        mPosition = 0;
    }
}

void BufferedWriter::readImpl(void* , size_t )
{
    throw except::Exception(
        Ctxt("We cannot do reads on a write-only handle"));
}

void BufferedWriter::writeImpl(const void* buf, size_t size)
{
    size_t from = 0;
    const char* bufPtr = static_cast<const char*>(buf);
    while (size > 0)
    {
        size_t bytes = size;

        if ((mPosition + size) > mBufferSize)
        {
            // We will be flushing the buffer
            // write as many bytes as we can
            bytes = mBufferSize - mPosition;
        }

        // copy bytes to internal buffer
        if (bytes < mBufferSize)
        {
            // Copy over and subtract bytes from the size left
            memcpy(mBuffer + mPosition, bufPtr + from, bytes);

            // update counters
            mPosition += bytes;
            size -= bytes;
            from += bytes;

            // check the internal buffer
            if (gsl::narrow<size_t>(mPosition) == mBufferSize)
            {
                flushBuffer();
            }
        }
        // flush using the input buffer directly
        else
        {
            // update mPosition before the flush
            mPosition += bytes;
            flushBuffer(bufPtr + from);

            // update counters
            size -= bytes;
            from += bytes;
        }
    }
}

bool BufferedWriter::canSeekImpl() const noexcept
{
    return true;
}

nitf::Off BufferedWriter::seekImpl(nitf::Off offset, int whence)
{
    // This is very unfortunate, since it creates a partial block
    flushBuffer();

    return mFile.seekTo(offset, whence);
}

nitf::Off BufferedWriter::tellImpl() const
{
    return (mFile.getCurrentOffset() + mPosition);
}

nitf::Off BufferedWriter::getSizeImpl() const
{
    return (mFile.length() + mPosition);
}

int BufferedWriter::getModeImpl() const noexcept
{
    return NITF_ACCESS_WRITEONLY;
}

void BufferedWriter::closeImpl()
{
    // Flush everything first
    // Need to time the call to flush() as well as this is spending time
    // actually flushing the data out to disk (previously the disk may have
    // just cached it)
    flushBuffer();


    const auto start = std::chrono::steady_clock::now();
    mFile.flush();
    const auto end = std::chrono::steady_clock::now();
    const std::chrono::duration<double> diff = end - start; // in seconds
    mElapsedTime += diff.count();

    mFile.close();
}
}

