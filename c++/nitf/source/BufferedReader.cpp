#include <stdio.h>

#include <nitf/BufferedReader.hpp>

namespace nitf
{
BufferedReader::BufferedReader(const std::string& file, size_t bufferSize) :
    mBufferSize(bufferSize),
    mScopedBuffer(new char[bufferSize]),
    mBuffer(mScopedBuffer.get()),
    mPosition(0),
    mTotalRead(0),
    mBlocksRead(0),
    mPartialBlocks(0),
    mElapsedTime(0),
    mFile(file, sys::File::READ_ONLY, sys::File::EXISTING)
{
    if (mBufferSize == 0)
    {
        throw except::Exception(Ctxt(
            "BufferedReaders must have a buffer size greater than zero"));
    }

    //! Start off by reading a block
    readNextBuffer();
}

BufferedReader::BufferedReader(const std::string& file,
                               char* buffer,
                               size_t size,
                               bool adopt) :
    mBufferSize(size),
    mScopedBuffer(adopt ? buffer : NULL),
    mBuffer(buffer),
    mPosition(0),
    mTotalRead(0),
    mBlocksRead(0),
    mPartialBlocks(0),
    mElapsedTime(0),
    mFile(file, sys::File::READ_ONLY, sys::File::EXISTING)
{
    if (mBufferSize == 0)
    {
        throw except::Exception(Ctxt(
            "BufferedReaders must have a buffer size greater than zero"));
    }

    //! Start off by reading a block
    readNextBuffer();
}

BufferedReader::~BufferedReader()
{
}

void BufferedReader::readNextBuffer()
{
    const sys::Size_T bufferSize = mFile.getCurrentOffset() +
            static_cast<sys::SSize_T>(mBufferSize) > mFile.length() ?
                    mFile.length() - mFile.getCurrentOffset() :
                    static_cast<sys::SSize_T>(mBufferSize);

    sys::RealTimeStopWatch sw;
    sw.start();
    mFile.readInto(mBuffer, bufferSize);
    mElapsedTime += (sw.stop() / 1000.0);

    mPosition = 0;
    mTotalRead += bufferSize;
    mBlocksRead += 1;
    if (mBufferSize != static_cast<size_t>(bufferSize))
    {
        mPartialBlocks += 1;
    }
}

void BufferedReader::readImpl(void* buf, size_t size)
{
    //! Ensure there is enough data to read
    if (tell() + static_cast<nitf::Off>(size) > getSize())
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
                std::min<size_t>(amountLeftToRead, mBufferSize - mPosition);

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
    const sys::Off_T newOffset = mFile.seekTo(offset, whence);
    readNextBuffer();
    return newOffset;
}

nitf::Off BufferedReader::tellImpl() const
{
    return (mFile.getCurrentOffset() - mBufferSize + mPosition);
}

nitf::Off BufferedReader::getSizeImpl() const
{
    return mFile.length();
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
