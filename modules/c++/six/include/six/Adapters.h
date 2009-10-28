/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#include <import/io.h>
#include <import/nitf.hpp>
#include <import/sys.h>
#include "six/Types.h"
#include "six/NITFSegmentInfo.h"
#include "six/Utilities.h"

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

    /*!
     *  Fulfills the C++ IO API function returning how many bytes
     *  are left in the stream
     *
     */
    virtual sys::Off_T available()
    {
        return (sys::Off_T) mReader.getSize();
    }

    /*!
     *  Read len bytes from the DES.  Returns the number of 
     *  bytes that succeeded (which is guaranteed to be len
     *  by SegmentReader)
     */
    virtual sys::SSize_T read(sys::byte* b, sys::Size_T len)
    {
        mReader.read((NITF_DATA*) b, len);
        return len;
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
class MemoryWriteHandler: public nitf::WriteHandler
{
    NITFSegmentInfo mSegmentInfo;
    const UByte* mImageBuffer;
    unsigned long mFirstRow;
    unsigned long mNumCols;
    unsigned long mNumChannels;
    unsigned long mPixelSize;
    bool mDoByteSwap;

public:
    MemoryWriteHandler(const NITFSegmentInfo& info, 
		       const UByte* buffer,
		       unsigned long firstRow, 
		       unsigned long numCols,
		       unsigned long numChannels, 
		       unsigned long pixelSize, 
		       bool doByteSwap) :
      mSegmentInfo(info), mImageBuffer(buffer), mFirstRow(firstRow),
      mNumCols(numCols), mNumChannels(numChannels), 
      mPixelSize(pixelSize), mDoByteSwap(doByteSwap)
    {
        // Dont do it if we only have a byte!
        if (pixelSize / numChannels == 1)
            mDoByteSwap = false;
    }

    /*!
     *  We will choose not to seek here. 
     *  That way we can assume that the offset
     *  is positioned correctly.  This makes it very easy
     *  to use a portion of a file (minus its header) as 
     *  a source for us
     */

    virtual void write(nitf::IOInterface& handle) throw (nitf::NITFException);

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
class StreamWriteHandler: public nitf::WriteHandler
{
    NITFSegmentInfo mSegmentInfo;
    io::InputStream* mInputStream;
    unsigned long mNumCols;
    unsigned long mNumChannels;
    unsigned long mPixelSize;
    bool mDoByteSwap;

public:
    StreamWriteHandler(const NITFSegmentInfo& info, io::InputStream* is,
            unsigned long numCols, unsigned long numChannels,
            unsigned long pixelSize, bool doByteSwap) :
        mSegmentInfo(info), mInputStream(is), mNumCols(numCols), mNumChannels(
                numChannels), mPixelSize(pixelSize), mDoByteSwap(doByteSwap)
    {
        // Dont do it if we only have a byte!
        if ((pixelSize / numChannels) == 1)
            mDoByteSwap = false;
    }

    /*!
     *  We will choose not to seek here. 
     *  That way we can assume that the offset
     *  is positioned correctly.  This makes it very easy
     *  to use a portion of a file (minus its header) as 
     *  a source for us
     */

    virtual void write(nitf::IOInterface& handle) throw (nitf::NITFException);

};

}

#endif
