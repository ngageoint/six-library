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
#include "io/ByteStream.h"
#include "sys/Conf.h"
#include "io/SeekableStreams.h"

namespace io
{
/*!
 *  The StringStream class is the cafe interface to std::stringstream.
 *  Added capabilities allow it to send and receive information quickly
 *  and easily to any other stream-inheriting class.  
 */
class StringStream : public SeekableBidirectionalStream
{
public:

    //!  Default constructor.
    StringStream()
    {}

    //!  Destructor.
    virtual ~StringStream()
    {}

    /*!
            *  Returns the number of available bytes to read from the stream
            *  \return the number of available bytes
            */
    virtual sys::Off_T available();

    /*!
     *  This method is inherited from OutputStream.  It must be overloaded
     *  for the class to work properly.
     *  \param data The data to write
     *  \param size The size of the data to write
     */
    virtual void write(const sys::byte* data, sys::Size_T size);

    virtual void write(const std::string& s);

    /*!
     *  This is the read method from ByteStream.  It has an added member
     *  for GCC < 3.0, and for sun forte compilers.  The older compilers
     *  have some problems with their read/write operations for stringstreams.
     *  If you have an old version of gcc, or sun, make sure that either
     *  __GCC_LESS_THAN_3_0 or __SUN is defined, and the hack will be used
     *  (it will probably have worse performance, however).
     *  \param  data  The data to read
     *  \param  size  The size of the data to read
     *  \return Size.
     */
    virtual sys::SSize_T read(sys::byte *data, sys::Size_T size);

    /*!
     *  Returns the stringstream associated with this StringStream
     *  \return the stringstream
     */
    const std::stringstream& stream() const
    {
        return mStream.stream();
    }

    sys::Off_T tell()
    {
        return mStream.tell();
    }

    sys::Off_T seek(sys::Off_T offset, Whence whence)
    {
        return mStream.seek(offset, whence);
    }

protected:
    ByteStream mStream;

};
}
#endif //__STRING_STREAM_H__
