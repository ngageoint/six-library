/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef __SIX_ADAPTERS_H__
#define __SIX_ADAPTERS_H__

#include <stdint.h>

#include <complex>
#include <utility>
#include <std/span>
#include <std/cstddef>
#include <memory>

#include <scene/sys_Conf.h>
#include <import/io.h>
#include <import/nitf.hpp>
#include <import/sys.h>
#include "six/Types.h"
#include "six/NITFSegmentInfo.h"
#include "six/Utilities.h"
#include "six/Data.h"

/*!
 *  This file contains adapters that are necessary to get six objects
 *  into places that they dont otherwise fit.
 *
 *
 */

namespace six
{

/*!
 *  \class SegmentInputStreamAdapter 
 *  \brief Adapter from segment to CODA InputStream
 *
 *  NITRO use a nitf::SegmentReader to get the data out of the DES.  This
 *  class adapts this object so that it can be used by xml.lite's parser.
 *
 */
class SegmentInputStreamAdapter: public ::io::InputStream
{

    nitf::SegmentReader& mReader;
public:

    /*!
     *  Bind a segment reader for a DES
     *
     */
    SegmentInputStreamAdapter(nitf::SegmentReader& reader) :
        mReader(reader)
    {
    }

    //! Destructor
    ~SegmentInputStreamAdapter()
    {
    }

    SegmentInputStreamAdapter& operator=(const SegmentInputStreamAdapter&) = delete;

    /*!
     *  Fulfills the C++ IO API function returning how many bytes
     *  are left in the stream
     *
     */
    virtual int64_t available()
    {
        return static_cast<int64_t>(mReader.getSize());
    }

protected:
    /*!
     *  Read len bytes from the DES.  Returns the number of 
     *  bytes that succeeded (which is guaranteed to be len
     *  by SegmentReader)
     */
    virtual ptrdiff_t readImpl(void* buffer, size_t len)
    {
        mReader.read(buffer, len);
        return static_cast<ptrdiff_t>(len);
    }
};

/*!
 *  \class MemoryWriteHandler
 *  \brief Overloaded NITF write handler from memory buffer
 *
 *  This is used to write an image buffer from memory.  Each row is read
 *  one at a time and transferred into the write handle.  It makes use
 *  of NITRO's low-level WriteHandler API, which assumes that you will handle
 *  the heavy lifting.  This is not typically used, since the ImageWriter
 *  is more general, but in the case of pixel interleaved data, the 
 *  WriteHandler is the most efficient method of data transfer into a NITF.
 *
 *  This class can handle both SIDD and SICD data.  In the current state
 *  of SIDD, data is always 1 or 3 channels and the size of the channel
 *  is always 1, so there is no byte swapping (this may not necessarily always
 *  remain this way).
 *
 *  For SICD data, we do have 2 channels, one for I and one for Q.  
 *  Additionally the channel size is 4 or 2.
 *
 *  The SICD/SIDD NITF writer sets this object up, one per actual image 
 *  segment. The caller is responsible for dealing with segmentation
 *  beforehand
 *
 */
struct MemoryWriteHandler: public nitf::WriteHandler // leaving this for any existing code that might use it
{
    MemoryWriteHandler(const NITFSegmentInfo& info, 
                       const UByte* buffer,
                       size_t firstRow,
                       size_t numCols,
                       size_t numChannels,
                       size_t pixelSize,
                       bool doByteSwap);
};

class NewMemoryWriteHandler final : public nitf::WriteHandler // all of our code now uses this
{
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;

    NewMemoryWriteHandler(const NITFSegmentInfo& info,
        const std::byte* buffer,
        size_t firstRow, const Data& data, bool doByteSwap);

public:
    NewMemoryWriteHandler() = delete;
    ~NewMemoryWriteHandler();
    NewMemoryWriteHandler(const NewMemoryWriteHandler&) = delete;
    NewMemoryWriteHandler& operator=(const NewMemoryWriteHandler&) = delete;
    NewMemoryWriteHandler(NewMemoryWriteHandler&&) = default;
    NewMemoryWriteHandler& operator=(NewMemoryWriteHandler&&) = default;

    NewMemoryWriteHandler(const NITFSegmentInfo& info,
			  std::span<const std::byte> buffer,
			  size_t firstRow, const Data& data, bool doByteSwap, ptrdiff_t cutoff);
    NewMemoryWriteHandler(const NITFSegmentInfo& info,
			  std::span<const uint8_t> buffer,
			  size_t firstRow, const Data& data, bool doByteSwap, ptrdiff_t);
    NewMemoryWriteHandler(const NITFSegmentInfo& info,
			  std::span<const uint16_t> buffer,
			  size_t firstRow, const Data& data, bool doByteSwap, ptrdiff_t);
    NewMemoryWriteHandler(const NITFSegmentInfo& info,
			  std::span<const six::zfloat> buffer,
			  size_t firstRow, const Data& data, bool doByteSwap, ptrdiff_t cutoff);
    NewMemoryWriteHandler(const NITFSegmentInfo& info,
			  std::span<const std::complex<short>> buffer,
			  size_t firstRow, const Data& data, bool doByteSwap, ptrdiff_t);
    NewMemoryWriteHandler(const NITFSegmentInfo& info,
			  std::span<const std::pair<uint8_t, uint8_t>> buffer,
			  size_t firstRow, const Data& data, bool doByteSwap, ptrdiff_t);
};


/*!
 *  \class StreamWriteHandler
 *  \brief Derived implementation for nitf::WriteHandler
 *
 *  This is used to write an image buffer from a file source.  Each row is read
 *  one at a time and transferred into the write handle.
 *
 *  This class can handle both SIDD and SICD data.  In the current state
 *  of SIDD, data is always 1 or 3 channels and the size of the channel
 *  is always 1, so there is no byte swapping (this may not necessarily always
 *  remain this way).
 *
 *  For SICD data, we do have 2 channels, one for I and one for Q.  
 *  Additionally the channel size is 4 or 2.
 *
 */
struct StreamWriteHandler: public nitf::WriteHandler
{
    StreamWriteHandler(const NITFSegmentInfo& info,
                       io::InputStream* is,
                       size_t numCols,
                       size_t numChannels,
                       size_t pixelSize,
                       bool doByteSwap);
    StreamWriteHandler(const NITFSegmentInfo& info,
                       io::InputStream* is, const Data&, bool doByteSwap);
};

}

#endif

