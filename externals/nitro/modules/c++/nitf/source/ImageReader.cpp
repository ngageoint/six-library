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

#include "nitf/ImageReader.hpp"

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
