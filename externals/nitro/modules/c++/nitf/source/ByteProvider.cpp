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

#include <sstream>
#include <algorithm>
#include <string.h>

#include <except/Exception.h>
#include <nitf/Writer.hpp>
#include <nitf/ByteProvider.hpp>
#include <nitf/IOStreamWriter.hpp>
#include <io/ByteStream.h>

namespace
{
void copyFromStreamAndClear(io::ByteStream& stream,
                            std::vector<sys::byte>& rawBytes)
{
    rawBytes.resize(stream.getSize());
    if (!rawBytes.empty())
    {
        ::memcpy(&rawBytes[0], stream.get(), stream.getSize());
    }

    stream.clear();
}
}

namespace nitf
{
ByteProvider::ByteProvider() :
    mNumCols(0),
    mOverallNumRowsPerBlock(0),
    mNumColsPerBlock(0),
    mNumBytesPerRow(0),
    mNumBytesPerPixel(0)
{
}

void ByteProvider::initialize(nitf::Record& record,
                              const std::vector<PtrAndLength>& desData,
                              size_t numRowsPerBlock,
                              size_t numColsPerBlock)
{
    // Get all the file headers and offsets
    getFileLayout(record, desData);
    mOverallNumRowsPerBlock = numRowsPerBlock;

    size_t numColsWithPad;
    if (numColsPerBlock != 0)
    {
        mNumColsPerBlock = numColsPerBlock;
        numColsWithPad = ImageSubheader::getActualImageDim(
                mNumCols, mNumColsPerBlock);
    }
    else
    {
        numColsWithPad = mNumCols;
    }

    mNumBytesPerRow = numColsWithPad * mNumBytesPerPixel;
    if (mOverallNumRowsPerBlock == 0)
    {
        mNumRowsPerBlock.resize(record.getNumImages());
        std::fill(mNumRowsPerBlock.begin(), mNumRowsPerBlock.end(), 0);
    }
    else
    {
        mNumRowsPerBlock = getImageBlocker()->getNumRowsPerBlock();
    }
}

void ByteProvider::getFileLayout(nitf::Record& inRecord,
                                 const std::vector<PtrAndLength>& desData)
{
    mem::SharedPtr<io::ByteStream> byteStream(new io::ByteStream());

    nitf::IOStreamWriter io(byteStream);

    // Make a copy just to make sure we don't corrupt anything since the
    // methods are non-const
    nitf::Record record = inRecord.clone();

    nitf::Writer writer;
    writer.prepareIO(io, record);

    //--------------------------------------------------------------------------
    // Write image subheaders
    //--------------------------------------------------------------------------

    const size_t numImages = record.getNumImages();
    mImageSubheaders.resize(numImages);
    mImageSegmentInfo.resize(numImages);
    std::vector<nitf::Off> imageDataLens(numImages);
    nitf::Off imageSegmentsTotalNumBytes(0);

    for (size_t ii = 0; ii < numImages; ++ii)
    {
        nitf::ImageSegment imageSegment = record.getImages()[ii];
        nitf::ImageSubheader subheader = imageSegment.getSubheader();

        nitf::Off comratOff(0);
        writer.writeImageSubheader(subheader,
                                   record.getVersion(),
                                   comratOff);
        copyFromStreamAndClear(*byteStream, mImageSubheaders[ii]);

        imageDataLens[ii] = subheader.getNumBytesOfImageData();

        imageSegmentsTotalNumBytes +=
                mImageSubheaders[ii].size() + imageDataLens[ii];

        const size_t numCols = subheader.getNumRows();
        const size_t numBytesPerPixel =
                NITF_NBPP_TO_BYTES(subheader.getActualBitsPerPixel());
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
        nitf::Uint32 userSublen;
        writer.writeDESubheader(subheader, userSublen, record.getVersion());
        desSubheaderLengths[ii] = byteStream->getSize();

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
    nitf_Uint32 hdrLen;
    record.setComplexityLevelIfUnset();
    writer.writeHeader(fileLenOff, hdrLen);
    const nitf::Off fileHeaderNumBytes = byteStream->getSize();

    // Overall file length and header length
    mFileNumBytes =
            fileHeaderNumBytes + imageSegmentsTotalNumBytes +
            mDesSubheaderAndData.size();

    byteStream->seek(fileLenOff, io::Seekable::START);
    writer.writeInt64Field(mFileNumBytes, NITF_FL_SZ, '0', NITF_WRITER_FILL_LEFT);
    writer.writeInt64Field(hdrLen, NITF_HL_SZ, '0', NITF_WRITER_FILL_LEFT);

    // Image segments
    byteStream->seek(NITF_NUMI_SZ, io::Seekable::CURRENT);
    for (size_t ii = 0; ii < numImages; ++ii)
    {
        writer.writeInt64Field(mImageSubheaders[ii].size(), NITF_LISH_SZ, '0',
                                NITF_WRITER_FILL_LEFT);

        writer.writeInt64Field(imageDataLens[ii], NITF_LI_SZ, '0',
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
    nitf::Off offset = mFileHeader.size();
    for (size_t ii = 0; ii < numImages; ++ii)
    {
         mImageSubheaderFileOffsets[ii] = offset;
         offset += static_cast<nitf::Off>(mImageSubheaders[ii].size()) +
                 imageDataLens[ii];
    }

    // DES is right after that
    mDesSubheaderFileOffset = offset;
}

std::auto_ptr<const ImageBlocker> ByteProvider::getImageBlocker() const
{
    std::vector<size_t> numRowsPerSegment(mImageSegmentInfo.size());
    for (size_t ii = 0; ii < mImageSegmentInfo.size(); ++ii)
    {
        numRowsPerSegment[ii] = mImageSegmentInfo[ii].numRows;
    }

    std::auto_ptr<const ImageBlocker> blocker(new ImageBlocker(
            numRowsPerSegment,
            mNumCols,
            mOverallNumRowsPerBlock,
            mNumColsPerBlock));

    return blocker;
}
}
