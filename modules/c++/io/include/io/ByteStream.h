/* =========================================================================
 * This file is part of io-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * io-c++ is free software; you can redistribute it and/or modify
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

#ifndef __IO_BYTE_STREAM_H__
#define __IO_BYTE_STREAM_H__

#include <sstream>
#include "io/BidirectionalStream.h"
#include "sys/Conf.h"
#include "except/Error.h"
#include "except/Exception.h"
#include "io/SeekableStreams.h"

/*!
 *  \file
 *  \brief  Class for buffering data, inherits from BidirectionalStream.
 *
 *  This type exists to handle piped data.  If all of your
 *  data is ascii, it is easy just to use a std::string from
 *  C++ to handle this.  However, for binary transfers, arbitrary
 *  0's can be anywhere (Null-bytes) making it impossible to use
 *  strings as containers.  
 * 
 *  Alternatively, we could have used std::stream<const char*>,
 *  but having it in a container makes it all less accessible, so we
 *  opted for our own growable data array
 */
namespace io
{
/*!
 *  \class ByteStream
 *  \brief  Class for buffering data, inherits from BidirectionalStream.
 *
 *  This type exists to handle piped data.  If all of your
 *  data is ascii, it is easy just to use a std::string from
 *  C++ to handle this.  However, for binary transfers, arbitrary
 *  0's can be anywhere (Null-bytes) making it impossible to use
 *  strings as containers.  
 */
class ByteStream :
            public SeekableBidirectionalStream
{
public:

    //! Default constructor
    ByteStream() : mData(std::stringstream::in |
                                 std::stringstream::out |
                                 std::stringstream::binary)
    {}

    //! Destructor
    ~ByteStream()
    {}

    /*!
     *  Returns the stringstream associated with this ByteStream
     *  \return the stringstream
     */
    const std::stringstream& stream() const
    {
        return mData;
    }


    sys::Off_T tell()
    {
        return mData.tellg();
    }

    sys::Off_T seek(sys::Off_T offset, Whence whence)
    {
        std::ios::seekdir flags;
        switch (whence)
        {
            case START:
                flags = std::ios::beg;
                break;

            case END:
                flags = std::ios::end;
                break;

            default:
                flags = std::ios::cur;

        }

        // off_t orig = tell();
        mData.seekg(offset, flags);
        return tell();
    }


    /*!
     *  Returns the available bytes to read from the stream
     *  \return the available bytes to read
     */
    sys::Off_T available();

    /*!
     *  Writes the bytes in data to the stream.
     *  \param b the data to write to the stream
     *  \param size the number of bytes to write to the stream
     */
    void write(const sys::byte *b, sys::Size_T size);

    /*!
     * Read up to len bytes of data from this buffer into an array
     * update the mark
     * \param b   Buffer to read into
     * \param len The length to read
     * \throw IoException
     * \return  The number of bytes read
     */
    virtual sys::SSize_T read(sys::byte *b, sys::Size_T len);

    //! Returns the internal std::stringstream
    std::stringstream& stream()
    {
        return mData;
    }

    /*\!
     *  Reserve size Number of bytes
     *  \param  size  Number of bytes to reserve
     *  \return  Number of bytes allocated
     */ 
    // int  reserve(int size);
    // long reset();
    //      void deallocate();
    //      int  reallocate(int nBytes);

protected:
    std::stringstream mData;
    // int   mDataSize;
    //      int   mBufferSize;
    //      int   mMark;
    // int   mDefaultChunk;
};
}

#endif
