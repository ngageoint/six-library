/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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
    setNative(x.getNative());
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

ImageSegment::ImageSegment() throw(nitf::NITFException)
{
    setNative(nitf_ImageSegment_construct(&error));
    getNativeOrThrow();
    setManaged(false);
}

ImageSegment::ImageSegment(NITF_DATA * x)
{
    setNative((nitf_ImageSegment*)x);
    getNativeOrThrow();
}

ImageSegment & ImageSegment::operator=(NITF_DATA * x)
{
    setNative((nitf_ImageSegment*)x);
    getNativeOrThrow();
    return *this;
}


nitf::ImageSegment ImageSegment::clone() throw(nitf::NITFException)
{
    nitf::ImageSegment dolly(nitf_ImageSegment_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

ImageSegment::~ImageSegment(){}


nitf::ImageSubheader ImageSegment::getSubheader()
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

nitf::Uint64 ImageSegment::getImageOffset() const
{
    return getNativeOrThrow()->imageOffset;
}

void ImageSegment::setImageOffset(nitf::Uint64 value)
{
    getNativeOrThrow()->imageOffset = value;
}

nitf::Uint64 ImageSegment::getImageEnd() const
{
    return getNativeOrThrow()->imageEnd;
}

void ImageSegment::setImageEnd(nitf::Uint64 value)
{
    getNativeOrThrow()->imageEnd = value;
}
