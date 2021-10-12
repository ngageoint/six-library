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

#ifndef __NITF_BANDSOURCE_HPP__
#define __NITF_BANDSOURCE_HPP__

#include <string.h>
#include <string>

#include "nitf/BandSource.h"
#include "nitf/RowSource.h"
#include "nitf/DirectBlockSource.h"
#include "nitf/DataSource.hpp"
#include "nitf/IOHandle.hpp"
#include "nitf/System.hpp"
#include "nitf/ImageReader.hpp"
#include "nitf/exports.hpp"

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
struct NITRO_NITFCPP_API MemorySource : public BandSource
{
    /*!
     *  Constructor
     *  \param data  The memory buffer
     *  \param size  The size of the buffer
     *  \param start  The start offset
     *  \param numBytesPerPixel The number of bytes per pixel
     *  \param pixelSkip  The amount of pixels to skip
     */
    MemorySource(const void* data, size_t size, nitf::Off start,
            int numBytesPerPixel, int pixelSkip);

    template<typename TSpanLike>
    MemorySource(const TSpanLike& span, nitf::Off start, int pixelSkip)
        : MemorySource(span.data(), span.size(), start, sizeof(span[0]), pixelSkip) {}
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
struct NITRO_NITFCPP_API FileSource : public BandSource
{
    FileSource(const std::string& fname,
               nitf::Off start,
               int numBytesPerPixel,
               int pixelSkip);

    /*!
     *  Constructor
     *  \param io The handle to store
     *  \param start  The location to seek to (as the beginning)
     *  \param numBytesPerPixel The number of bytes per pixel
     *  \param pixelSkip  The number of pixels to skip each time
     */
    FileSource(nitf::IOHandle& io,
               nitf::Off start,
               int numBytesPerPixel,
               int pixelSkip);
};

struct NITRO_NITFCPP_API RowSourceCallback
{
    virtual ~RowSourceCallback() // can't be "noexcpet" as that breaks derived classes
    {
    }

    virtual void nextRow(uint32_t band, void* buf) = 0;
};

struct NITRO_NITFCPP_API RowSource : public BandSource
{
    RowSource(uint32_t band, uint32_t numRows, uint32_t numCols,
            uint32_t pixelSize, RowSourceCallback *callback);

private:
    static
    NITF_BOOL nextRow(void* algorithm,
                      uint32_t band,
                      NITF_DATA* buffer,
                      nitf_Error* error);

private:
    uint32_t mBand, mNumRows, mNumCols, mPixelSize;
};

struct NITRO_NITFCPP_API DirectBlockSource : public BandSource
{
    DirectBlockSource(nitf::ImageReader& imageReader,
                      uint32_t numBands);

protected:
    virtual void nextBlock(void* buf,
                           const void* block,
                           uint32_t blockNumber,
                           uint64_t blockSize) = 0;
private:
    static
    NITF_BOOL nextBlock(void *algorithm,
                        void* buf,
                        const void* block,
                        uint32_t blockNumber,
                        uint64_t blockSize,
                        nitf_Error * error);
};

struct NITRO_NITFCPP_API CopyBlockSource: public ::nitf::DirectBlockSource
{
    CopyBlockSource(nitf::ImageReader& imageReader, uint32_t numBands) :
        nitf::DirectBlockSource(imageReader, numBands)
    {}

    ~CopyBlockSource() = default;

protected:
    void nextBlock(void* buf,
                           const void* block,
                           uint32_t /*blockNumber*/,
                           uint64_t blockSize) noexcept override
    {
        memcpy(buf, block, blockSize);
    }
};


}

#endif
