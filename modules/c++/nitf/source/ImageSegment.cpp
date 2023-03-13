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

#include "nitf/ImageSegment.hpp"

using namespace nitf;

ImageSegment::ImageSegment(const ImageSegment & x)
{
    *this = x;
}

ImageSegment & ImageSegment::operator=(const ImageSegment & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

ImageSegment::ImageSegment(nitf_ImageSegment * x)
{
    setNative(x);
    getNativeOrThrow();
}

ImageSegment::ImageSegment() noexcept(false) : ImageSegment(nitf_ImageSegment_construct(&error))
{
    setManaged(false);
}

ImageSegment::ImageSegment(NITF_DATA * x) : ImageSegment(static_cast<nitf_ImageSegment*>(x))
{
}

ImageSegment & ImageSegment::operator=(NITF_DATA * x)
{
    setNative(static_cast<nitf_ImageSegment*>(x));
    getNativeOrThrow();
    return *this;
}


nitf::ImageSegment ImageSegment::clone() const
{
    nitf::ImageSegment dolly(nitf_ImageSegment_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}


nitf::ImageSubheader ImageSegment::getSubheader() const
{
    return nitf::ImageSubheader(getNativeOrThrow()->subheader);
}


void ImageSegment::setSubheader(nitf::ImageSubheader & value)
{
    //release the one currently "owned"
    nitf::ImageSubheader sub = nitf::ImageSubheader(getNativeOrThrow()->subheader);
    sub.setManaged(false);

    //have the library manage the "new" one
    getNativeOrThrow()->subheader = value.getNative();
    value.setManaged(true);
}

uint64_t ImageSegment::getImageOffset() const
{
    return getNativeOrThrow()->imageOffset;
}

void ImageSegment::setImageOffset(uint64_t value)
{
    getNativeOrThrow()->imageOffset = value;
}

uint64_t ImageSegment::getImageEnd() const
{
    return getNativeOrThrow()->imageEnd;
}

void ImageSegment::setImageEnd(uint64_t value)
{
    getNativeOrThrow()->imageEnd = value;
}
