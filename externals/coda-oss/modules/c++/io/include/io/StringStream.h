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

#ifndef __IO_STRING_STREAM_H__
#define __IO_STRING_STREAM_H__

/*! \file StringStream.h
 *  \brief  A stream interface to the std::stringstream from C++ STL.
 *
 *  String streams are very useful.  They are even more useful with cafe 
 *  streaming capabilities.  The capabilities are added by making the class
 *  inherit from a stream -- a class that can pipe information back and
 *  forth to other streams.
 */

#include <sstream>
#include "io/BidirectionalStream.h"
#include "sys/Conf.h"
#include "io/SeekableStreams.h"

namespace io
{
/*!
 *  The StringStream class is the cafe interface to std::stringstream.
 *  Added capabilities allow it to send and receive information quickly
 *  and easily to any other stream-inheriting class.  
 */
struct StringStream : public SeekableBidirectionalStream
{
    StringStream() = default;

    StringStream(const StringStream&) = delete;
    StringStream& operator=(const StringStream&) = delete;

    /*!
     *  Returns the stringstream associated with this StringStream
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
        std::ios::seekdir flags = std::ios::cur;
        switch (whence)
        {
        case START:
            flags = std::ios::beg;
            break;
        case END:
            flags = std::ios::end;
            break;
        case CURRENT:
        default:
            flags = std::ios::cur;
            break;
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

    using OutputStream::write;

    /*!
     *  Writes the bytes in data to the stream.
     *  \param buffer the data to write to the stream
     *  \param size the number of bytes to write to the stream
     */
    void write(const void* buffer, sys::Size_T size);

    //! Returns the internal std::stringstream
    std::stringstream& stream()
    {
        return mData;
    }

    void reset()
    {
        mData.str("");
        // clear eof/errors/etc.
        mData.clear();
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
    std::stringstream mData{std::stringstream::in | std::stringstream::out | std::stringstream::binary};
};

}
#endif //__STRING_STREAM_H__
