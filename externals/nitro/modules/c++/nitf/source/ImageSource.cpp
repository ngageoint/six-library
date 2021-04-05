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

#include "nitf/ImageSource.hpp"

using namespace nitf;

ImageSource::ImageSource(const ImageSource & x)
{
    setNative(x.getNative());
}

ImageSource & ImageSource::operator=(const ImageSource & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

ImageSource::ImageSource(nitf_ImageSource * x)
{
    setNative(x);
    getNativeOrThrow();
}

ImageSource::ImageSource() : ImageSource(nitf_ImageSource_construct(&error))
{
    setManaged(false);
}

ImageSource::~ImageSource()
{
    //    //go through and delete all the attached bands
    //    for(std::vector<nitf::BandSource*>::iterator it = mBands.begin();
    //        it != mBands.end(); ++it)
    //    {
    //        (*it)->decRef();
    //        delete *it;
    //    }
}

void ImageSource::addBand(nitf::BandSource bandSource)
{
    const NITF_BOOL x = nitf_ImageSource_addBand(getNativeOrThrow(),
                                           bandSource.getNativeOrThrow(),
                                           &error);
    if (!x)
        throw nitf::NITFException(&error);
    bandSource.setManaged(true); //the underlying memory is managed now
    //    mBands.push_back(bandSource);
    //    bandSource->incRef();
}

nitf::BandSource ImageSource::getBand(int n)
{
    nitf_DataSource * x = nitf_ImageSource_getBand(getNativeOrThrow(), n,
                                                   &error);
    if (!x)
        throw nitf::NITFException(&error);

    nitf::BandSource bandSource(x);
    bandSource.setManaged(true);
    return bandSource;
}
