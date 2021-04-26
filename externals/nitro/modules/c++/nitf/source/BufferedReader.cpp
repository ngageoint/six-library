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

#include "nitf/BufferedReader.hpp"

#include <stdio.h>

#include <chrono>

#include "gsl/gsl.h"

namespace nitf
{
BufferedReader::BufferedReader(const std::string& file, size_t bufferSize) :
    mMaxBufferSize(gsl::narrow<nitf::Off>(bufferSize)),
    mScopedBuffer(std::make_unique<char[]>(bufferSize)),
    mBuffer(mScopedBuffer.get()),
    mPosition(0),
    mBufferSize(0),
    mTotalRead(0),
    mBlocksRead(0),
    mPartialBlocks(0),
    mFile(file, sys::File::READ_ONLY, sys::File::EXISTING),
    mFileLen(mFile.length())
{
    if (mMaxBufferSize == 0)
    {
        throw except::Exception(Ctxt(
            "BufferedReaders must have a buffer size greater than zero"));
    }

    //! Start off by reading a block
    readNextBuffer();
}

BufferedReader::BufferedReader(const std::string& file,
                               void* buffer,
                               size_t size,
                               bool adopt) :
    mMaxBufferSize(gsl::narrow<nitf::Off>(size)),
    mScopedBuffer(adopt ? static_cast<char*>(buffer) : nullptr),
    mBuffer(static_cast<char*>(buffer)),
    mPosition(0),
    mBufferSize(0),
    mTotalRead(0),
    mBlocksRead(0),
    mPartialBlocks(0),
    mFile(file, sys::File::READ_ONLY, sys::File::EXISTING),
    mFileLen(mFile.length())
{
    if (mMaxBufferSize == 0)
    {
        throw except::Exception(Ctxt(
            "BufferedReaders must have a buffer size greater than zero"));
    }

    //! Start off by reading a block
    readNextBuffer();
}

void BufferedReader::readNextBuffer()
{
    const int64_t currentOffset = mFile.getCurrentOffset();

    const int64_t endOffsetIfPerformMaxRead =
            currentOffset + gsl::narrow<int64_t>(mMaxBufferSize);

    const nitf::Off bufferSize = (endOffsetIfPerformMaxRead > mFileLen) ?
            mFileLen - currentOffset : mMaxBufferSize;

    const auto start = std::chrono::steady_clock::now();
    mFile.readInto(mBuffer, gsl::narrow<size_t>(bufferSize));
    const auto end = std::chrono::steady_clock::now();
    const std::chrono::duration<double> diff = end - start; // in seconds
    mElapsedTime += diff.count();

    mPosition = 0;
    mBufferSize = bufferSize;
    mTotalRead += bufferSize;
    mBlocksRead += 1;
    if (mMaxBufferSize != bufferSize)
    {
        ++mPartialBlocks;
    }
}

#undef min
inline size_t min(size_t amountLeftToRead, nitf::Off mBufferSize_mPosition_)
{
    const auto mBufferSize_mPosition = gsl::narrow<size_t>(mBufferSize_mPosition_);
    return std::min(amountLeftToRead, mBufferSize_mPosition);
}

void BufferedReader::readImpl(void* buf, size_t size)
{
    //! Ensure there is enough data to read
    if (tell() + gsl::narrow<nitf::Off>(size) > getSize())
    {
        throw except::Exception(Ctxt(
                "Attempting to read past the end of a buffered reader."));
    }

    char* bufPtr = static_cast<char*>(buf);
    size_t amountLeftToRead = size;
    size_t offset = 0;

    while (amountLeftToRead)
    {
        const size_t readSize =
                min(amountLeftToRead, mBufferSize - mPosition);

        memcpy(bufPtr + offset, mBuffer + mPosition, readSize);
        mPosition += readSize;
        offset += readSize;
        amountLeftToRead -= readSize;

        if (mPosition >= mBufferSize)
        {
            readNextBuffer();
        }
    }
}

void BufferedReader::writeImpl(const void* , size_t )
{
    throw except::Exception(
        Ctxt("We cannot do writes on a read-only handle"));
}

bool BufferedReader::canSeekImpl() const
{
    return true;
}

nitf::Off BufferedReader::seekImpl(nitf::Off offset, int whence)
{
    const nitf::Off bufferEnd = mFile.getCurrentOffset();
    const nitf::Off bufferStart = bufferEnd - mBufferSize;

    nitf::Off desiredPos = 0;
    switch (whence)
    {
    case sys::File::FROM_START:
        desiredPos = offset;
        break;
    case sys::File::FROM_CURRENT:
        desiredPos = bufferStart + mPosition + offset;
        break;
    case sys::File::FROM_END:
        desiredPos = mFileLen + offset;
        break;
    default:
        throw except::Exception(Ctxt(
                "Invalid whence " + std::to_string(whence)));
    }

    if (desiredPos >= bufferStart && desiredPos < bufferEnd)
    {
        // We've already read this in - we don't really need to seek in the
        // file
        mPosition = desiredPos - bufferStart;
        return desiredPos;
    }
    else
    {
        // Need to do a legit read
        const int64_t newOffset = mFile.seekTo(offset, whence);
        readNextBuffer();
        return newOffset;
    }
}

nitf::Off BufferedReader::tellImpl() const
{
    return (mFile.getCurrentOffset() - mBufferSize + mPosition);
}

nitf::Off BufferedReader::getSizeImpl() const
{
    return mFileLen;
}

int BufferedReader::getModeImpl() const
{
    return NITF_ACCESS_READONLY;
}

void BufferedReader::closeImpl()
{
    mFile.close();
}
}
