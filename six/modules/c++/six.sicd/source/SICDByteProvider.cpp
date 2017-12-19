#include <limits>
#include <sstream>

#include <six/NITFWriteControl.h>
#include <six/XMLControlFactory.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/SICDByteProvider.h>

namespace six
{
namespace sicd
{
SICDByteProvider::NITFBuffer::NITFBuffer() :
    mData(NULL),
    mNumBytes(0)
{
}

SICDByteProvider::NITFBuffer::NITFBuffer(const void* data, size_t numBytes) :
    mData(data),
    mNumBytes(numBytes)
{
}

size_t SICDByteProvider::NITFBufferList::getTotalNumBytes() const
{
    size_t numBytes(0);

    for (size_t ii = 0; ii < mBuffers.size(); ++ii)
    {
        numBytes += mBuffers[ii].mNumBytes;
    }

    return numBytes;
}

size_t SICDByteProvider::NITFBufferList::getNumBlocks(size_t blockSize) const
{
    if (blockSize == 0)
    {
        throw except::Exception(Ctxt("Block size must be positive"));
    }

    return getTotalNumBytes() / blockSize;
}

size_t SICDByteProvider::NITFBufferList::getNumBytesInBlock(
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

const void* SICDByteProvider::NITFBufferList::getBlock(
        size_t blockSize,
        size_t blockIdx,
        std::vector<sys::byte>& scratch,
        size_t& numBytes) const
{
    const size_t startByte = blockIdx * blockSize;
    numBytes = getNumBytesInBlock(blockSize, blockIdx);

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

            const sys::byte* const startPtr =
                    static_cast<const sys::byte*>(buffer.mData) +
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

size_t SICDByteProvider::getNumBytesPerRow(
        const NITFWriteControl& writer) const
{
    const Data& data = *writer.getContainer()->getData(0);
    return data.getNumCols() * data.getNumBytesPerPixel();
}

SICDByteProvider::SICDByteProvider(
        const NITFWriteControl& writer,
        const std::vector<std::string>& schemaPaths) :
    mNumBytesPerRow(getNumBytesPerRow(writer))
{
    writer.getFileLayout(schemaPaths,
                         mFileHeader,
                         mImageSubheaders,
                         mDesSubheaderAndData,
                         mImageSubheaderFileOffsets,
                         mImageSegmentInfo,
                         mDesSubheaderFileOffset,
                         mFileNumBytes);
}

SICDByteProvider::SICDByteProvider(
        const ComplexData& data,
        const std::vector<std::string>& schemaPaths,
        size_t maxProductSize) :
    mNumBytesPerRow(data.getNumCols() * data.getNumBytesPerPixel())
{
    XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator(six::DataType::COMPLEX,
                           new six::XMLControlCreatorT<
                                   six::sicd::ComplexXMLControl>());

    mem::SharedPtr<Container> container(new Container(
            DataType::COMPLEX));

    // The container wants to take ownership of the data
    // To avoid memory problems, we'll just clone it
    container->addData(data.clone());

    NITFWriteControl writer;
    writer.setXMLControlRegistry(&xmlRegistry);

    if (maxProductSize != 0)
    {
        writer.getOptions().setParameter(
                six::NITFWriteControl::OPT_MAX_PRODUCT_SIZE,
                maxProductSize);
    }

    writer.initialize(container);

    // Now we can get the file headers and offsets we want
    writer.getFileLayout(schemaPaths,
                         mFileHeader,
                         mImageSubheaders,
                         mDesSubheaderAndData,
                         mImageSubheaderFileOffsets,
                         mImageSegmentInfo,
                         mDesSubheaderFileOffset,
                         mFileNumBytes);
}

nitf::Off SICDByteProvider::getNumBytes(size_t startRow, size_t numRows) const
{
    nitf::Off numBytes(0);

    const size_t imageDataEndRow = startRow + numRows;

    for (size_t seg = 0; seg < mImageSegmentInfo.size(); ++seg)
    {
        // See if we're in this segment
        const NITFSegmentInfo& imageSegmentInfo(mImageSegmentInfo[seg]);

        size_t startGlobalRowToWrite;
        size_t numRowsToWrite;
        if (imageSegmentInfo.isInRange(startRow, numRows,
                                       startGlobalRowToWrite,
                                       numRowsToWrite))
        {
            const size_t segStartRow = imageSegmentInfo.firstRow;

            if (startRow <= segStartRow)
            {
                // We have the first row of this image segment, so we're
                // responsible for the image subheader
                if (seg == 0)
                {
                    // For the very first image segment, we're responsible for
                    // the file header too
                    numBytes += mFileHeader.size();
                }

                numBytes += mImageSubheaders[seg].size();
            }

            numBytes += numRowsToWrite * mNumBytesPerRow;

            if (seg == mImageSegmentInfo.size() - 1 &&
                imageSegmentInfo.endRow() == imageDataEndRow)
            {
                // When we write out the last row of the last image segment, we
                // tack on the DES
                numBytes += mDesSubheaderAndData.size();
            }
        }
    }

    return numBytes;
}

void SICDByteProvider::getBytes(const void* imageData,
                                size_t startRow,
                                size_t numRows,
                                nitf::Off& fileOffset,
                                NITFBufferList& buffers) const
{
    fileOffset = std::numeric_limits<nitf::Off>::max();
    buffers.clear();

    const size_t imageDataEndRow = startRow + numRows;

    for (size_t seg = 0; seg < mImageSegmentInfo.size(); ++seg)
    {
        // See if we're in this segment
        const NITFSegmentInfo& imageSegmentInfo(mImageSegmentInfo[seg]);

        size_t startGlobalRowToWrite;
        size_t numRowsToWrite;
        if (imageSegmentInfo.isInRange(startRow, numRows,
                                       startGlobalRowToWrite,
                                       numRowsToWrite))
        {
            const size_t segStartRow = imageSegmentInfo.firstRow;

            if (startRow <= segStartRow)
            {
                // We have the first row of this image segment, so we're
                // responsible for the image subheader
                if (seg == 0)
                {
                    // For the very first image segment, we're responsible for
                    // the file header too
                    fileOffset = 0;
                    buffers.pushBack(mFileHeader);
                }

                if (buffers.empty())
                {
                    fileOffset = mImageSubheaderFileOffsets[seg];
                }
                buffers.pushBack(mImageSubheaders[seg]);
            }

            // Figure out what offset of 'imageData' we're writing from
            const size_t startLocalRowToWrite =
                    startGlobalRowToWrite - startRow;
            const sys::byte* imageDataPtr =
                    static_cast<const sys::byte*>(imageData) +
                    startLocalRowToWrite * mNumBytesPerRow;

            if (buffers.empty())
            {
                const size_t rowsInSegmentSkipped =
                        startRow - segStartRow;

                fileOffset = mImageSubheaderFileOffsets[seg] +
                        mImageSubheaders[seg].size() +
                        rowsInSegmentSkipped * mNumBytesPerRow;
            }
            buffers.pushBack(imageDataPtr, numRowsToWrite * mNumBytesPerRow);

            if (seg == mImageSegmentInfo.size() - 1 &&
                imageSegmentInfo.endRow() == imageDataEndRow)
            {
                // When we write out the last row of the last image segment, we
                // tack on the DES
                buffers.pushBack(mDesSubheaderAndData);
            }
        }
    }
}
}
}
