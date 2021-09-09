/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include "six/NITFImageInputStream.h"

#include <gsl/gsl.h>

six::NITFImageInputStream::NITFImageInputStream(nitf::ImageSubheader subheader,
        nitf::ImageReader imageReader) :
    mSubheader(subheader), mReader(imageReader), mRowBufferRemaining(0),
            mRowOffset(0)
{
    auto bytesPerPixel = NITF_NBPP_TO_BYTES(subheader.getNumBitsPerPixel());
    mRowSize = subheader.numCols() * bytesPerPixel;

    uint32_t nBands = subheader.getBandCount();
    const auto imageMode = subheader.imageMode();
    const auto irep = subheader.imageRepresentation();
    const auto ic = subheader.imageCompression();

    //Check for optimization cases - RGB and IQ
    if ((nBands == 3 && imageMode[0] == 'P' && irep == "RGB" && bytesPerPixel
            == 1 && (ic == "NC" || ic == "NM")) || (nBands == 2 && imageMode[0]
            == 'P' && bytesPerPixel == 4 && (ic == "NC" || ic == "NM")))
    {
        auto subcategory = subheader.getBandInfo(0).subcategory;
        if (subcategory == nitf::Subcategory::I)
        {
            subcategory = subheader.getBandInfo(1).subcategory;
            if (subcategory == nitf::Subcategory::Q)
            {
                //using special interleaved shortcut
                std::cout << "Using optimized pre pixel-interleaved image" << std::endl;
                mRowSize *= nBands;
                nBands = 1;
            }
        }
    }

    if (nBands > 1)
    {
        throw except::NotImplementedException(
                Ctxt(
                        "Only 1 band images (and special-case RGB) are currently supported"));
    }

    mRowBuffer.reset(new sys::ubyte[mRowSize]);
    mAvailable = gsl::narrow<decltype(mAvailable )>(mRowSize* subheader.numRows());

    mBandList.reset(new uint32_t[nBands]);
    for (uint32_t band = 0; band < nBands; ++band)
        mBandList.get()[band] = band;

    //setup the window
    mWindow.setNumRows(1);
    mWindow.setNumCols(gsl::narrow<uint32_t>(subheader.numCols()));
    mWindow.setBandList(mBandList.get());
    mWindow.setNumBands(nBands);
}

int64_t six::NITFImageInputStream::available()
{
    return mAvailable;
}

ptrdiff_t six::NITFImageInputStream::read(std::byte* b, size_t len)
{
    //TODO to be 100% complete, we need to interleave multi-bands
    //this does NOT include the special RGB case that is already interleaved
    //the special RGB case is already taken care of here

    size_t bytesToGo = len, bOffset = 0, rowBufferOffset = 0;

    while (bytesToGo > 0)
    {
        if (mRowBufferRemaining == 0) readRow();

        rowBufferOffset = mRowSize - mRowBufferRemaining;
        if (bytesToGo >= mRowBufferRemaining)
        {
            memcpy(b + bOffset, mRowBuffer.get() + rowBufferOffset,
                    mRowBufferRemaining);
            bytesToGo -= mRowBufferRemaining;
            bOffset += mRowBufferRemaining;
            mRowBufferRemaining = 0;
        }
        else
        {
            memcpy(b + bOffset, mRowBuffer.get() + rowBufferOffset, bytesToGo);
            mRowBufferRemaining -= bytesToGo;
            bytesToGo = 0;
        }
    }
    mAvailable -= len;
    return static_cast<ptrdiff_t>(len);
}
ptrdiff_t six::NITFImageInputStream::read(sys::byte* b, size_t len)
{
    void* b_ = b;
    return read(static_cast<std::byte*>(b_), len);
}
ptrdiff_t six::NITFImageInputStream::read(std::span<sys::byte> b)
{
    return read(b.data(), b.size());
}

ptrdiff_t six::NITFImageInputStream::readRow()
{
    mWindow.setStartRow(static_cast<uint32_t>(mRowOffset++));
    auto buffer = mRowBuffer.get();
    int padded = 0;
    mReader.read(mWindow, &buffer, &padded);
    mRowBufferRemaining = mRowSize;
    return (ptrdiff_t)mRowSize;
}

