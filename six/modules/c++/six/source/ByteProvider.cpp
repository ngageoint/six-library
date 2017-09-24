/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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

#include <limits>
#include <sstream>

#include <six/NITFWriteControl.h>
#include <six/ByteProvider.h>

namespace six
{
ByteProvider::ByteProvider() :
    mNumBytesPerRow(0)
{
}

void ByteProvider::initialize(mem::SharedPtr<Container> container,
                              const XMLControlRegistry& xmlRegistry,
                              const std::vector<std::string>& schemaPaths,
                              size_t maxProductSize,
                              size_t numRowsPerBlock,
                              size_t numColsPerBlock)
{
    if (container->getNumData() != 1)
    {
        throw except::Exception(Ctxt(
                "Expected data of size 1 in container but got " +
                str::toString(container->getNumData())));
    }

    const six::Data* const data = container->getData(0);

    NITFWriteControl writer;
    writer.setXMLControlRegistry(&xmlRegistry);

    if (maxProductSize != 0)
    {
        writer.getOptions().setParameter(
                six::NITFWriteControl::OPT_MAX_PRODUCT_SIZE,
                maxProductSize);
    }

    if (numRowsPerBlock != 0)
    {
        writer.getOptions().setParameter(
                six::NITFWriteControl::OPT_NUM_ROWS_PER_BLOCK,
                numRowsPerBlock);
        numRowsPerBlock = std::min(numRowsPerBlock, data->getNumRows());
    }

    if (numColsPerBlock != 0)
    {
        writer.getOptions().setParameter(
                six::NITFWriteControl::OPT_NUM_COLS_PER_BLOCK,
                numColsPerBlock);
        numColsPerBlock = std::min(numColsPerBlock, data->getNumCols());
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

    const size_t actualNumCols =
            nitf::ImageSubheader::getActualImageDim(data->getNumCols(),
                                                    numColsPerBlock);

    mNumBytesPerRow = actualNumCols * data->getNumBytesPerPixel();
}

nitf::Off ByteProvider::getNumBytes(size_t startRow, size_t numRows) const
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
                // tack on the DES(s)
                numBytes += mDesSubheaderAndData.size();
            }
        }
    }

    return numBytes;
}

void ByteProvider::getBytes(const void* imageData,
                                size_t startRow,
                                size_t numRows,
                                nitf::Off& fileOffset,
                                nitf::NITFBufferList& buffers) const
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
                // tack on the DES(s)
                buffers.pushBack(mDesSubheaderAndData);
            }
        }
    }
}
}
