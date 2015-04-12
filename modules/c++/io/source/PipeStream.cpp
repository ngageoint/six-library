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
#include "io/PipeStream.h"

using namespace io;

sys::SSize_T io::PipeStream::read(sys::byte *cStr, sys::Size_T numBytes)
{
    FILE* pipe = mExecPipe.getPipe();

    size_t bytesLeft = numBytes;
    while (bytesLeft && !feof(pipe))
    {
        sys::SSize_T bytesRead = fread(cStr, 1, bytesLeft, pipe);
        if (bytesRead > 0)
        {
            bytesLeft -= bytesRead;
            cStr += bytesRead;
        }

        // throw if error reading stream
        else if (ferror(pipe))
            throw except::IOException(Ctxt("Error reading from command pipe"));
    }

    // check if didn't read any bytes before end of file
    if (bytesLeft == numBytes)
        return IS_EOF;

    return numBytes - bytesLeft;
}

sys::SSize_T io::PipeStream::readln(sys::byte *cStr,
                                    const sys::Size_T strLenPlusNullByte)
{
    FILE* pipe = mExecPipe.getPipe();

    while (!feof(pipe))
    {
        // get the next line or return null
        if (fgets(cStr, strLenPlusNullByte, pipe) != NULL)
        {
            return strlen(cStr);
        }

        // throw if error reading stream
        if (ferror(pipe))
            throw except::IOException(Ctxt("Error reading from command pipe"));
    }
    return IS_EOF;
}

sys::SSize_T io::PipeStream::streamTo(OutputStream& soi,
                                      sys::SSize_T numBytes)
{
    sys::SSize_T totalBytesRead = 0;
    if (numBytes == IS_END)
    {
        while (!feof(mExecPipe.getPipe()))
        {
            sys::SSize_T bytesRead = read(mCharString.get(), mBufferSize);
            if (bytesRead > 0)
            {
                soi.write(mCharString.get(), bytesRead);
                totalBytesRead += bytesRead;
            }
        }
    }
    else
    {
        sys::Size_T bytesLeft = numBytes;
        while (bytesLeft && !feof(mExecPipe.getPipe()))
        {
            // don't read more bytes than streaming forward or buff size
            sys::SSize_T bytesRead = read(mCharString.get(),
                                          std::min(bytesLeft, mBufferSize));
            if (bytesRead > 0)
            {
                soi.write(mCharString.get(), bytesRead);
                bytesLeft -= bytesRead;
            }
        }

        totalBytesRead = numBytes - bytesLeft;
    }

    // check if didn't read any bytes before end of file
    if (totalBytesRead == 0)
        return IS_EOF;

    return totalBytesRead;
}
