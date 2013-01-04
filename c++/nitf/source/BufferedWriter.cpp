#include <stdio.h>

#include "nitf/BufferedWriter.hpp"

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
    mFile(file, sys::File::WRITE_ONLY, sys::File::CREATE)
{
}

BufferedWriter::BufferedWriter(const std::string& file,
                               char* buffer,
                               size_t size,
                               bool adopt) :
    mBufferSize(size),
    mScopedBuffer(adopt ? buffer : NULL),
    mBuffer(buffer),
    mPosition(0),
    mTotalWritten(0),
    mBlocksWritten(0),
    mPartialBlocks(0),
    mFile(file, sys::File::WRITE_ONLY, sys::File::CREATE)
{
}

void BufferedWriter::flushBuffer()
{
    if (mPosition > 0)
    {
        mFile.writeFrom(mBuffer, mPosition);

        mTotalWritten += mPosition;

        ++mBlocksWritten;

        if (mPosition != mBufferSize)
        {
            ++mPartialBlocks;
        }

        mPosition = 0;
    }
}

void BufferedWriter::readImpl(char* , size_t )
{
    throw except::Exception(
        Ctxt("We cannot do reads on a write-only handle"));
}

void BufferedWriter::writeImpl(const char* buf, size_t size)
{
    size_t from = 0;
    while (size > 0)
    {
        size_t bytes = size;

        if (mPosition == mBufferSize)
        {
            flushBuffer();
        }

        if ((mPosition + size) > mBufferSize)
        {
            // We will be flushing the buffer
            // write as many bytes as we can
            bytes = mBufferSize - mPosition;
        }
        if (bytes)
        {
            // Copy over and subtract bytes from the size left
            memcpy(mBuffer + mPosition, buf + from, bytes);
            size -= bytes;
            mPosition += bytes;
            from += bytes;

        }
    }
}

bool BufferedWriter::canSeekImpl() const
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
    return (mTotalWritten + mPosition);
}

int BufferedWriter::getModeImpl() const
{
    return NITF_ACCESS_WRITEONLY;
}

void BufferedWriter::closeImpl()
{
    flushBuffer();
    mFile.close();
}
}
