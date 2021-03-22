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

#ifndef __IO_OUTPUT_STREAM_H__
#define __IO_OUTPUT_STREAM_H__

#include "sys/Dbg.h"
#include "sys/Conf.h"

/*!
 * \file OutputStream.h
 * \brief File containing class for handling output streams
 *
 *  This file defines a java-like output interface for streams.
 */

namespace io
{
/*!
 * \class OutputStream
 * \brief Class for handling output streams
 *
 * This class is analogous to the corresponding java class.  It is
 * responsible for writes. 
 *
 */

class OutputStream
{
public:
    //! Default constructor
    OutputStream()
    {
    }
    //! Destructor
    virtual ~OutputStream()
    {
    }

    /*!
     * Write one byte to the stream
     * \throw IOException
     */
    void write(sys::byte b)
    {
        write(&b, 1);
    }

    /*!
     *  Write a string
     *  \param str
     */
    void write(const std::string& str)
    {
        const void* const pStr = str.c_str();
        write(static_cast<const sys::byte*>(pStr), str.length());
    }

    /*!
     *  Write a string with a newline at the end
     *  \param str
     */
    void writeln(const std::string& str)
    {
        write(str);
        write('\n');
    }

    /*!
     * This method defines a given OutputStream. By defining,
     * this method, you can define the unique attributes of an OutputStream
     * inheriting class.
     * \param buffer The byte array to write to the stream
     * \param len The length of the byte array to write to the stream
     * \throw IOException
     */
    virtual void write(const void* buffer, size_t len) = 0;

    /*!
     *  Flush the stream if needed
     */
    virtual void flush()
    {
    }

    /*!
     * Close the stream
     */
    virtual void close()
    {
    }
};
}

#endif
