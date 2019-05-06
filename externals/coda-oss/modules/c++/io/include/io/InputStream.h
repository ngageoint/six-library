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

#ifndef __IO_INPUT_STREAM_H__
#define __IO_INPUT_STREAM_H__

#include "sys/Dbg.h"
#include "io/OutputStream.h"

/*!
 *  \file InputStream.h
 *  \brief Contains java-like InputStream object
 *
 *  This file contains the details of the implemenation of
 *  a java-like InputStream interface.  The interface has some
 *  simplifications and some extensions, simultaneously
 */

namespace io
{

/*!
 * \class InputStream
 * \brief Class for handling input byte streams
 *
 * This class is somewhat akin to the corresponding java
 * class.  It is responsible for reading, and it is
 * streamable.  When the streaming feature is used, an internal
 * buffer is used, and the destination is an OutputStream.
 * This "piping" feature is not found in the java class,
 * but it is important because it allows for quick, efficient transfers
 * of large streams of bytes.
 *
 */
class InputStream
{
public:
    enum
    {
        IS_EOF = -1, IS_END = -1, DEFAULT_CHUNK_SIZE = 1024
    };

    //! Default Constructor
    InputStream()
    {
    }

    //! Default Destructor
    virtual ~InputStream()
    {
    }

    /*!
     * Returns the number of bytes that can be read
     * without blocking by the next caller of a method for this input.
     * \throw IOException
     * \return number of bytes which are readable
     */
    virtual sys::Off_T available()
    {
        return 0;
    }

    /*!
     * Read up to len bytes of data from input stream into an array
     * \param buffer Buffer to read into
     * \param len The length to read
     * \param verifyFullRead If set to true, checks to see if 'len' bytes
     * were read and, if not, throws.  Defaults to false.
     * \throw IOException
     * \return  The number of bytes read, or -1 if EOF.  If 'verifyFullRead'
     * is true, this will always return 'len' bytes if it didn't throw.
     */
    sys::SSize_T read(void* buffer,
                      size_t len,
                      bool verifyFullRead = false);

    /*!
     * Read either a buffer of len size, or up until a newline,
     * whichever comes first
     * \param cStr String to read data into
     * \param strLenPlusNullByte The max length we will read
     */
    virtual sys::SSize_T readln(sys::byte *cStr,
                                const sys::Size_T strLenPlusNullByte);

    /*!
     * The streaming occurs as follows: If the numBytes is IS_END, 
     * we want to pipe all bytes to the output handler
     * Otherwise, we'll take what we've got
     * We want to return the number of bytes total.
     * \param soi      Stream to write to
     * \param numBytes The number of bytes to stream
     * \throw BadPipeException
     * \return         The number of bytes transferred from the
     * input stream to the output stream
     */
    virtual sys::SSize_T streamTo(OutputStream& soi,
                                  sys::SSize_T numBytes = IS_END);

protected:
    /*!
     * Read up to len bytes of data from input stream into an array
     * \param buffer   Buffer to read into
     * \param len The length to read
     * \throw IOException
     * \return  The number of bytes read, or -1 if EOF
     */
    virtual sys::SSize_T readImpl(void* buffer, size_t len) = 0;
};
}

#endif
