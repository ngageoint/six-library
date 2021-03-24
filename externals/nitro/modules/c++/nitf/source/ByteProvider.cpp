/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
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

#include "nitf/ByteProvider.hpp"

#include <string.h>
#include <sstream>
#include <algorithm>
#include <limits>

#include <except/Exception.h>
#include <nitf/Writer.hpp>
#include <nitf/IOStreamWriter.hpp>
#include <io/ByteStream.h>

#include "gsl/gsl.h"

#undef min
#undef max

namespace nitf
{

ByteProvider::ByteProvider(Record& record,
                           const std::vector<PtrAndLength>& desData,
                           size_t numRowsPerBlock,
                           size_t numColsPerBlock) :
    mNumCols(0),
    mOverallNumRowsPerBlock(0),
    mNumColsPerBlock(0),
    mNumBytesPerRow(0),
    mNumBytesPerPixel(0)
{
    initialize(record, desData, numRowsPerBlock, numColsPerBlock);
}

ByteProvider::~ByteProvider()
{
}

template<typename T>
static void copyFromStreamAndClear_(io::ByteStream& stream,
                                          std::vector<T>& rawBytes)
{
    rawBytes.resize(stream.getSize());
    if (!rawBytes.empty())
    {
        ::memcpy(rawBytes.data(), stream.get(), stream.getSize());
    }

    stream.clear();
}
void ByteProvider::copyFromStreamAndClear(io::ByteStream& stream,
                                          std::vector<sys::byte>& rawBytes)
{
    copyFromStreamAndClear_(stream, rawBytes);
}
void ByteProvider::copyFromStreamAndClear(io::ByteStream& stream,
                                          std::vector<std::byte>& rawBytes)
{
    copyFromStreamAndClear_(stream, rawBytes);
}

void ByteProvider::initializeImpl(const Record& record,
                                  const std::vector<PtrAndLength>& desData,
                                  size_t numRowsPerBlock,
                                  size_t numColsPerBlock)
{
    // Get all the file headers and offsets
    getFileLayout(record, desData);
    mOverallNumRowsPerBlock = numRowsPerBlock;

    size_t numColsWithPad = 0;
    if (numColsPerBlock != 0)
    {
        mNumColsPerBlock = numColsPerBlock;
        numColsWithPad = ImageSubheader::getActualImageDim(
                mNumCols, mNumColsPerBlock);
    }
    else
    {
        mNumColsPerBlock = mNumCols;
        numColsWithPad = mNumCols;
    }

    mNumBytesPerRow = numColsWithPad * mNumBytesPerPixel;
    if (mOverallNumRowsPerBlock == 0)
    {
        mNumRowsPerBlock.resize(record.getNumImages());
        for (size_t ii = 0; ii < mNumRowsPerBlock.size(); ++ii)
        {
            mNumRowsPerBlock[ii] = mImageSegmentInfo[ii].numRows;
        }
    }
    else
    {
        mNumRowsPerBlock = getImageBlocker()->getNumRowsPerBlock();
    }
}

void ByteProvider::initialize(const Record& record,
                              const std::vector<PtrAndLength>& desData,
                              size_t numRowsPerBlock,
                              size_t numColsPerBlock)
{
    // Set image lengths
    const size_t numImages = record.getNumImages();
    mImageDataLengths.resize(numImages);
    for (size_t ii = 0; ii < numImages; ++ii)
    {
        nitf::ImageSegment imageSegment = record.getImages()[ii];
        nitf::ImageSubheader subheader = imageSegment.getSubheader();
        mImageDataLengths[ii] = subheader.getNumBytesOfImageData();
    }
    initializeImpl(record, desData, numRowsPerBlock, numColsPerBlock);
}

void ByteProvider::getFileLayout(const nitf::Record& inRecord,
                                 const std::vector<PtrAndLength>& desData)
{
    std::shared_ptr<io::ByteStream> byteStream(new io::ByteStream());

    nitf::IOStreamWriter io(byteStream);

    // Make a copy just to make sure we don't corrupt anything since the
    // methods are non-const
    nitf::Record record = inRecord.clone();

    // This stuff is low priority so not supported yet
    if (record.getNumGraphics() > 0 ||
        record.getNumLabels() > 0 ||
        record.getNumTexts() > 0 ||
        record.getNumReservedExtensions() > 0)
    {
        throw except::NotImplementedException(Ctxt(
                "Graphics, labels, text, and reserved extensions not yet "
                "supported"));
    }

    nitf::Writer writer;
    writer.prepareIO(io, record);

    //--------------------------------------------------------------------------
    // Write image subheaders
    //--------------------------------------------------------------------------

    const size_t numImages = record.getNumImages();
    mImageSubheaders.resize(numImages);
    mImageSegmentInfo.resize(numImages);

    for (size_t ii = 0; ii < numImages; ++ii)
    {
        nitf::ImageSegment imageSegment = record.getImages()[ii];
        nitf::ImageSubheader subheader = imageSegment.getSubheader();

        nitf::Off comratOff(0);
        writer.writeImageSubheader(subheader,
                                   record.getVersion(),
                                   comratOff);
        copyFromStreamAndClear(*byteStream, mImageSubheaders[ii]);

        const size_t numCols = subheader.getNumCols();
        const size_t numBands = subheader.getNumImageBands();
        const size_t numBytesPerPixel =
                NITF_NBPP_TO_BYTES(subheader.getActualBitsPerPixel()) *
                numBands;
        if (ii == 0)
        {
            mNumCols = numCols;
            mNumBytesPerPixel = numBytesPerPixel;
        }
        else
        {
            if (numCols != mNumCols)
            {
                std::ostringstream ostr;
                ostr << "First image segment had " << mNumCols
                     << " columns but image segment " << ii << " has "
                     << numCols;
                throw except::Exception(Ctxt(ostr.str()));
            }

            if (numBytesPerPixel != mNumBytesPerPixel)
            {
                std::ostringstream ostr;
                ostr << "First image segment had " << mNumBytesPerPixel
                     << " bytes/pxeil but image segment " << ii << " has "
                     << numBytesPerPixel;
                throw except::Exception(Ctxt(ostr.str()));
            }
        }

        mImageSegmentInfo[ii].firstRow = (ii == 0) ?
                0 : mImageSegmentInfo[ii - 1].endRow();

        mImageSegmentInfo[ii].numRows = subheader.getNumRows();
    }

    nitf::Off imageSegmentsTotalNumBytes(0);
    for (size_t ii = 0; ii < numImages; ++ii)
    {
        imageSegmentsTotalNumBytes +=
                mImageSubheaders[ii].size() + mImageDataLengths[ii];
    }

    //--------------------------------------------------------------------------
    // Write data extension segment(s) (subheader + data)
    //--------------------------------------------------------------------------

    const size_t numDESs = record.getNumDataExtensions();
    if (numDESs != desData.size())
    {
        std::ostringstream ostr;
        ostr << "Record has " << numDESs << " DESs but "
             << desData.size() << " DES data was sent in";
        throw except::Exception(Ctxt(ostr.str()));
    }

    std::vector<size_t> desSubheaderLengths(numDESs);
    std::vector<size_t> desDataLengths(numDESs);
    for (size_t ii = 0; ii < numDESs; ++ii)
    {
        nitf::DESegment deSegment = record.getDataExtensions()[ii];
        nitf::DESubheader subheader = deSegment.getSubheader();
        const size_t prevSize = byteStream->getSize();
        uint32_t userSublen;
        writer.writeDESubheader(subheader, userSublen, record.getVersion());
        desSubheaderLengths[ii] = byteStream->getSize() - prevSize;

        // Write data
        const PtrAndLength& curData(desData[ii]);
        byteStream->write(curData.first, curData.second);
        desDataLengths[ii] = curData.second;
    }

    copyFromStreamAndClear(*byteStream, mDesSubheaderAndData);

    //--------------------------------------------------------------------------
    // Write the file header
    //--------------------------------------------------------------------------

    // This initial write won't set a number of the lengths, so we'll seek
    // around to set those ourselves
    nitf_Off fileLenOff;
    uint32_t hdrLen;
    record.setComplexityLevelIfUnset();
    writer.writeHeader(fileLenOff, hdrLen);
    const nitf::Off fileHeaderNumBytes = gsl::narrow<nitf::Off>(byteStream->getSize());

    // Overall file length and header length
    mFileNumBytes =
            fileHeaderNumBytes + imageSegmentsTotalNumBytes +
            gsl::narrow<nitf::Off>(mDesSubheaderAndData.size());

    byteStream->seek(fileLenOff, io::Seekable::START);
    writer.writeInt64Field(gsl::narrow<uint64_t>(mFileNumBytes), NITF_FL_SZ, '0', NITF_WRITER_FILL_LEFT);
    writer.writeInt64Field(hdrLen, NITF_HL_SZ, '0', NITF_WRITER_FILL_LEFT);

    // Image segments
    byteStream->seek(NITF_NUMI_SZ, io::Seekable::CURRENT);
    for (size_t ii = 0; ii < numImages; ++ii)
    {
        writer.writeInt64Field(mImageSubheaders[ii].size(), NITF_LISH_SZ, '0',
                                NITF_WRITER_FILL_LEFT);

        writer.writeInt64Field(mImageDataLengths[ii], NITF_LI_SZ, '0',
                                NITF_WRITER_FILL_LEFT);
    }

    // Seek past all the 3 byte lengths for the various other types that we
    // don't have in our file (graphics, NUMX, text, plus the number of DESs
    // that we've already written to the file anyhow)
    byteStream->seek(NITF_NUMS_SZ + NITF_NUMX_SZ + NITF_NUMT_SZ + NITF_NUMDES_SZ,
                     io::Seekable::CURRENT);

    // Data extension segments
    for (size_t ii = 0; ii < numDESs; ++ii)
    {
        writer.writeInt64Field(desSubheaderLengths[ii], NITF_LDSH_SZ, '0',
                               NITF_WRITER_FILL_LEFT);

        writer.writeInt64Field(desDataLengths[ii], NITF_LD_SZ, '0',
                               NITF_WRITER_FILL_LEFT);
    }

    copyFromStreamAndClear(*byteStream, mFileHeader);

    // Figure out where the image subheader offsets are
    mImageSubheaderFileOffsets.resize(numImages);
    nitf::Off offset = gsl::narrow<nitf::Off>(mFileHeader.size());
    for (size_t ii = 0; ii < numImages; ++ii)
    {
         mImageSubheaderFileOffsets[ii] = offset;
         offset += gsl::narrow<nitf::Off>(mImageSubheaders[ii].size()) +
                 mImageDataLengths[ii];
    }

    // DES is right after that
    mDesSubheaderFileOffset = offset;
}

mem::auto_ptr<const ImageBlocker> ByteProvider::getImageBlocker() const
{
    std::vector<size_t> numRowsPerSegment(mImageSegmentInfo.size());
    for (size_t ii = 0; ii < mImageSegmentInfo.size(); ++ii)
    {
        numRowsPerSegment[ii] = mImageSegmentInfo[ii].numRows;
    }

    mem::auto_ptr<const ImageBlocker> blocker(new ImageBlocker(
            numRowsPerSegment,
            mNumCols,
            mOverallNumRowsPerBlock,
            mNumColsPerBlock));

    return blocker;
}

void ByteProvider::checkBlocking(size_t seg,
                                 size_t startGlobalRowToWrite,
                                 size_t numRowsToWrite) const
{
    if (mOverallNumRowsPerBlock != 0)
    {
        const SegmentInfo& imageSegmentInfo(mImageSegmentInfo[seg]);
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

size_t ByteProvider::countPadRows(
        size_t seg, size_t /*numRowsToWrite*/, size_t imageDataEndRow) const noexcept
{
    const SegmentInfo& imageSegmentInfo(mImageSegmentInfo[seg]);
    const size_t numRowsPerBlock(mNumRowsPerBlock[seg]);
    size_t numPadRows = 0;

    if (numRowsPerBlock != 0 &&
        imageDataEndRow >= imageSegmentInfo.endRow())
    {
        const size_t numLeftovers =
                imageSegmentInfo.numRows % numRowsPerBlock;
        if (numLeftovers != 0)
        {
            // We need to finish the block
            numPadRows = numRowsPerBlock - numLeftovers;
        }
    }

    return numPadRows;
}

void ByteProvider::addImageData(
        size_t seg,
        size_t numRowsToWrite,
        size_t startRow,
        size_t imageDataEndRow,
        size_t startGlobalRowToWrite,
        const void* imageData,
        size_t& numPadRowsSoFar,
        nitf::Off& fileOffset,
        NITFBufferList& buffers) const
{
    const SegmentInfo& imageSegmentInfo = mImageSegmentInfo[seg];
    const size_t segStartRow = imageSegmentInfo.firstRow;

    // Figure out what offset of 'imageData' we're writing from
    const size_t startLocalRowToWrite =
            startGlobalRowToWrite - startRow + numPadRowsSoFar;
    const auto imageDataPtr =
            static_cast<const sys::byte*>(imageData) +
            startLocalRowToWrite * mNumBytesPerRow;

    if (buffers.empty())
    {
        const size_t rowsInSegmentSkipped =
                startRow - segStartRow;

        fileOffset = gsl::narrow<nitf::Off>(mImageSubheaderFileOffsets[seg] +
                mImageSubheaders[seg].size() +
                rowsInSegmentSkipped * mNumBytesPerRow);
    }

    const size_t numPadRows = countPadRows(seg, numRowsToWrite, imageDataEndRow);
    numRowsToWrite += numPadRows;
    numPadRowsSoFar += numPadRows;

    buffers.pushBack(imageDataPtr, numRowsToWrite * mNumBytesPerRow);
}

size_t ByteProvider::countBytesForHeaders(size_t seg, size_t startRow) const noexcept
{
    size_t numBytes = 0;
    if (shouldAddHeader(seg, startRow))
    {
        numBytes += mFileHeader.size();
    }
    if (shouldAddSubheader(seg, startRow))
    {
        numBytes += mImageSubheaders[seg].size();
    }
    return numBytes;
}

bool ByteProvider::shouldAddHeader(size_t seg, size_t startRow) const noexcept
{
    return seg == 0 && startRow == 0;
}

bool ByteProvider::shouldAddSubheader(size_t seg, size_t startRow) const noexcept
{
    const size_t segStartRow = mImageSegmentInfo[seg].firstRow;
    return startRow <= segStartRow;
}

void ByteProvider::addHeaders(size_t seg,
        size_t startRow,
        nitf::Off& fileOffset,
        NITFBufferList& buffers) const
{
    if (shouldAddHeader(seg, startRow))
    {
        fileOffset = 0;
        buffers.pushBack(mFileHeader);
    }

    if (shouldAddSubheader(seg, startRow))
    {
        if (buffers.empty())
        {
            fileOffset = mImageSubheaderFileOffsets[seg];
        }
        buffers.pushBack(mImageSubheaders[seg]);
    }
}

bool ByteProvider::shouldAddDES(size_t seg, size_t imageDataEndRow) const noexcept
{
    // When we write out the last row of the last image segment, we
    // tack on the DES(s)
    return (seg == mImageSegmentInfo.size() - 1 &&
            mImageSegmentInfo[seg].endRow() == imageDataEndRow);
}

size_t ByteProvider::countBytesForDES(size_t seg, size_t imageDataEndRow) const noexcept
{
    return shouldAddDES(seg, imageDataEndRow) ? mDesSubheaderAndData.size() : 0;
}

void ByteProvider::addDES(size_t seg, size_t imageDataEndRow,
        NITFBufferList& buffers) const
{
    if (shouldAddDES(seg, imageDataEndRow) && !mDesSubheaderAndData.empty())
    {
        buffers.pushBack(mDesSubheaderAndData);
    }
}

nitf::Off ByteProvider::getNumBytes(size_t startRow, size_t numRows) const
{
    nitf::Off numBytes(0);
    const size_t imageDataEndRow = startRow + numRows;

    for (size_t seg = 0; seg < mImageSegmentInfo.size(); ++seg)
    {
        // See if we're in this segment
        const SegmentInfo& imageSegmentInfo(mImageSegmentInfo[seg]);

        size_t startGlobalRowToWrite;
        size_t numRowsToWrite;
        if (imageSegmentInfo.isInRange(startRow, numRows,
                                       startGlobalRowToWrite,
                                       numRowsToWrite))
        {
            checkBlocking(seg, startGlobalRowToWrite, numRowsToWrite);
            numBytes += countBytesForHeaders(seg, startRow);
            numBytes += mNumBytesPerRow *
                    (numRowsToWrite +
                     countPadRows(seg, numRowsToWrite, imageDataEndRow));
            numBytes += countBytesForDES(seg, imageDataEndRow);
        }
    }

    return numBytes;
}

void ByteProvider::getBytes(const void* imageData,
                            size_t startRow,
                            size_t numRows,
                            nitf::Off& fileOffset,
                            NITFBufferList& buffers) const
{
    fileOffset = std::numeric_limits<nitf::Off>::max();
    buffers.clear();

    const size_t imageDataEndRow = startRow + numRows;
    size_t numPadRowsSoFar(0);

    for (size_t seg = 0; seg < mImageSegmentInfo.size(); ++seg)
    {
        // See if we're in this segment
        const SegmentInfo& imageSegmentInfo(mImageSegmentInfo[seg]);

        size_t startGlobalRowToWrite;
        size_t numRowsToWrite;
        if (imageSegmentInfo.isInRange(startRow, numRows,
                                       startGlobalRowToWrite,
                                       numRowsToWrite))
        {
            checkBlocking(seg, startGlobalRowToWrite, numRowsToWrite);
            addHeaders(seg, startRow, fileOffset, buffers);
            addImageData(seg,
                    numRowsToWrite,
                    startRow,
                    imageDataEndRow,
                    startGlobalRowToWrite,
                    imageData,
                    numPadRowsSoFar,
                    fileOffset,
                    buffers);

            addDES(seg, imageDataEndRow, buffers);
        }
    }
}
}

static std::span<const std::byte> make_span(const std::vector<sys::byte>& v)
{
    const void* const pData = v.data();
    return gsl::make_span(static_cast<const std::byte*>(pData), v.size());
}

void nitf::ByteProvider::getFileHeader(std::span<const std::byte>& result) const
{
    result = make_span(getFileHeader());
}

void nitf::ByteProvider::getImageSubheaders(std::vector<std::span<const std::byte>>& result) const
{
    auto& headers = getImageSubheaders();
    for (auto& header : headers)
    {
        result.emplace_back(make_span(header));
    }
}

void nitf::ByteProvider::getDesSubheaderAndData(std::span<const std::byte>& result) const
{
    result = make_span(getDesSubheaderAndData());
}