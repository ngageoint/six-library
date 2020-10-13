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

#include "nitf/ImageWriter.hpp"

using namespace nitf;

ImageWriter::ImageWriter(nitf::ImageSubheader& subheader)
{
    auto* writer =
            nitf_ImageWriter_construct(subheader.getNative(), nullptr, &error);
    setNativeOrThrow(writer, &error);
}

ImageWriter::~ImageWriter()
{
}

void ImageWriter::attachSource(nitf::ImageSource imageSource)
{
    if (!nitf_ImageWriter_attachSource(getNativeOrThrow(),
                                       imageSource.getNative(),
                                       &error))
    {
        throw nitf::NITFException(&error);
    }
    imageSource.setManaged(true);
}

void ImageWriter::setWriteCaching(int enable)
{
    nitf_ImageWriter_setWriteCaching(getNativeOrThrow(), enable);
}

void ImageWriter::setDirectBlockWrite(int enable)
{
    if (!nitf_ImageWriter_setDirectBlockWrite(getNativeOrThrow(),
                                              enable,
                                              &error))
    {
        throw nitf::NITFException(&error);
    }
}

void ImageWriter::setPadPixel(uint8_t* value, uint32_t length)
{
    if (!nitf_ImageWriter_setPadPixel(
                getNativeOrThrow(), value, length, &error))
    {
        throw nitf::NITFException(&error);
    }
}
