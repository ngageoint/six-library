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

nitf::GenericSource::GenericSource() throw(nitf::NITFException)
{
    static nitf_IDataSource genericSource =
        {
            &GenericSource::readInterface,
            &GenericSource::destructInterface,
            &GenericSource::getSizeInterface,
            &GenericSource::setSizeInterface
        };

    nitf_BandSource *bandSource = reinterpret_cast<nitf_BandSource*>(
        NITF_MALLOC(sizeof(nitf_BandSource)));
    if (!bandSource)
    {
        throw nitf::NITFException(Ctxt("Out of memory"));
    }

    bandSource->data = this;
    bandSource->iface = &genericSource;

    setNative(bandSource);
    setManaged(false);
}

NITF_BOOL nitf::GenericSource::readInterface(NITF_DATA* data,
                                             char* buf,
                                             nitf_Off size,
                                             nitf_Error* error)
{
    GenericSource* const source = reinterpret_cast<GenericSource*>(data);
    if (!source)
    {
        nitf_Error_init(error, "Null pointer reference",
                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }

    try
    {
        source->readImpl(buf, size);
    }
    catch (const except::Exception& ex)
    {
        nitf_Error_init(error,
                        ex.getMessage().c_str(),
                        NITF_CTXT,
                        NITF_ERR_READING_FROM_FILE);
        return NITF_FAILURE;
    }
    catch (const std::exception& ex)
    {
        nitf_Error_init(error,
                        ex.what(),
                        NITF_CTXT,
                        NITF_ERR_READING_FROM_FILE);
        return NITF_FAILURE;
    }
    catch (...)
    {
        nitf_Error_init(error, "Unknown exception", NITF_CTXT, NITF_ERR_READING_FROM_FILE);
        return NITF_FAILURE;
    }

    return NITF_SUCCESS;
}

void nitf::GenericSource::destructInterface(NITF_DATA* )
{
}

nitf_Off nitf::GenericSource::getSizeInterface(NITF_DATA* data,
                                               nitf_Error* error)
{
    GenericSource* const source = reinterpret_cast<GenericSource*>(data);
    if (!source)
    {
        return 0;
    }

    try
    {
        return source->getSizeImpl();
    }
    catch (const except::Exception& ex)
    {
        nitf_Error_init(error,
                        ex.getMessage().c_str(),
                        NITF_CTXT,
                        NITF_ERR_UNK);
        return -1;
    }
    catch (const std::exception& ex)
    {
        nitf_Error_init(error, ex.what(), NITF_CTXT, NITF_ERR_UNK);
        return -1;
    }
    catch (...)
    {
        nitf_Error_init(error, "Unknown exception", NITF_CTXT, NITF_ERR_UNK);
        return -1;
    }
}

NITF_BOOL nitf::GenericSource::setSizeInterface(NITF_DATA* data,
                                                nitf_Off size,
                                                nitf_Error* error)
{
    GenericSource* const source = reinterpret_cast<GenericSource*>(data);
    if (!source)
    {
        nitf_Error_init(error, "Null pointer reference",
                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }

    try
    {
        source->setSizeImpl(size);
    }
    catch (const except::Exception& ex)
    {
        nitf_Error_init(error,
                        ex.getMessage().c_str(),
                        NITF_CTXT,
                        NITF_ERR_UNK);
        return NITF_FAILURE;
    }
    catch (const std::exception& ex)
    {
        nitf_Error_init(error, ex.what(), NITF_CTXT, NITF_ERR_UNK);
        return NITF_FAILURE;
    }
    catch (...)
    {
        nitf_Error_init(error, "Unknown exception", NITF_CTXT, NITF_ERR_UNK);
        return NITF_FAILURE;
    }

    return NITF_SUCCESS;
}
