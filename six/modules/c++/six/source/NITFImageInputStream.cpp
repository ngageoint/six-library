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

six::NITFImageInputStream::NITFImageInputStream(nitf::ImageSubheader subheader,
        nitf::ImageReader imageReader) :
    mSubheader(subheader), mReader(imageReader), mRowBufferRemaining(0),
            mRowOffset(0)
{
    int bytesPerPixel = NITF_NBPP_TO_BYTES(subheader.getNumBitsPerPixel());
    mRowSize = (uint32_t) subheader.getNumCols() * bytesPerPixel;

    uint32_t nBands = subheader.getBandCount();
    std::string imageMode = subheader.getImageMode().toString();
    std::string irep = subheader.getImageRepresentation().toString();
    std::string ic = subheader.getImageCompression().toString();

    str::trim(irep);
    str::trim(ic);

    //Check for optimization cases - RGB and IQ
    if ((nBands == 3 && imageMode[0] == 'P' && irep == "RGB" && bytesPerPixel
            == 1 && (ic == "NC" || ic == "NM")) || (nBands == 2 && imageMode[0]
            == 'P' && bytesPerPixel == 4 && (ic == "NC" || ic == "NM")
            && subheader.getBandInfo(0).getSubcategory().toString()[0] == 'I'
            && subheader.getBandInfo(1).getSubcategory().toString()[0] == 'Q'))
    {
        //using special interleaved shortcut
        std::cout << "Using optimized pre pixel-interleaved image" << std::endl;
        mRowSize *= nBands;
        nBands = 1;
    }

    if (nBands > 1)
    {
        throw except::NotImplementedException(
                Ctxt(
                        "Only 1 band images (and special-case RGB) are currently supported"));
    }

    mRowBuffer.reset(new sys::ubyte[mRowSize]);
    mAvailable = mRowSize * (uint32_t) subheader.getNumRows();

    mBandList.reset(new uint32_t[nBands]);
    for (uint32_t band = 0; band < nBands; ++band)
        mBandList.get()[band] = band;

    //setup the window
    mWindow.setStartCol(0);
    mWindow.setNumRows(1);
    mWindow.setNumCols((uint32_t) subheader.getNumCols());
    mWindow.setBandList(mBandList.get());
    mWindow.setNumBands(nBands);
}

sys::Off_T six::NITFImageInputStream::available()
{
    return mAvailable;
}

sys::SSize_T six::NITFImageInputStream::read(sys::byte* b, sys::Size_T len)
{
    //TODO to be 100% complete, we need to interleave multi-bands
    //this does NOT include the special RGB case that is already interleaved
    //the special RGB case is already taken care of here

    sys::Size_T bytesToGo = len, bOffset = 0, rowBufferOffset = 0;

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
    return len;
}

sys::SSize_T six::NITFImageInputStream::readRow()
{
    mWindow.setStartRow(static_cast<uint32_t>(mRowOffset++));
    int padded;
    uint8_t* buffer = mRowBuffer.get();
    mReader.read(mWindow, &buffer, &padded);
    mRowBufferRemaining = mRowSize;
    return (sys::SSize_T)mRowSize;
}

