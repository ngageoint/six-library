/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <gsl/gsl.h>

#include "nitf/ImageReader.hpp"

#undef min
#undef max

using namespace nitf;

ImageReader::ImageReader(const ImageReader & x)
{
    setNative(x.getNative());
}

ImageReader & ImageReader::operator=(const ImageReader & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

ImageReader::ImageReader(nitf_ImageReader * x)
{
    setNative(x);
    getNativeOrThrow();
}

nitf::BlockingInfo ImageReader::getBlockingInfo() const
{
    nitf_BlockingInfo* blockingInfo =
            nitf_ImageReader_getBlockingInfo(getNativeOrThrow(), &error);
    // This creates a new object, not a reference to a field,
    // So need to tell the wrapper it needs to destruct itself
    nitf::BlockingInfo cppBlockingInfo(blockingInfo);
    cppBlockingInfo.setManaged(false);
    return cppBlockingInfo;
}

void ImageReader::read(const nitf::SubWindow & subWindow, uint8_t** user, int * padded)
{
    void* pUser = user;
    auto user_ = static_cast<uint8_t**>(pUser);
    const NITF_BOOL x = nitf_ImageReader_read(getNativeOrThrow(), subWindow.getNative(), user_, padded, &error);
    if (!x)
        throw nitf::NITFException(&error);
}

const uint8_t* ImageReader::readBlock(uint32_t blockNumber, uint64_t* blockSize)
{
    const uint8_t* const x = nitf_ImageReader_readBlock(
        getNativeOrThrow(), blockNumber, blockSize, &error);
    if (!x)
        throw nitf::NITFException(&error);
    return x;
}

void ImageReader::setReadCaching()
{
    nitf_ImageReader_setReadCaching(getNativeOrThrow());
}

BufferList<std::byte> ImageReader::read(const nitf::SubWindow& window, size_t nbpp)
{
    // see py_ImageReader_read() and doRead() in test_buffered_read.cpp

    const auto numBitsPerPixel = nbpp;
    const auto numBytesPerPixel = gsl::narrow<size_t>(NITF_NBPP_TO_BYTES(numBitsPerPixel));
    const auto numBytesPerBand = static_cast<size_t>(window.getNumRows()) * static_cast<size_t>(window.getNumCols()) *  numBytesPerPixel;

    auto downsampler = window.getDownSampler();
    const uint32_t rowSkip = downsampler ? downsampler->getRowSkip() : 1;
    const uint32_t colSkip = downsampler ? downsampler->getColSkip() : 1;

    auto imageDeblocker = getNativeOrThrow()->imageDeblocker;
    const auto subimageSize = static_cast<size_t>(window.getNumRows() / rowSkip) *
        (window.getNumCols() / colSkip) * nitf_ImageIO_pixelSize(imageDeblocker);

    BufferList<std::byte> retval(window.getNumBands());
    retval.initialize(subimageSize);
    read(window, retval.data(), &retval.padded);

    return retval;
}

extern "C" {
    NITF_BOOL nitf_ImageIO_getMaskInfo(nitf_ImageIO* nitf,
        uint32_t* imageDataOffset, uint32_t* blockRecordLength,
        uint32_t* padRecordLength, uint32_t* padPixelValueLength,
        uint8_t** padValue, uint64_t** blockMask, uint64_t** padMask);

}

bool ImageReader::getMaskInfo(uint32_t& imageDataOffset, uint32_t& blockRecordLength,
    uint32_t& padRecordLength, uint32_t& padPixelValueLength,
    uint8_t* &padValue, uint64_t* &blockMask, uint64_t* &padMask) const
{
    auto iReader = getNativeOrThrow();
    return nitf_ImageIO_getMaskInfo(iReader->imageDeblocker,
        &imageDataOffset, &blockRecordLength,
        &padRecordLength, &padPixelValueLength,
        &padValue, &blockMask, &padMask);
}