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

#ifndef __IO_STANDARD_STREAMS_H__
#define __IO_STANDARD_STREAMS_H__

#include "io/OutputStream.h"
#include "except/Exception.h"
#include "sys/Mutex.h"

#if defined(_REENTRANT)

#  define _STDSTREAM_DECLARE_MUTEX_SEMICOLON_ static sys::Mutex mCritSection;
#  define _STDERR_DEFINE_MUTEX_SEMICOLON_ sys::Mutex io::StandardErrStream::mCritSection;
#  define _STDOUT_DEFINE_MUTEX_SEMICOLON_ sys::Mutex io::StandardOutStream::mCritSection;
#  define _STDSTREAM_BEGIN_CS_SEMICOLON_  mCritSection.lock();
#  define _STDSTREAM_END_CS_SEMICOLON_    mCritSection.unlock();

#else

#  define _STDSTREAM_DECLARE_MUTEX_SEMICOLON_
#  define _STDERR_DEFINE_MUTEX_SEMICOLON_
#  define _STDOUT_DEFINE_MUTEX_SEMICOLON_
#  define _STDSTREAM_BEGIN_CS_SEMICOLON_
#  define _STDSTREAM_END_CS_SEMICOLON_

#endif 
/*!
 *  \file
 *  \brief Defines classes for writing to stderr and stdout
 */
namespace io
{
/*!
 * \class StandardOutStream
 * \brief Class for writing output streams to stdout
 */
class StandardOutStream : public OutputStream
{

public:
    //! Default constructor
    StandardOutStream()
    {}
    //! Destructor
    virtual ~StandardOutStream()
    {}

    /*!
     * This method defines a write to stdout.
     * \param b   The byte array to write to the stream
     * \param len the length of bytes to read
     * \throw except::IOException
     */
    void write(const sys::byte* b, sys::Size_T len);

    using OutputStream::write;
    using OutputStream::writeln;

protected:
    _STDSTREAM_DECLARE_MUTEX_SEMICOLON_

};

/*!
 * \class StandardErrStream
 * \brief Class for writing output streams to stderr
 */
class StandardErrStream : public OutputStream
{

public:
    //! Default constructor
    StandardErrStream()
    {}
    //! Destructor
    virtual ~StandardErrStream()
    {}

    /*!
     * This method defines a write to stdout.
     * \param b   The byte array to write to the stream
     * \param len the length of bytes to read
     * \throw except::IOException
     */
    void write(const sys::byte* b, sys::Size_T len);

protected:
    _STDSTREAM_DECLARE_MUTEX_SEMICOLON_

};

}
#endif
