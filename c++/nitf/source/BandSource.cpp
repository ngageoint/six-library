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

#include "nitf/BandSource.hpp"

nitf::MemorySource::MemorySource(char * data,
                                 size_t size,
                                 nitf::Off start,
                                 int numBytesPerPixel,
                                 int pixelSkip) throw(nitf::NITFException)
{
    setNative(nitf_MemorySource_construct(data, size, start, numBytesPerPixel, pixelSkip, &error));
    setManaged(false);
}

nitf::FileSource::FileSource(nitf::IOHandle & io,
                             nitf::Off start,
                             int numBytesPerPixel,
                             int pixelSkip) throw(nitf::NITFException)
{
    setNative(nitf_IOSource_construct(io.getNative(), start, numBytesPerPixel, pixelSkip, &error));
    setManaged(false);
    io.setManaged(true); //TODO must release this on destruction
}

nitf::FileSource::FileSource(const std::string& fname,
                             nitf::Off start,
                             int numBytesPerPixel,
                             int pixelSkip) throw (nitf::NITFException)
{
    setNative(nitf_FileSource_constructFile(fname.c_str(),
                                            start,
                                            numBytesPerPixel,
                                            pixelSkip, & error));
    setManaged(false);
}

nitf::RowSource::RowSource(
    nitf::Uint32 band,
    nitf::Uint32 numRows,
    nitf::Uint32 numCols,
    nitf::Uint32 pixelSize,
    nitf::RowSourceCallback *callback) throw(nitf::NITFException)
    : mBand(band),
      mNumRows(numRows),
      mNumCols(numCols),
      mPixelSize(pixelSize)
{
    setNative(nitf_RowSource_construct(callback,
                                       &RowSource::nextRow,
                                       mBand,
                                       mNumRows,
                                       mNumCols * mPixelSize,
                                       &error));
    setManaged(false);
}

NITF_BOOL nitf::RowSource::nextRow(void* algorithm,
                                   nitf_Uint32 band,
                                   NITF_DATA* buffer,
                                   nitf_Error* error)
{
    nitf::RowSourceCallback* const callback =
        reinterpret_cast<nitf::RowSourceCallback*>(algorithm);
    if (!callback)
    {
        nitf_Error_init(error, "Null pointer reference",
                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }

    try
    {
        callback->nextRow(band, (char*)buffer);
    }
    catch (const except::Exception &ex)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_READING_FROM_FILE,
                         ex.getMessage().c_str());
        return NITF_FAILURE;
    }
    catch (const std::exception &ex)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_READING_FROM_FILE,
                         ex.what());
        return NITF_FAILURE;
    }
    catch (...)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_READING_FROM_FILE,
                         "Unknown exception");
        return NITF_FAILURE;
    }

    return NITF_SUCCESS;
}
