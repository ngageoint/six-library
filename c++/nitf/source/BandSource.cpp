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
                                 nitf::Off start,
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
                             nitf::Off start,
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


nitf::RowSource::RowSource(nitf::Uint32 band, nitf::Uint32 numRows,
        nitf::Uint32 numCols, nitf::Uint32 pixelSize) throw(nitf::NITFException)
    : mBand(band), mNumRows(numRows), mNumCols(numCols), mPixelSize(pixelSize)
{
    setNative(nitf_RowSource_construct((void*)this, &__nitf_RowSource_nextRow,
                                       mBand, mNumRows, 
                                       mNumCols * mPixelSize, &error));
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


extern "C" NITF_BOOL __nitf_RowSource_nextRow(void *algorithm,
                                              nitf_Uint32 band,
                                              NITF_DATA * buffer,
                                              nitf_Error * error)
{
    nitf::RowSource *source = (nitf::RowSource*)algorithm;
    try
    {
        source->nextRow((char*)buffer);
    }
    catch(nitf::NITFException &ex)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_READING_FROM_FILE,
                ex.getMessage().c_str());
        return NITF_FAILURE;
    }
    return NITF_SUCCESS;
}
