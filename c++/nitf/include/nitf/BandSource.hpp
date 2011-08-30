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

#ifndef __NITF_BANDSOURCE_HPP__
#define __NITF_BANDSOURCE_HPP__

#include "nitf/BandSource.h"
#include "nitf/RowSource.h"
#include "nitf/DataSource.hpp"
#include "nitf/IOHandle.hpp"
#include "nitf/System.hpp"
#include <string>

/*!
 *  \file BandSource.hpp
 *  \brief  Contains wrapper implementations for BandSources
 */
namespace nitf
{
//! BandSource === DataSource
typedef DataSource BandSource;

/*!
 *  \class MemorySource
 *  \brief  The C++ wrapper for the nitf_MemorySource.
 *
 *  The memory source class allows us to read directly from
 *  a data buffer.  In the event that this is a memory-mapped file,
 *  we will likely get a performance gain over the direct fd approach.
 *
 *  The constructor takes in a buffer, a size, and optionally a
 *  sampling factor (Typically, the factor will be applied most
 *  times during the case of memory mapping, although it may be used
 *  to sample down or cut the image into pieces).
 */
class MemorySource : public BandSource
{
public:
    /*!
     *  Constructor
     *  \param data  The memory buffer
     *  \param size  The size of the buffer
     *  \param numBytesPerPixel The number of bytes per pixel
     *  \param start  The start offset
     *  \param pixelSkip  The amount of pixels to skip
     */
    MemorySource(char * data, size_t size, nitf::Off start,
            int numBytesPerPixel, int pixelSkip) throw (nitf::NITFException);
};

/*!
 *  \class FileSource
 *  \brief  The C++ wrapper for the nitf_FileSource
 *
 *  The FileSource class is a BandSource that comes from an open
 *  file descriptor or handle.  Due to any number of constraints,
 *  not the least of which is the band interleaved by pixel case,
 *  we allow the creator to specify a start point, and a pixel skip
 *  (this would help you create a thumbnail as well).
 */
class FileSource : public BandSource
{
public:
    FileSource(const std::string& fname,
               nitf::Off start,
               int numBytesPerPixel,
               int pixelSkip) throw (nitf::NITFException);
    
    /*!
     *  Constructor
     *  \param handle  The handle to store
     *  \param start  The location to seek to (as the beginning)
     *  \param numBytesPerPixel The number of bytes per pixel
     *  \param pixelSkip  The number of pixels to skip each time
     */
    FileSource(nitf::IOHandle& io,
               nitf::Off start,
               int numBytesPerPixel,
               int pixelSkip) throw (nitf::NITFException);
};

struct RowSourceCallback
{
    virtual void nextRow(nitf::Uint32 band, char* buf) throw (nitf::NITFException) = 0;
};

class RowSource : public BandSource
{
public:
    RowSource(nitf::Uint32 band, nitf::Uint32 numRows, nitf::Uint32 numCols,
            nitf::Uint32 pixelSize, RowSourceCallback *callback)
            throw (nitf::NITFException);

private:
    static
    NITF_BOOL nextRow(void* algorithm,
                      nitf_Uint32 band,
                      NITF_DATA* buffer,
                      nitf_Error* error);

private:
    nitf::Uint32 mBand, mNumRows, mNumCols, mPixelSize;
};

}

#endif
