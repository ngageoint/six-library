/* =========================================================================
 * This file is part of tiff-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * tiff-c++ is free software; you can redistribute it and/or modify
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

#include "tiff/ImageWriter.h"

#include <sstream>
#include <cmath>
#include <import/except.h>

#include "tiff/Common.h"
#include "tiff/GenericType.h"
#include "tiff/IFDEntry.h"

const unsigned short tiff::ImageWriter::CHUNK_SIZE = 8192;

void tiff::ImageWriter::putData(const unsigned char *buffer,
                                sys::Uint32_T numElementsToWrite)
{
    validate();

    if (mFormat == TILED)
    {
        putTileData(buffer, numElementsToWrite);
    }
    else
    {
        putStripData(buffer, numElementsToWrite);
    }
}

void tiff::ImageWriter::writeIFD()
{
    // Retain the current file offset.
    const auto offset = mOutput->tell();

    // Seek to the position to write the current offset to.
    mOutput->seek(mIFDOffset, io::Seekable::START);

    // Write the current offset.
    mOutput->write((sys::byte *)&offset, sizeof(offset));

    // Reseek to the current offset and write out the IFD.
    mOutput->seek(offset, io::Seekable::START);
    mIFD.serialize(*mOutput);

    // Keep the position in the file that the offset to the next
    // IFD can be written to, in case there is another IFD.
    mIFDOffset = mIFD.getNextIFDOffsetPosition();
}

void tiff::ImageWriter::validate()
{
    if (mValidated)
        return;

    // ImageWidth
    tiff::IFDEntry *imageWidth = mIFD["ImageWidth"];
    if (!imageWidth)
        throw except::Exception(Ctxt("ImageWidth must be defined"));

    // ImageLength
    tiff::IFDEntry *imageLength = mIFD["ImageLength"];
    if (!imageLength)
        throw except::Exception(Ctxt("ImageLength must be defined"));

    // Compression
    tiff::IFDEntry *compression = mIFD["Compression"];
    if (!compression)
        mIFD.addEntry("Compression", (unsigned short) 1);
    else
    {
        if (*(tiff::GenericType<unsigned short> *)(*compression)[0] != 1)
            throw except::Exception(Ctxt("Unsupported compression type"));
    }

    // XResolution
    tiff::IFDEntry *xResolution = mIFD["XResolution"];
    if (!xResolution)
    {
        mIFD.addEntry("XResolution", tiff::combine(
                          (sys::Uint32_T) 72, (sys::Uint32_T) 1));
    }

    // YResolution
    tiff::IFDEntry *yResolution = mIFD["YResolution"];
    if (!yResolution)
    {
        mIFD.addEntry("YResolution", tiff::combine(
                          (sys::Uint32_T) 72, (sys::Uint32_T) 1));
    }

    // ResolutionUnit
    tiff::IFDEntry *resolutionUnit = mIFD["ResolutionUnit"];
    if (!resolutionUnit)
        mIFD.addEntry("ResolutionUnit", (unsigned short) 2);

    // SamplesPerPixel
    tiff::IFDEntry *samplesPerPixel = mIFD["SamplesPerPixel"];
    unsigned short spp = (!samplesPerPixel) ? 0
            : (unsigned short)*(tiff::GenericType<unsigned short> *)(*samplesPerPixel)[0];

    // PhotometricInterpretation
    tiff::IFDEntry *photoInterp = mIFD["PhotometricInterpretation"];
    if (!photoInterp)
        throw except::Exception(Ctxt("No default for PhotometricInterpretation; it must be defined"));

    switch (*(tiff::GenericType<unsigned short> *)(*photoInterp)[0])
    {
    case tiff::Const::PhotoInterpType::BLACK_IS_ZERO:
    case tiff::Const::PhotoInterpType::WHITE_IS_ZERO:
        break;

    case tiff::Const::PhotoInterpType::RGB:

        if (!samplesPerPixel)
        {
            spp = 3;
            mIFD.addEntry("SamplesPerPixel", (unsigned short) spp);
            samplesPerPixel = mIFD["SamplesPerPixel"];
        }
        else
        {
            if (spp < 3)
                throw except::Exception(Ctxt("SamplesPerPixel must be at least 3 for RGB files"));
        }

        break;

    case tiff::Const::PhotoInterpType::COLORMAP:

        if (!mIFD["ColorMap"])
            throw except::Exception(Ctxt("Colormap must be defined for ColorMapped files"));

        if (samplesPerPixel && *(tiff::GenericType<unsigned short> *)(*samplesPerPixel)[0] != 1)
            throw except::Exception(Ctxt("SamplesPerPixel must be 1 for ColorMapped files"));

        break;

    default:
        throw except::Exception(Ctxt("Unsupported PhotometricInterpretation"));
    }

    tiff::IFDEntry *bitsPerSample = mIFD["BitsPerSample"];
    if (samplesPerPixel)
    {
        for (int i = 0; i < spp; ++i)
        {
            if (!bitsPerSample)
                mIFD.addEntry("BitsPerSample", (unsigned short) 8);
            else
            {
                unsigned short bps = *(tiff::GenericType<unsigned short> *)(*bitsPerSample)[0];

                if (!(*bitsPerSample)[i])
                    mIFD.addEntryValue("BitsPerSample", (unsigned short) bps);
                else
                {
                    unsigned short value =
                            *(tiff::GenericType<unsigned short> *)(*bitsPerSample)[i];
                    if (value != 8 && i < 3)
                        throw except::Exception(Ctxt("BitsPerSample values must be 8 for RGB files"));
                }
            }

            tiff::IFDEntry *sampleFormat = mIFD["SampleFormat"];
            if (sampleFormat)
            {
                unsigned short format = *(tiff::GenericType<unsigned short> *)(*sampleFormat)[0];
                if (!(*sampleFormat)[i])
                    mIFD.addEntryValue("SampleFormat", (unsigned short) format);
                else
                {
                    unsigned short value =
                            *(tiff::GenericType<unsigned short> *)(*sampleFormat)[i];
                    if (value != 1 && i < 3)
                        throw except::Exception(Ctxt("SampleFormat values must be 1 for RGB files"));
                }
            }
        }
    }

    if (!bitsPerSample)
        mIFD.addEntry("BitsPerSample", (unsigned short) 8);

    mElementSize = mIFD.getElementSize();

    if (mFormat == TILED)
        initTiles();
    else
        initStrips();

    //  if (mGeoTIFFReader)
    //  {
    //    tiff::IFDEntry *entry = nullptr;
    //    if ((entry = (*mGeoTIFFReader)["ModelPixelScaleTag"]))
    //      mIFD.addEntry(entry);
    //    if ((entry = (*mGeoTIFFReader)["ModelTiepointTag"]))
    //      mIFD.addEntry(entry);
    //    if ((entry = (*mGeoTIFFReader)["ModelTransformationTag"]))
    //      mIFD.addEntry(entry);
    //    if ((entry = (*mGeoTIFFReader)["GeoKeyDirectoryTag"]))
    //      mIFD.addEntry(entry);
    //    if ((entry = (*mGeoTIFFReader)["GeoDoubleParamsTag"]))
    //      mIFD.addEntry(entry);
    //    if ((entry = (*mGeoTIFFReader)["GeoAsciiParamsTag"]))
    //      mIFD.addEntry(entry);
    //  }

    mValidated = true;
}

void tiff::ImageWriter::initTiles()
{
    const sys::Uint32_T root = (sys::Uint32_T)sqrt((double)mIdealChunkSize
            / (double)mIFD.getElementSize());
    const sys::Uint32_T ceiling = (sys::Uint32_T)ceil(((double)root) / 16);
    const sys::Uint32_T tileSize = ceiling * 16;

    mIFD.addEntry("TileWidth", (sys::Uint32_T) tileSize);
    mIFD.addEntry("TileLength", (sys::Uint32_T) tileSize);

    auto fileOffset = mOutput->tell();
    const sys::Uint32_T tilesAcross = (mIFD.getImageWidth() + tileSize - 1)
            / tileSize;
    const sys::Uint32_T tilesDown = (mIFD.getImageLength() + tileSize - 1) / tileSize;

    const unsigned short elementSize = mIFD.getElementSize();

    mIFD.addEntry("TileByteCounts");
    mIFD.addEntry("TileOffsets");
    for (sys::Uint32_T y = 0; y < tilesDown; ++y)
    {
        for (sys::Uint32_T x = 0; x < tilesAcross; ++x)
        {
            const sys::Uint32_T byteCount = tileSize * tileSize * elementSize;
            mIFD.addEntryValue("TileOffsets", (sys::Uint32_T) fileOffset);
            mIFD.addEntryValue("TileByteCounts", (sys::Uint32_T) byteCount);
            fileOffset += byteCount;
        }
    }

    mTileOffsets = mIFD["TileOffsets"];
    mTileWidth = mIFD["TileWidth"];
    mTileLength = mIFD["TileLength"];
    mTileByteCounts = mIFD["TileByteCounts"];
}

void tiff::ImageWriter::initStrips()
{
    const sys::Uint32_T bytesPerLine = mIFD.getImageWidth() * mIFD.getElementSize();

    sys::Uint32_T stripByteCount = 0;
    sys::Uint32_T rowsPerStrip = 1;
    if (bytesPerLine > mIdealChunkSize)
        stripByteCount = bytesPerLine;
    else
    {
        rowsPerStrip = (mIdealChunkSize + (mIdealChunkSize >> 1))
                / bytesPerLine;
        stripByteCount = bytesPerLine * rowsPerStrip;
    }

    mIFD.addEntry("RowsPerStrip", rowsPerStrip);

    const sys::Uint32_T length = mIFD.getImageLength();
    const sys::Uint32_T stripsPerImage =
            (sys::Uint32_T)floor(static_cast<double>(length + rowsPerStrip - 1)
                    / static_cast<double>(rowsPerStrip));

    auto offset = mOutput->tell();

    // Add counts and offsets for all but the last strip.
    mIFD.addEntry("StripOffsets");
    mIFD.addEntry("StripByteCounts");
    for (sys::Uint32_T i = 0; i < stripsPerImage - 1; ++i)
    {
        mIFD.addEntryValue("StripOffsets", (sys::Uint32_T) offset);
        mIFD.addEntryValue("StripByteCounts", (sys::Uint32_T) stripByteCount);
        offset += stripByteCount;
    }

    // Add the last offset.
    mIFD.addEntryValue("StripOffsets", (sys::Uint32_T) offset);

    // The last byte count can be less than the previous counts.  This occurs
    // (for example) if RowsPerStrip is even, and ImageLength is odd.
    sys::Uint32_T remainingBytes = mIFD.getImageSize() - ((stripsPerImage - 1)
            * stripByteCount);

    // Add the last byteCount.
    mIFD.addEntryValue("StripByteCounts", remainingBytes);
    mStripByteCounts = mIFD["StripByteCounts"];
}

void tiff::ImageWriter::putTileData(const unsigned char *buffer,
                                    sys::Uint32_T numElementsToWrite)
{
    const sys::Uint32_T imageElemWidth = mIFD.getImageWidth();
    const sys::Uint32_T imageByteWidth = imageElemWidth * mElementSize;

    const sys::Uint32_T tileElemWidth = *(tiff::GenericType<sys::Uint32_T> *)(*mTileWidth)[0];
    const sys::Uint32_T tileByteWidth = tileElemWidth * mElementSize;

    const auto tileElemLength = *(tiff::GenericType<sys::Uint32_T> *)(*mTileLength)[0];

    // Compute the number of tiles wide the image is.
    const sys::Uint32_T tilesAcross = (imageElemWidth + tileElemWidth - 1) / tileElemWidth;

    // Determine how many bytes were used to pad the right edge.
    const sys::Uint32_T widthPadding = (tileByteWidth * tilesAcross) - imageByteWidth;
    sys::Uint32_T globalReadOffset = 0;
    sys::Uint32_T tempBytePosition = mBytePosition;
    const sys::Uint32_T numBytesToWrite = numElementsToWrite * mElementSize;
    sys::Uint32_T currentNumBytesRead = 0;
    sys::Uint32_T remainingElementsToWrite = numElementsToWrite;
    while (remainingElementsToWrite)
    {
        if (((mBytePosition + currentNumBytesRead) / imageByteWidth)
                / tileElemLength > (tempBytePosition / imageByteWidth)
                / tileElemLength)
        {
            tempBytePosition = mBytePosition + currentNumBytesRead;
            globalReadOffset = currentNumBytesRead;
        }

        // Compute the row and tile row.
        const sys::Uint32_T row = tempBytePosition / imageByteWidth;
        const sys::Uint32_T tileRow = row / tileElemLength;

        // Compute the column and tile column.
        const sys::Uint32_T column = tempBytePosition - (row * imageByteWidth);
        const sys::Uint32_T tileColumn = column / tileByteWidth;

        // Compute the 1D tile index from the tile row and tile column.
        const sys::Uint32_T tileIndex = (tileRow * tilesAcross) + tileColumn;

        const sys::Uint32_T tileByteCount = *(tiff::GenericType<sys::Uint32_T> *)(*mTileByteCounts)[tileIndex];

        const sys::Uint32_T rowInTile = row % tileElemLength;
        sys::Uint32_T paddedBytes = ((tileColumn + 1) / tilesAcross) * widthPadding;

        sys::Uint32_T remainingBytesInTile = tileByteCount - (tileElemLength
                * paddedBytes) - (rowInTile * (tileByteWidth - paddedBytes)
                + (column % tileByteWidth));

        tempBytePosition += tileByteWidth - (paddedBytes + (column
                % tileByteWidth));

        sys::byte *copyBuffer = new sys::byte[tileByteWidth * tileElemLength];
        memset(copyBuffer, 0, tileByteWidth * tileElemLength);
        sys::Uint32_T copyOffset = 0;
        sys::Uint32_T readOffset = 0;
        sys::Uint32_T tempColumn = column;
        unsigned short iteration = 0;
        while (readOffset < numBytesToWrite)
        {
            sys::Uint32_T remainingBytesThisLine = tileByteWidth - (paddedBytes
                    + (tempColumn % tileByteWidth));

            sys::Uint32_T numBytesToCopy = remainingBytesThisLine;

            if (!remainingBytesInTile)
                break;

            if (iteration == 0)
            {
                iteration++;
                readOffset = globalReadOffset;
                globalReadOffset += numBytesToCopy;
            }

            if (numBytesToCopy > numBytesToWrite - readOffset)
                numBytesToCopy = numBytesToWrite - readOffset;

            if (paddedBytes)
            {
                if (((tempColumn % tileByteWidth) + numBytesToCopy)
                        / (tileByteWidth - paddedBytes) == 0)
                    paddedBytes = 0;
            }

            memcpy(copyBuffer + copyOffset, buffer + readOffset, numBytesToCopy);
            copyOffset += (numBytesToCopy + paddedBytes);
            readOffset += (imageByteWidth - (tempColumn % tileByteWidth));
            tempColumn -= (tempColumn % tileByteWidth);
            remainingBytesInTile -= numBytesToCopy;
            remainingElementsToWrite -= (numBytesToCopy / mElementSize);
            currentNumBytesRead += numBytesToCopy;
        }

        sys::Uint32_T seekPos = *(tiff::GenericType<sys::Uint32_T> *)(*mTileOffsets)[tileIndex];
        seekPos += (row % tileElemLength) * tileByteWidth;
        seekPos += (column % tileByteWidth);
        mOutput->seek(seekPos, io::Seekable::START);
        mOutput->write(copyBuffer, copyOffset);
        delete [] copyBuffer;
    }

    mBytePosition += numBytesToWrite;

    // All of the real data is in, just have to pad the bottom of the file.
    if (mBytePosition == mIFD.getImageSize())
    {
        sys::Uint32_T imageElemLength = mIFD.getImageLength();
        sys::Uint32_T tilesDown = (imageElemLength + tileElemLength - 1)
                / tileElemLength;
        const sys::Uint32_T startIndex = (tilesDown - 1) * tilesAcross;
        const sys::Uint32_T paddingStartLine = imageElemLength % tileElemLength;
        if (paddingStartLine)
        {
            const sys::Uint32_T paddedLines = tileElemLength - paddingStartLine;

            sys::byte *padBuffer = new sys::byte[paddedLines * tileByteWidth];
            memset(padBuffer, 0, paddedLines * tileByteWidth);

            for (sys::Uint32_T i = 0; i < tilesAcross; ++i)
            {
                sys::Uint32_T seekPos = *(tiff::GenericType<sys::Uint32_T> *)(*mTileOffsets)[startIndex + i];
                seekPos += paddingStartLine * tileByteWidth;
                mOutput->seek(seekPos, io::Seekable::START);
                mOutput->write(padBuffer, paddedLines * tileByteWidth);
            }

            delete [] padBuffer;
        }

        // If no padding, seek the end of the image.  This is so that 
        // when the IFD is written, it doesn't try to write it into the
        // middle of the file.  This is a patch and a better solution
        // should probably be found.
        else
        {
            auto lastTileIndex = static_cast<sys::Uint32_T>(mTileOffsets->getValues().size() - 1);
            sys::Uint32_T seekPos = *(tiff::GenericType<sys::Uint32_T> *)(*mTileOffsets)[lastTileIndex];
            seekPos += *(tiff::GenericType<sys::Uint32_T> *)(*mTileByteCounts)[lastTileIndex];
            mOutput->seek(seekPos, io::Seekable::START);
        }
    }
}

void tiff::ImageWriter::putStripData(const unsigned char *buffer,
                                     sys::Uint32_T numElementsToWrite)
{
    sys::Uint32_T stripSize = *(tiff::GenericType<sys::Uint32_T> *)(*mStripByteCounts)[0];
    sys::Uint32_T bufferIndex = 0;

    while (numElementsToWrite)
    {
        sys::Uint32_T bytesToWrite = mElementSize * numElementsToWrite;
        sys::Uint32_T stripIndex = mBytePosition / stripSize;
        sys::Uint32_T stripPosition = mBytePosition % stripSize;

        // Calculate what remains to be written in the current strip.
        sys::Uint32_T remainingBytesInStrip =
                (*(tiff::GenericType<sys::Uint32_T> *)(*mStripByteCounts)[stripIndex]) - stripPosition;

        if (bytesToWrite > remainingBytesInStrip)
            bytesToWrite = remainingBytesInStrip;

        mOutput->write((sys::byte *)buffer + bufferIndex, bytesToWrite);
        bufferIndex += bytesToWrite;

        numElementsToWrite -= (bytesToWrite / mElementSize);
        mBytePosition += bytesToWrite;
    }
}
