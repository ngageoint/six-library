#include <limits>

#include <six/NITFWriteControl.h>
#include <six/XMLControlFactory.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/SICDByteProvider.h>

namespace six
{
namespace sicd
{
SICDByteProvider::Buffer::Buffer() :
    mData(NULL),
    mNumBytes(0)
{
}

SICDByteProvider::Buffer::Buffer(const void* data, size_t numBytes) :
    mData(data),
    mNumBytes(numBytes)
{
}

size_t SICDByteProvider::BufferList::getTotalNumBytes() const
{
    size_t numBytes(0);

    for (size_t ii = 0; ii < mBuffers.size(); ++ii)
    {
        numBytes += mBuffers[ii].mNumBytes;
    }

    return numBytes;
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
                                BufferList& buffers) const
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
