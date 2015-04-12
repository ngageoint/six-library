#include "nitf/BufferedWriter.hpp"

using namespace nitf;

BufferedWriter::~BufferedWriter()
{
    if (mOwn && mBuffer)
        delete [] mBuffer;
}

void BufferedWriter::flushBuffer()
{
    if (!mPosition)
        return;

    IOHandle::write(mBuffer, mPosition);

    //fsync(mIOHandle.getHandle());
    mTotalWritten += (nitf::Off)mPosition;

    mBlocksWritten++;

    if (mPosition != mBufferSize)
        mPartialBlocks++;

    mPosition = 0;

}

void BufferedWriter::read(char * buf, size_t size)
{
    throw except::Exception(
        Ctxt("We cannot do reads on a write-only handle")
        );
}

void BufferedWriter::write(const char * buf, size_t size) 
    throw(nitf::NITFException)
{

    size_t from = 0;
    while (size)
    {

        // NITF 02.1 003B F011 2345
        size_t bytes = size;

        if (mPosition == mBufferSize)
            flushBuffer();

        if ((mPosition + size) > mBufferSize)
        {
            // We will be flushing the buffer
            // write as many bytes as we can
            bytes = mBufferSize - mPosition;
        }
        if (bytes)
        {
            // Copy over and subtract bytes from the size left
            memcpy((char*)(mBuffer + mPosition), buf + from, bytes);
            size -= bytes;
            mPosition += bytes;
            from += bytes;

        }
    }
}


nitf::Off BufferedWriter::seek(nitf::Off offset, int whence) 
    throw(nitf::NITFException)
{
    // This is very unfortunate, since it creates a partial block
    flushBuffer();
    
    return IOHandle::seek(offset, whence);
}

nitf::Off BufferedWriter::tell() throw(nitf::NITFException)
{
    return ( (nitf::Off)mPosition + IOHandle::tell());
}

nitf::Off BufferedWriter::getSize() throw(nitf::NITFException)
{
    return mTotalWritten + mPosition;
}

void BufferedWriter::close()
{
    flushBuffer();
    IOHandle::close();
}
