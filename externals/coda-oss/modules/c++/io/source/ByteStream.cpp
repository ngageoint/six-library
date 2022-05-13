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

#include "io/ByteStream.h"

sys::Off_T io::ByteStream::seek(sys::Off_T offset, Whence whence)
{
    if (mPosition < 0)
        throw except::Exception(Ctxt("Invalid seek on eof"));

    switch (whence)
    {
    case START:
        mPosition = offset;
        break;
    case END:
        if (offset > static_cast<sys::Off_T>(mData.size()))
        {
            mPosition = 0;
        }
        else
        {
            mPosition = mData.size() - offset;
        }
        break;
    case CURRENT:
    default:
        mPosition += offset;
        break;
    }

    if (mPosition > static_cast<sys::Off_T>(mData.size()))
        mPosition = -1;
    return tell();
}

sys::Off_T io::ByteStream::available()
{
    if (mPosition < 0)
        throw except::Exception(Ctxt("Invalid available bytes on eof"));

    sys::Off_T where = mPosition;
    sys::Off_T until = static_cast<sys::Off_T>(mData.size());
    sys::Off_T diff = until - where;
    return (diff < 0) ? 0 : diff;
}

void io::ByteStream::write(const void* buffer, sys::Size_T size)
{
    if (mPosition < 0)
        throw except::Exception(Ctxt("Invalid write on eof"));

    // Guard against &mData[mPosition] below indexing out of bounds
    // when mPosition is still 0
    if (size > 0)
    {
        sys::Size_T newPos = mPosition + size;
        if (newPos >= mData.size())
            mData.resize(newPos);

        const sys::ubyte* const bufferPtr =
                static_cast<const sys::ubyte*>(buffer);
        std::copy(bufferPtr, bufferPtr + size, &mData[mPosition]);
        mPosition = newPos;
    }
}

sys::SSize_T io::ByteStream::readImpl(void* buffer, size_t len)
{
    if (mPosition < 0)
        throw except::Exception(Ctxt("Invalid read on eof"));

    sys::Off_T maxSize = available();
    if (maxSize <= 0) return io::InputStream::IS_END;

    if (maxSize <  static_cast<sys::Off_T>(len)) len = maxSize;
    if (len     <= 0)                            return 0;

    ::memcpy(buffer, &mData[mPosition], len);
    mPosition += len;
    return len;
}

