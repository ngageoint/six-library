/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#include "nitf/BandSource.hpp"

nitf::MemorySource::MemorySource(char * data,
                                 size_t size,
                                 off_t start,
                                 int numBytesPerPixel,
                                 int pixelSkip) throw(nitf::NITFException)
{
    setNative(nitf_MemorySource_construct(data, size, start, numBytesPerPixel, pixelSkip, &error));
    getNativeOrThrow();

    static nitf_IDataSource iBandSource = getIDataSource();

    mData = getNativeOrThrow()->data;
    mIface = getNativeOrThrow()->iface;

    // Attach 'this' as the data, which will be the data
    // for the BandSource_read function
    getNativeOrThrow()->data = this;
    getNativeOrThrow()->iface = &iBandSource;

    setManaged(false);
}

nitf::FileSource::FileSource(nitf::IOHandle & io,
                             off_t start,
                             int numBytesPerPixel,
                             int pixelSkip) throw(nitf::NITFException)
{
    setNative(nitf_FileSource_construct(io.getHandle(), start, numBytesPerPixel, pixelSkip, &error));
    getNativeOrThrow();

    static nitf_IDataSource iBandSource = getIDataSource();

    mData = getNativeOrThrow()->data;
    mIface = getNativeOrThrow()->iface;

    // Attach 'this' as the data, which will be the data
    // for the BandSource_read function
    getNativeOrThrow()->data = this;
    getNativeOrThrow()->iface = &iBandSource;

    setManaged(false);
}

