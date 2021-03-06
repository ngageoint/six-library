/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

#ifndef __IO_PIPE_STREAM_H__
#define __IO_PIPE_STREAM_H__

#include <import/except.h>
#include <str/Convert.h>
#include <sys/Err.h>
#include <sys/Exec.h>
#include <mem/ScopedArray.h>

#include "io/InputStream.h"

namespace io
{

/*!
 *  \class PipeStream
 *  \brief captures the standard output from a pipe
 *         and streams it to the specified location
 */
struct PipeStream : InputStream
{
    /*!
    *  Constructor --
    *  Streams data from a pipe when available
    *
    *  \param pipe             - pipe for reading
    *  \param streamBufferSize - size of internal buffer for streaming
    */
    PipeStream(const std::string& cmd,
               size_t streamBufferSize = DEFAULT_CHUNK_SIZE) : 
        InputStream(),
        mExecPipe(cmd),
        mCharString(new char[streamBufferSize]),
        mBufferSize(streamBufferSize)
    {
        mExecPipe.run();
    }

    //! cleanup the stream if not done already
    virtual ~PipeStream() 
    {
    }

    //! closes the stream connected to the child process manually
    int close()
    { 
        return mExecPipe.closePipe(); 
    }

    /*! 
     *  \func readln
     *  \brief returns one line ending in a newline or the requested size --
     *         requested size cannot be greater than the maxLength
     *         (default 0 means read until max or newline)
     */
    virtual sys::SSize_T readln(sys::byte *cStr,
                                const sys::Size_T strLenPlusNullByte = 0);

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

    PipeStream(const PipeStream&) = delete;
    PipeStream& operator=(const PipeStream&) = delete;

protected:
    /*!
     *  \brief returns the requested size in bytes from the stream
     */
    virtual sys::SSize_T readImpl(void* buffer, size_t len);


    sys::ExecPipe mExecPipe;
    mem::ScopedArray<char> mCharString;
    size_t mBufferSize = DEFAULT_CHUNK_SIZE;
};

}

#endif
