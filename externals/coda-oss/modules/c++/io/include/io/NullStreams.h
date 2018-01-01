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

#ifndef __IO_NULL_STREAMS_H__
#define __IO_NULL_STREAMS_H__

#include "io/InputStream.h"
#include "io/OutputStream.h"

namespace io
{
class NullInputStream : public InputStream
{
public:
    NullInputStream(sys::SSize_T size) :
        mSize(size),
        mAvailable(size)
    {
    }

    virtual ~NullInputStream()
    {
    }

    virtual sys::Off_T available()
    {
        return mAvailable;
    }

    virtual sys::SSize_T readln(sys::byte *cStr,
                                const sys::Size_T strLenPlusNullByte)
    {
        return read(cStr, strLenPlusNullByte);
    }

    virtual sys::SSize_T streamTo(OutputStream& soi,
                                  sys::SSize_T numBytes = IS_END)
    {
        sys::SSize_T toProcess = (numBytes == IS_END) ? numBytes : (mAvailable
                >= numBytes ? numBytes : mAvailable);
        mAvailable -= toProcess;
        for (sys::SSize_T i = 0; i < toProcess; ++i)
            soi.write(processByte());
        return toProcess;
    }

protected:
    sys::SSize_T mSize;
    sys::SSize_T mAvailable;

    virtual sys::byte processByte() const
    {
        return (sys::byte) 0;
    }
    virtual void processBytes(void* buffer, sys::Size_T len) const
    {
        //override for different behavior
        memset(buffer, 0, len);
    }

    virtual sys::SSize_T readImpl(void* buffer, size_t len)
    {
        size_t numToRead =
                (mAvailable >= (sys::SSize_T) len ? len : (size_t) mAvailable);

        mAvailable -= (sys::SSize_T) numToRead;

        if (numToRead == 0)
            throw except::IOException(Ctxt("EOF - no more data to read"));

        processBytes(buffer, numToRead);
        return numToRead;
    }
};

class NullOutputStream : public OutputStream
{
public:
    NullOutputStream()
    {
    }
    virtual ~NullOutputStream()
    {
    }

    void write(sys::byte )
    {
    }

    void write(const std::string& )
    {
    }

    void writeln(const std::string& )
    {
    }

    virtual void write(const void* , size_t )
    {
    }

    virtual void flush()
    {
    }
};
}

#endif
