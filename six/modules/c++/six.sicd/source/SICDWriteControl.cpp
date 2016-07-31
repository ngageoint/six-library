/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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

#include <six/sicd/SICDWriteControl.h>

namespace six
{
namespace sicd
{
SICDWriteControl::SICDWriteControl(const std::string& outputFile,
                                   const std::vector<std::string>& schemaPaths) :
    mIO(new nitf::BufferedWriter(outputFile, DEFAULT_BUFFER_SIZE)),
    mSchemaPaths(schemaPaths)
{
}

void SICDWriteControl::setComplexityLevelIfRequired()
{
    // Fill out the complexity level if they didn't set it
    char* const clevelRaw =
            mRecord.getHeader().getComplianceLevel().getRawData();

    if (!strncmp(clevelRaw, "00", 2))
    {
        nitf_Error error;
        const NITF_CLEVEL clevel =
            nitf_ComplexityLevel_measure(mRecord.getNativeOrThrow(),
                                         &error);

        if (clevel == NITF_CLEVEL_CHECK_FAILED)
        {
            throw nitf::NITFException(&error);
        }

        nitf_ComplexityLevel_toString(clevel, clevelRaw);
    }
}

void SICDWriteControl::initialize(Container* container)
{
    NITFWriteControl::initialize(container);

    mWriter.prepareIO(*mIO, mRecord);

    // Write the file header
    nitf_Off fileLenOff;
    nitf_Uint32 hdrLen;
    setComplexityLevelIfRequired();
    mWriter.writeHeader(fileLenOff, hdrLen);

    // Write image subheaders
    const size_t numImages = mRecord.getNumImages();
    std::vector<nitf::Off> imageSubLens(numImages);
    std::vector<nitf::Off> imageDataLens(numImages);
    mImageDataStart.resize(numImages);

    size_t numBands = 0;
    size_t numBytesPerPixel = 0;
    for (size_t ii = 0; ii < numImages; ++ii)
    {
        nitf::ImageSegment imageSegment = mRecord.getImages()[ii];
        nitf::ImageSubheader subheader = imageSegment.getSubheader();

        const nitf::Off start = mIO->tell();
        nitf::Off comratOff(0);
        mWriter.writeImageSubheader(subheader,
                                    mRecord.getVersion(),
                                    comratOff);
        mImageDataStart[ii] = mIO->tell();
        imageSubLens[ii] = mImageDataStart[ii] - start;

        // Seek past the pixel data
        // TODO: This will be more complicated when handling blocking for
        //       SIDD as you will have some pad
        // TODO: Would be nice if ImageSubheader had a method to compute this
        //       for you
        const size_t numRows(subheader.getNumRows());
        const size_t numCols(subheader.getNumCols());
        numBands = subheader.getNumImageBands();
        const size_t numBitsPerPixel(subheader.getNumBitsPerPixel());
        numBytesPerPixel = NITF_NBPP_TO_BYTES(numBitsPerPixel);
        const size_t numBytes = numRows * numCols * numBands * numBytesPerPixel;
        imageDataLens[ii] = numBytes;
        mIO->seek(numBytes, NITF_SEEK_CUR);
    }

    // Write DE subheader and data
    const size_t numDEs = mRecord.getNumDataExtensions();
    std::vector<nitf::Off> deSubLens(numDEs);
    std::vector<nitf::Off> deDataLens(numDEs);

    for (size_t ii = 0; ii < numDEs; ++ii)
    {
        nitf::DESegment deSegment = mRecord.getDataExtensions()[ii];
        nitf::DESubheader subheader = deSegment.getSubheader();

        // Write subheader
        const nitf::Off start = mIO->tell();
        nitf::Uint32 userSublen = 999;
        mWriter.writeDESubheader(subheader, userSublen, mRecord.getVersion());
        deSubLens[ii] = mIO->tell() - start;

        // Write XML
        const Data* data = mContainer->getData(ii);

        const std::string desStr =
                six::toValidXMLString(data, mSchemaPaths, mLog, mXMLRegistry);
        deDataLens[ii] = desStr.length();
        mIO->write(desStr.c_str(), desStr.length());
    }

    //--------------------------------------------------------------------------
    // Update file header with real lengths
    //--------------------------------------------------------------------------

    // Overall file length and header length
    const nitf::Off fileLen = mIO->tell();
    mIO->seek(fileLenOff, NITF_SEEK_SET);
    mWriter.writeInt64Field(fileLen, NITF_FL_SZ, '0', NITF_WRITER_FILL_LEFT);
    mWriter.writeInt64Field(hdrLen, NITF_HL_SZ, '0', NITF_WRITER_FILL_LEFT);

    // Image segments
    mIO->seek(NITF_NUMI_SZ, NITF_SEEK_CUR);
    for (size_t ii = 0; ii < numImages; ++ii)
    {
        mWriter.writeInt64Field(imageSubLens[ii], NITF_LISH_SZ, '0',
                                NITF_WRITER_FILL_LEFT);

        mWriter.writeInt64Field(imageDataLens[ii], NITF_LI_SZ, '0',
                                NITF_WRITER_FILL_LEFT);
    }

    // Seek past all the 3 byte lengths for the various other types that we
    // don't have in our file (graphics, NUMX, text, plus the number of DESs
    // that we've already written to the file anyhow)
    mIO->seek(NITF_NUMS_SZ + NITF_NUMX_SZ + NITF_NUMT_SZ + NITF_NUMDES_SZ,
                    NITF_SEEK_CUR);

    // Data extension segments
    for (size_t ii = 0; ii < numDEs; ++ii)
    {
        mWriter.writeInt64Field(deSubLens[ii], NITF_LDSH_SZ, '0',
                                NITF_WRITER_FILL_LEFT);

        mWriter.writeInt64Field(deDataLens[ii], NITF_LD_SZ, '0',
                                NITF_WRITER_FILL_LEFT);
    }
}

void SICDWriteControl::save(void* imageData,
                            const types::RowCol<size_t>& offset,
                            const types::RowCol<size_t>& dims,
                            bool restoreData)
{
    const six::Data* const data = mContainer->getData(0);
    static const size_t NUM_BANDS = 2;
    const size_t numBytesPerPixel = data->getNumBytesPerPixel() / NUM_BANDS;
    const size_t numPixelsToWrite = dims.area() * NUM_BANDS;
    const bool doByteSwap = shouldByteSwap();

    // Byte swap if needed
    if (doByteSwap)
    {
        sys::byteSwap(imageData,
                      static_cast<unsigned short>(numBytesPerPixel),
                      numPixelsToWrite);
    }

    // TODO: Need to compute the correct segment and offset here
    //       Use NITFSegmentInfo for this
    const size_t globalNumCols = data->getNumCols();
    const size_t pixelOffset = offset.row * globalNumCols + offset.col;
    size_t byteOffset = pixelOffset * numBytesPerPixel * NUM_BANDS;

    // TODO: For SIDD we'll have to handle blocking too

    if (dims.col == globalNumCols)
    {
        // Life is easy - one write
        mIO->seek(mImageDataStart[0] + byteOffset, NITF_SEEK_SET);
        mIO->write(imageData, numPixelsToWrite * numBytesPerPixel);
    }
    else
    {
        const sys::ubyte* imageDataPtr = static_cast<sys::ubyte*>(imageData);
        const size_t numBytesPerRow = dims.col * numBytesPerPixel * NUM_BANDS;
        const size_t rowSeekStride = globalNumCols * numBytesPerPixel * NUM_BANDS;

        for (size_t row = 0;
             row < dims.row;
             ++row, byteOffset += rowSeekStride, imageDataPtr += numBytesPerRow)
        {
            mIO->seek(mImageDataStart[0] + byteOffset, NITF_SEEK_SET);
            mIO->write(imageDataPtr, numBytesPerRow);
        }
    }

    // Byte swap back if needed
    if (doByteSwap && restoreData)
    {
        sys::byteSwap(imageData,
                      static_cast<unsigned short>(numBytesPerPixel),
                      numPixelsToWrite);
    }
}

void SICDWriteControl::close()
{
    mIO->close();
}
}
}
