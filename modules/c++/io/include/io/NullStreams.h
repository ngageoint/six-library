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

#ifndef __IO_NULL_STREAMS_H__
#define __IO_NULL_STREAMS_H__

#include "io/InputStream.h"
#include "io/OutputStream.h"

namespace io
{
class NullInputStream : public InputStream
{
public:
    NullInputStream(long size) :
        mSize(size), mAvailable(size)
    {
    }

    virtual ~NullInputStream()
    {
    }

    virtual sys::Off_T available()
    {
        return mAvailable;
    }

    virtual sys::SSize_T read(sys::byte* b, sys::Size_T len)
    {
        sys::Size_T numToRead =
                (mAvailable >= (long) len ? len : (sys::Size_T) mAvailable);

        mAvailable -= (long) numToRead;

        if (numToRead == 0)
            throw except::IOException(Ctxt("EOF - no more data to read"));

        processBytes(b, numToRead);
        return numToRead;
    }

    virtual sys::SSize_T readln(sys::byte *cStr,
                                const sys::Size_T strLenPlusNullByte)
    {
        return read(cStr, strLenPlusNullByte);
    }

    virtual int streamTo(OutputStream& soi, long numBytes = IS_END)
    {
        long toProcess = (numBytes == IS_END) ? numBytes : (mAvailable
                >= numBytes ? numBytes : mAvailable);
        mAvailable -= toProcess;
        for (long i = 0; i < toProcess; ++i)
            soi.write(processByte());
        return (int) toProcess;
    }

protected:
    long mSize, mAvailable;

    virtual sys::byte processByte() const
    {
        return (sys::byte) 0;
    }
    virtual void processBytes(sys::byte* b, sys::Size_T len) const
    {
        //override for different behavior
        memset(b, 0, len);
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

    void write(sys::byte b)
    {
    }

    void write(const std::string& str)
    {
    }

    void writeln(const std::string& str)
    {
    }

    virtual void write(const sys::byte* b, sys::Size_T len)
    {
    }

    virtual void flush()
    {
    }

};

}

#endif
