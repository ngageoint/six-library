/* =========================================================================
 * This file is part of io-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <vector>
#include "sys/Conf.h"
#include "except/Error.h"
#include "except/Exception.h"
#include "io/SeekableStreams.h"

/*!
 *  \file
 *  \brief  Class for buffering data, inherits from
 *      SeekableInputStream, SeekableOutputStream
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
 *  \brief  Class for buffering data
 *
 *  This type exists to handle piped data.  If all of your
 *  data is ascii, it is easy just to use a std::string from
 *  C++ to handle this.  However, for binary transfers, arbitrary
 *  0's can be anywhere (Null-bytes) making it impossible to use
 *  strings as containers.  
 */
class ByteStream : public SeekableInputStream, public SeekableOutputStream
{
public:

    //! Default constructor
    ByteStream(sys::Size_T len = 0) :
        mData(len), mPosition(0)
    {
    }

    //! Destructor
    virtual ~ByteStream()
    {
    }

    virtual
    sys::Off_T tell()
    {
        return mPosition;
    }

    virtual
    sys::Off_T seek(sys::Off_T offset, Whence whence);

    /*!
     *  Returns the available bytes to read from the stream
     *  \return the available bytes to read
     */
    virtual
    sys::Off_T available();

    using OutputStream::write;
    using InputStream::streamTo;

    /*!
     *  Writes the bytes in data to the stream.
     *  \param buffer the data to write to the stream
     *  \param size the number of bytes to write to the stream
     */
    virtual
    void write(const void* buffer, size_t size);

    void reset()
    {
        mPosition = 0;
    }

    void clear()
    {
        mPosition = 0;
        mData.clear();
    }
    
    /*!
     * Get a pointer to the internal buffer 
     * This pointer should not be treated as valid
     * after a call to the seek, write, or reset methods
     * \return pointer to the internal buffer
     */
    sys::ubyte *
    get()
    {
        return mData.empty() ? nullptr : &mData[0];
    }

    sys::Size_T
    getSize()
    {
        return mData.size();
    }

protected:
    /*!
     * Read up to len bytes of data from this buffer into an array
     * update the mark
     * \param buffer Buffer to read into
     * \param len The length to read
     * \throw IoException
     * \return  The number of bytes read
     */
    virtual sys::SSize_T readImpl(void* buffer, size_t len);

private:
    std::vector<sys::ubyte> mData;
    sys::Off_T mPosition;
};
}

#endif
