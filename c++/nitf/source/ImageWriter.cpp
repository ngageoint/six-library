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

#include "nitf/ImageWriter.hpp"

using namespace nitf;

ImageWriter::ImageWriter(nitf::ImageSubheader& subheader)
        throw (nitf::NITFException)
{
    setNative(nitf_ImageWriter_construct(subheader.getNative(), &error));
}

ImageWriter::~ImageWriter()
{
    //    if (mAdopt && mImageSource)
    //    {
    //        mImageSource->decRef();
    //        delete mImageSource;
    //    }
}

void ImageWriter::attachSource(nitf::ImageSource imageSource)
        throw (nitf::NITFException)
{
    if (!nitf_ImageWriter_attachSource(getNativeOrThrow(),
                                       imageSource.getNative(), &error))
        throw nitf::NITFException(&error);
    imageSource.setManaged(true);
    //    imageSource->incRef();
    //    mImageSource = imageSource;
    //    mAdopt = adopt;
}

void ImageWriter::setWriteCaching(int enable)
{
    nitf_ImageWriter_setWriteCaching(getNativeOrThrow(), enable);
}

void ImageWriter::setPadPixel(nitf::Uint8* value, nitf::Uint32 length)
{
    if (!nitf_ImageWriter_setPadPixel(getNativeOrThrow(), value, length, &error))
        throw nitf::NITFException(&error);
}
