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

#include "nitf/BandSource.hpp"

#include "gsl/gsl.h"

nitf::MemorySource::MemorySource(const void* data,
                                 size_t size,
                                 nitf::Off start,
                                 int numBytesPerPixel,
                                 int pixelSkip)
{
    setNative(nitf_MemorySource_construct(data, gsl::narrow<nitf::Off>(size), start, numBytesPerPixel, pixelSkip, &error));
    setManaged(false);
}

nitf::FileSource::FileSource(nitf::IOHandle & io,
                             nitf::Off start,
                             int numBytesPerPixel,
                             int pixelSkip)
{
    setNative(nitf_IOSource_construct(io.getNative(), start, numBytesPerPixel, pixelSkip, &error));
    setManaged(false);
    io.setManaged(true); //TODO must release this on destruction
}

nitf::FileSource::FileSource(const std::string& fname,
                             nitf::Off start,
                             int numBytesPerPixel,
                             int pixelSkip)
{
    setNative(nitf_FileSource_constructFile(fname.c_str(),
                                            start,
                                            numBytesPerPixel,
                                            pixelSkip, & error));
    setManaged(false);
}

nitf::RowSource::RowSource(
    uint32_t band,
    uint32_t numRows,
    uint32_t numCols,
    uint32_t pixelSize,
    nitf::RowSourceCallback *callback)
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
                                   uint32_t band,
                                   NITF_DATA* buffer,
                                   nitf_Error* error)
{
    auto const callback = static_cast<nitf::RowSourceCallback*>(algorithm);
    if (!callback)
    {
        nitf_Error_init(error, "Null pointer reference",
                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }

    try
    {
        callback->nextRow(band, static_cast<char*>(buffer));
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

nitf::DirectBlockSource::DirectBlockSource(nitf::ImageReader& imageReader, uint32_t numBands)
{
    setNative(nitf_DirectBlockSource_construct(this,
                                               &DirectBlockSource::nextBlock, 
                                               imageReader.getNative(), 
                                               numBands, &error));
    setManaged(false);
}

NITF_BOOL nitf::DirectBlockSource::nextBlock(void* callback,
                                             void* buf,
                                             const void* block,
                                             uint32_t blockNumber,
                                             uint64_t blockSize,
                                             nitf_Error * error)
{
    auto const cb = static_cast<nitf::DirectBlockSource*>(callback);
    if (!cb)
    {
        nitf_Error_init(error, "Null pointer reference",
                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }

    try
    {
        cb->nextBlock(buf, block, blockNumber, blockSize);
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
