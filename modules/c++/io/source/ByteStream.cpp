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

#include "io/ByteStream.h"

sys::Off_T io::ByteStream::available()
{
    sys::Off_T where = (sys::Off_T)mData.tellg();

    mData.seekg( 0, std::ios::end );
    sys::Off_T until = (sys::Off_T)mData.tellg();

    mData.seekg( where, std::ios::beg );
    return (until - where);
}

void io::ByteStream::write(const sys::byte *b, sys::Size_T size)
{
    mData.write((const char*)b, size);
}

sys::SSize_T io::ByteStream::read(sys::byte *b, sys::Size_T len)
{
    sys::Off_T maxSize = available();
    if (maxSize <= 0) return io::InputStream::IS_END;

    if (maxSize < (sys::Off_T)len)
        len = maxSize;

    if (len <= 0) return 0;

    sys::SSize_T bytesRead(0);
#if defined(__SUNPRO_CC) && (__SUNPRO_CC == 0x530)
    while (bytesRead < len && mData.good())
    {
        b[bytesRead++] = mData.get();
    }
    len = bytesRead;
#else
    mData.read((char *)b, len);
#endif
    // Could be problem if streams are broken
    // alternately could return gcount in else
    // case above
    return len;
}

