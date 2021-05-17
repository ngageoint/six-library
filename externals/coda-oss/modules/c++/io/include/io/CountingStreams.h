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

#ifndef __IO_COUNTING_STREAMS_H__
#define __IO_COUNTING_STREAMS_H__

#include "io/ProxyStreams.h"

namespace io
{

/**
 * An OutputStream that keeps track of the number of bytes written to the stream.
 */
struct CountingOutputStream: public ProxyOutputStream
{
    CountingOutputStream(OutputStream *proxy, bool ownPtr = false) :
        ProxyOutputStream(proxy, ownPtr), mByteCount(0)
    {
    }
    virtual ~CountingOutputStream() = default;
    CountingOutputStream(const CountingOutputStream&) = delete;
    CountingOutputStream& operator=(const CountingOutputStream&) = delete;

    using ProxyOutputStream::write;

    /*!
     * \param buffer The byte array to write to the stream
     * \param len The length of the byte array to write to the stream
     */
    virtual void write(const void* buffer, size_t len)
    {
        ProxyOutputStream::write(buffer, len);
        mByteCount += len;
    }

    sys::Off_T getCount() const
    {
        return mByteCount;
    }

    void resetCount()
    {
        mByteCount = 0;
    }

protected:
    sys::Off_T mByteCount;
};

}

#endif
