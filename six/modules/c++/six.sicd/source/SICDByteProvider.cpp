#include <utility>

#include <six/NITFWriteControl.h>
#include <six/XMLControlFactory.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/SICDByteProvider.h>

namespace six
{
namespace sicd
{
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

const void* SICDByteProvider::getBytes(const void* imageData,
                                       size_t startRow,
                                       size_t numRows,
                                       size_t& numBytes) const
{
    std::vector<std::pair<const sys::byte*, size_t> > data;

    const size_t imageDataEndRow = startRow + numRows;

    for (size_t seg = 0; seg < mImageSegmentInfo.size(); ++seg)
    {
        // See if we're in this segment
        const size_t segStartRow = mImageSegmentInfo[seg].firstRow;
        const size_t segEndRow = mImageSegmentInfo[seg].endRow();

        const size_t startGlobalRowToWrite = std::max(segStartRow, startRow);
        const size_t endGlobalRowToWrite = std::min(segEndRow, imageDataEndRow);

        if (endGlobalRowToWrite > startGlobalRowToWrite)
        {
            const size_t numRowsToWrite =
                    endGlobalRowToWrite - startGlobalRowToWrite;

            if (startRow <= segStartRow)
            {
                // We have the first row of this image segment, so we're
                // responsible for the image subheader
                if (seg == 0)
                {
                    // For the very first image segment, we're responsible for
                    // the file header too
                    data.push_back(std::make_pair(&mFileHeader[0],
                                                  mFileHeader.size()));
                }

                data.push_back(std::make_pair(&mImageSubheaders[seg][0],
                                              mImageSubheaders[seg].size()));
            }

            // Figure out what offset of 'imageData' we're writing from
            const size_t startLocalRowToWrite =
                    startGlobalRowToWrite - startRow;
            const sys::byte* imageDataPtr =
                    static_cast<const sys::byte*>(imageData) +
                    startLocalRowToWrite * mNumBytesPerRow;

            data.push_back(std::make_pair(imageDataPtr,
                                          numRowsToWrite * mNumBytesPerRow));

            if (seg == mImageSegmentInfo.size() - 1 &&
                segEndRow == imageDataEndRow)
            {
                // When we write out the last row of the last image segment, we
                // tack on the DES
                data.push_back(std::make_pair(&mDesSubheaderAndData[0],
                                              mDesSubheaderAndData.size()));
            }
        }
    }

    if (data.size() == 1)
    {
        // We must not have any headers - we can simply return the input
        // pointer
        numBytes = numRows * mNumBytesPerRow;
        return imageData;
    }
    else
    {
        // Need to copy these to the internal buffer
        mBuffer.clear();

        for (size_t ii = 0; ii < data.size(); ++ii)
        {
            mBuffer.insert(mBuffer.end(),
                           data[ii].first,
                           data[ii].first + data[ii].second);
        }

        numBytes = mBuffer.size();
        return &mBuffer[0];
    }
}
}
}
