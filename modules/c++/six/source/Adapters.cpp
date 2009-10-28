/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#include "six/Adapters.h"

using namespace six;

void MemoryWriteHandler::write(nitf::IOInterface& handle)
        throw (nitf::NITFException)
{
    unsigned long rowSize = mPixelSize * mNumCols;
    sys::Uint64_T off = mFirstRow * rowSize;

    UByte* rowCopy = new UByte[rowSize];

    for (unsigned int i = 0; i < mSegmentInfo.numRows; i++)
    {

        memcpy(rowCopy, (const sys::byte*) &mImageBuffer[off], rowSize);

        if (mDoByteSwap)
	    sys::byteSwap((sys::byte*) rowCopy, mPixelSize / mNumChannels, mNumCols
                    * mNumChannels);
        // And write it back
        handle.write((const sys::byte*) rowCopy, rowSize);
        off += rowSize;
    }
    delete[] rowCopy;
}

void StreamWriteHandler::write(nitf::IOInterface& handle)
        throw (nitf::NITFException)
{
    unsigned long rowSize = mPixelSize * mNumCols;

    UByte* rowCopy = new UByte[rowSize];

    for (unsigned int i = 0; i < mSegmentInfo.numRows; i++)
    {

        mInputStream->read((sys::byte*) rowCopy, rowSize);

        if (mDoByteSwap)
            sys::byteSwap((sys::byte*) rowCopy, mPixelSize / mNumChannels, mNumCols
                    * mNumChannels);

        // And write it back
        handle.write((const sys::byte*) rowCopy, rowSize);
    }
    delete[] rowCopy;
}
