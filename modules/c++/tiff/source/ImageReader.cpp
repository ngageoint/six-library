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

#include "tiff/ImageReader.h"

#include <sstream>
#include <import/io.h>
#include <import/except.h>
#include "tiff/Common.h"
#include "tiff/GenericType.h"
#include "tiff/IFDEntry.h"

void tiff::ImageReader::process(const bool reverseBytes)
{
    mReverseBytes = reverseBytes;

    mIFD.deserialize(*mInput, mReverseBytes);

    mInput->read((sys::byte *)&mNextOffset, sizeof(mNextOffset));
    if (mReverseBytes)
        mNextOffset = sys::byteSwap(mNextOffset);

    // Done here to lower the number of calls to it later.
    mElementSize = mIFD.getElementSize();

    mStripByteCounts = mIFD["StripByteCounts"];
    mStripOffsets = mIFD["StripOffsets"];
}

void tiff::ImageReader::print(io::OutputStream &output) const
{
    mIFD.print(output);
    output.writeln("");

    std::ostringstream message;
    message << "Next IFD Offset:     " << mNextOffset << std::endl;
    output.write(message.str());
}

void tiff::ImageReader::getData(unsigned char *buffer,
        const sys::Uint32_T numElementsToRead)
{
    //see if it is uncompressed
    tiff::IFDEntry *compression = mIFD["Compression"];
    if (compression)
    {
        unsigned short c = *(tiff::GenericType<unsigned short> *)(*compression)[0];
        if (c != tiff::Const::CompressionType::NO_COMPRESSION)
            throw except::Exception(Ctxt(str::Format("Unsupported compression type: %d", c)));
    }
    
    if (mIFD["StripOffsets"])
        getStripData(buffer, numElementsToRead);
    else if (mIFD["TileOffsets"])
        getTileData(buffer, numElementsToRead);
    else
        throw except::Exception(Ctxt("Unsupported TIFF file format"));

    if (mReverseBytes)
        sys::byteSwap(buffer, mElementSize, numElementsToRead);
}

void tiff::ImageReader::getStripData(unsigned char *buffer,
        sys::Uint32_T numElementsToRead)
{
    sys::Uint32_T bufferOffset = 0;
    
    //figure out how far we are in the current strip
    sys::Uint32_T stripOffset = 0;
    for (sys::Uint32_T i = 0; i < mStripIndex; ++i)
        stripOffset += *(tiff::GenericType<sys::Uint32_T> *)(*mStripByteCounts)[i];
    sys::Uint32_T stripPosition = mBytePosition - stripOffset;
    
    //how many bytes do we need to read?
    sys::Uint32_T numBytesToRead = numElementsToRead * mElementSize;

    while (numBytesToRead)
    {
        if (mStripIndex >= mStripOffsets->getCount())
            throw except::Exception(Ctxt("Invalid strip offset index"));

        sys::Uint32_T stripSize = *(tiff::GenericType<sys::Uint32_T> *)(*mStripByteCounts)[mStripIndex];

        // Calculate what remains to be read in the current strip.
        sys::Uint32_T remainingBytesInStrip = stripSize - stripPosition;

        // Seek to the strip offset plus the last read position.
        sys::Uint32_T seekPos = (*(tiff::GenericType<sys::Uint32_T> *)(*mStripOffsets)[mStripIndex]) + stripPosition;

        
        sys::Uint32_T thisRead = numBytesToRead;
        
        // If the total number of bytes to read exceeds the bytes remaining
        // in the current strip, just read what can be read from the current strip.
        if (numBytesToRead > remainingBytesInStrip)
        {
            thisRead = remainingBytesInStrip;
            mStripIndex++; //increment the strip index for next time
        }
        
        // Go to the offset, and read.
        mInput->seek(seekPos, io::Seekable::START);
        mInput->read((sys::byte *)buffer + bufferOffset, thisRead);

        // Update the tile position in bytes.
        mBytePosition += thisRead;

        // Decrement the number of elements remaining to read.
        numBytesToRead -= thisRead;
        bufferOffset += thisRead;

        //reset to 0
        stripPosition = 0;
    }
}

void tiff::ImageReader::getTileData(unsigned char*buffer,
        sys::Uint32_T numElementsToRead)
{
    // Get the image width and length.
    sys::Uint32_T imageElemWidth = mIFD.getImageWidth();
    sys::Uint32_T imageByteWidth = imageElemWidth * mElementSize;

    // Get the tile width.
    tiff::IFDEntry *tileWidth = mIFD["TileWidth"];
    sys::Uint32_T tileElemWidth = 0;
    if (tileWidth->getType() == tiff::Const::Type::LONG)
        tileElemWidth = *(tiff::GenericType<sys::Uint32_T> *)(*tileWidth)[0];
    else
        tileElemWidth = *(tiff::GenericType<unsigned short> *)(*tileWidth)[0];
    sys::Uint32_T tileByteWidth = tileElemWidth * mElementSize;

    // Get the tile length.
    tiff::IFDEntry *tileLength = mIFD["TileLength"];
    sys::Uint32_T tileElemLength = 0;
    if (tileLength->getType() == tiff::Const::Type::LONG)
        tileElemLength = *(tiff::GenericType<sys::Uint32_T> *)(*tileLength)[0];
    else
        tileElemLength = *(tiff::GenericType<unsigned short> *)(*tileLength)[0];

    // Compute the number of tiles wide the image is.
    sys::Uint32_T tilesAcross = (imageElemWidth + tileElemWidth - 1)
            / tileElemWidth;

    // Determine how many bytes were used to pad the right edge.
    sys::Uint32_T widthPadding = (tileByteWidth * tilesAcross) - imageByteWidth;
    sys::Uint32_T bufferOffset = 0;

    while (numElementsToRead)
    {
        sys::Uint32_T bytesToRead = mElementSize * numElementsToRead;

        // Compute the row in image, row in tile, and tile row.
        sys::Uint32_T row = mBytePosition / imageByteWidth;
        sys::Uint32_T tileRow = row / tileElemLength;
        sys::Uint32_T rowInTile = row % tileElemLength;

        // Compute the column in image, column in tile, and tile column.
        sys::Uint32_T column = mBytePosition - (row * imageByteWidth);
        sys::Uint32_T tileColumn = column / tileByteWidth;
        sys::Uint32_T colInTile = column % tileByteWidth;

        // Compute the 1D tile index from the tile row and tile column.
        sys::Uint32_T tileIndex = (tileRow * tilesAcross) + tileColumn;

        sys::Uint32_T paddedBytes = ((tileColumn + 1) / tilesAcross)
                * widthPadding;

        sys::Uint32_T remainingBytesThisLine = tileByteWidth - (paddedBytes
                + (column % tileByteWidth));

        // If the total number of bytes to read exceeds the bytes remaining
        // in the current line of the current tile, just read what can be 
        // read from the current tile.
        if (bytesToRead> remainingBytesThisLine)
            bytesToRead = remainingBytesThisLine;

        // Seek to the tile offset plus the last read position.
        tiff::IFDEntry *tileOffsets = mIFD["TileOffsets"];
        sys::Uint32_T seekPos = (*(tiff::GenericType<sys::Uint32_T> *)(*tileOffsets)[tileIndex]) + (rowInTile * tileByteWidth)
                + colInTile;

        // Go to the offset.
        mInput->seek(seekPos, io::Seekable::START);

        // Read the data.
        mInput->read((sys::byte *)buffer + bufferOffset, bytesToRead);

        // Update the strip position in bytes.
        mBytePosition += bytesToRead;

        // Decrement the number of elements remaining to read.
        bufferOffset += bytesToRead;
        numElementsToRead -= (bytesToRead / mElementSize);
    }
}
