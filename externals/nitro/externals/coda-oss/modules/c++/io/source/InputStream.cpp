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

#include <sys/Conf.h>
#include <except/Exception.h>
#include <io/InputStream.h>

namespace io
{
sys::SSize_T InputStream::read(void* buffer,
                               size_t len,
                               bool verifyFullRead)
{
    const sys::SSize_T numBytes = readImpl(buffer, len);
    if (verifyFullRead)
    {
        if (numBytes == -1)
        {
            std::ostringstream ostr;
            ostr << "Tried to read " << len << " bytes but read failed";
            throw except::IOException(Ctxt(ostr.str()));
        }
        else if (numBytes != static_cast<sys::SSize_T>(len))
        {
            std::ostringstream ostr;
            ostr << "Tried to read " << len << " bytes but only read "
                 << numBytes << " bytes";
            throw except::IOException(Ctxt(ostr.str()));
        }
    }

    return numBytes;
}

sys::SSize_T InputStream::streamTo(OutputStream& soi, sys::SSize_T bytesToPipe)
{

    // In this event, we want to find the end of file,
    // and pipe that many bytes
    if (bytesToPipe == io::InputStream::IS_END)
    {
        bytesToPipe = available();
    }

    // Do some maintenance checking to make sure our state is as expected
    sys::SSize_T bytesRead = 0;
    sys::SSize_T totalBytesTransferred = 0;

    constexpr auto defaultChunkSize = static_cast<sys::SSize_T>(DEFAULT_CHUNK_SIZE);

    sys::SSize_T sizeOfVec = (bytesToPipe <= defaultChunkSize) ? bytesToPipe : defaultChunkSize;
    sys::byte vec[DEFAULT_CHUNK_SIZE];
    memset(vec, 0, DEFAULT_CHUNK_SIZE);

    // While we dont have end of (stream), read into the vec, use the
    // vec to write the pipe, and reset the vec
    // This could be easily done using a char vec as well
    while ( ((bytesRead = read(vec, sizeOfVec)) != io::InputStream::IS_EOF) &&
            (totalBytesTransferred != bytesToPipe))
    {

        soi.write(vec, bytesRead);
        totalBytesTransferred += bytesRead;
        memset(vec, 0, DEFAULT_CHUNK_SIZE);
        sizeOfVec = (bytesToPipe - totalBytesTransferred <= defaultChunkSize) ?
                    (bytesToPipe - totalBytesTransferred) : defaultChunkSize;
    }
    // Return the number of bytes we piped
    return totalBytesTransferred;
}

sys::SSize_T InputStream::readln(sys::byte *cStr, const sys::Size_T strLenPlusNullByte)
{
    // Put a null byte at the end by default
    ::memset(cStr, 0, strLenPlusNullByte);
    // Read length - 1 bytes, because we need a null-byte
    size_t i;
    for (i = 0; i < strLenPlusNullByte - 1; i++)
    {
        // If we got nothing
        if (read(cStr + i, 1) == -1) return -1;
        // Otherwise, if we got newline, return that we read i + 1
        if (*(cStr + i) == '\n') return i + 1;
        // Otherwise, append c;
    }
    return (sys::SSize_T)i;
}
}
