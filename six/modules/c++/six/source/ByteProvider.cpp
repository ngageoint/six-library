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
    mNumCols(0),
    mOverallNumRowsPerBlock(0),
    mNumColsPerBlock(0),
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

    initialize(writer, schemaPaths);
}

void ByteProvider::initialize(const NITFWriteControl& writer,
                              const std::vector<std::string>& schemaPaths)
{
    // Now we can get the file headers and offsets we want
    writer.getFileLayout(schemaPaths,
                         mFileHeader,
                         mImageSubheaders,
                         mDesSubheaderAndData,
                         mImageSubheaderFileOffsets,
                         mImageSegmentInfo,
                         mDesSubheaderFileOffset,
                         mFileNumBytes);

    // getFileLayout() is going to ensure this is one vertically stacked image
    // and not 2+ unrelated images with different sizes, so this is a reliable
    // way to get the # of cols and bytes/pixel
    const six::Data* const data = writer.getContainer()->getData(0);
    mNumCols = data->getNumCols();

    const Options& options(writer.getOptions());

    if (options.hasParameter(six::NITFWriteControl::OPT_NUM_ROWS_PER_BLOCK))
    {
        mOverallNumRowsPerBlock = options.getParameter(
                six::NITFWriteControl::OPT_NUM_ROWS_PER_BLOCK);
    }

    size_t numColsWithPad;
    if (options.hasParameter(six::NITFWriteControl::OPT_NUM_COLS_PER_BLOCK))
    {
        mNumColsPerBlock = options.getParameter(
                six::NITFWriteControl::OPT_NUM_COLS_PER_BLOCK);

        numColsWithPad = nitf::ImageSubheader::getActualImageDim(
                mNumCols, mNumColsPerBlock);
    }
    else
    {
        numColsWithPad = mNumCols;
    }

    mNumBytesPerRow = numColsWithPad * data->getNumBytesPerPixel();
    if (mOverallNumRowsPerBlock == 0)
    {
        mNumRowsPerBlock.resize(mImageSegmentInfo.size());
        std::fill(mNumRowsPerBlock.begin(), mNumRowsPerBlock.end(), 0);
    }
    else
    {
        mNumRowsPerBlock = getImageBlocker()->getNumRowsPerBlock();
    }
}

void ByteProvider::checkBlocking(size_t seg,
                                 size_t startGlobalRowToWrite,
                                 size_t numRowsToWrite) const
{
    if (mOverallNumRowsPerBlock != 0)
    {
        const NITFSegmentInfo& imageSegmentInfo(mImageSegmentInfo[seg]);
        const size_t segStartRow = imageSegmentInfo.firstRow;

        // Need to start on a block boundary
        const size_t segStartRowToWrite =
                startGlobalRowToWrite - segStartRow;
        const size_t numRowsPerBlock(mNumRowsPerBlock[seg]);
        if (segStartRowToWrite % numRowsPerBlock != 0)
        {
            std::ostringstream ostr;
            ostr << "Trying to write starting at segment " << seg
                 << "'s row " << segStartRowToWrite
                 << " which is not a multiple of " << numRowsPerBlock;
            throw except::Exception(Ctxt(ostr.str()));
        }

        // Need to end on a block boundary or the end of the segment
        if (numRowsToWrite % numRowsPerBlock != 0 &&
            segStartRowToWrite + numRowsToWrite < imageSegmentInfo.numRows)
        {
            std::ostringstream ostr;
            ostr << "Trying to write " << numRowsToWrite
                 << " rows at global start row " << startGlobalRowToWrite
                 << " starting at segment " << seg
                 << " which is not a multiple of " << numRowsPerBlock
                 << " (segment has start = " << imageSegmentInfo.firstRow
                 << ", num = " << imageSegmentInfo.numRows << ")";
            throw except::Exception(Ctxt(ostr.str()));
        }
    }
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
            checkBlocking(seg, startGlobalRowToWrite, numRowsToWrite);
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

            const size_t numRowsPerBlock(mNumRowsPerBlock[seg]);
            if (numRowsPerBlock != 0 &&
                imageDataEndRow >= imageSegmentInfo.endRow())
            {
                const size_t numLeftovers =
                        imageSegmentInfo.numRows % numRowsPerBlock;
                if (numLeftovers != 0)
                {
                    // We need to finish the block
                    const size_t numPadRows = numRowsPerBlock - numLeftovers;
                    numRowsToWrite += numPadRows;
                }
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

    size_t numPadRowsSoFar(0);

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
            checkBlocking(seg, startGlobalRowToWrite, numRowsToWrite);
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
                    startGlobalRowToWrite - startRow + numPadRowsSoFar;
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

            const size_t numRowsPerBlock(mNumRowsPerBlock[seg]);
            if (numRowsPerBlock != 0 &&
                imageDataEndRow >= imageSegmentInfo.endRow())
            {
                const size_t numLeftovers =
                        imageSegmentInfo.numRows % numRowsPerBlock;
                if (numLeftovers != 0)
                {
                    // We need to finish the block
                    // This is already accounted for in the incoming image
                    const size_t numPadRows = numRowsPerBlock - numLeftovers;
                    numRowsToWrite += numPadRows;
                    numPadRowsSoFar += numPadRows;
                }
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

std::auto_ptr<const nitf::ImageBlocker> ByteProvider::getImageBlocker() const
{
    std::vector<size_t> numRowsPerSegment(mImageSegmentInfo.size());
    for (size_t ii = 0; ii < mImageSegmentInfo.size(); ++ii)
    {
        numRowsPerSegment[ii] = mImageSegmentInfo[ii].numRows;
    }

    std::auto_ptr<const nitf::ImageBlocker> blocker(new nitf::ImageBlocker(
            numRowsPerSegment,
            mNumCols,
            mOverallNumRowsPerBlock,
            mNumColsPerBlock));

    return blocker;
}
}
