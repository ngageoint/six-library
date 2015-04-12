/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

sys::SSize_T io::PipeStream::read(sys::byte *cStr,
                                  const sys::Size_T strLenPlusNullByte)
{
    size_t bytesLeft = strLenPlusNullByte;
    char* tmp = cStr;
    while (bytesLeft)
    {
        size_t bytesRead = readln(tmp, bytesLeft);
        if (bytesRead != -1)
        {
            // take off null terminated byte in count
            bytesLeft -= bytesRead - 1;
            tmp += bytesRead - 1;
        }
        else
        {
            // null terminate the array
            tmp[0] = 0;
            return (bytesLeft + 1) - strLenPlusNullByte;
        }
    }
}

sys::SSize_T io::PipeStream::readln(sys::byte *cStr,
                                    const sys::Size_T strLenPlusNullByte)
{
    // validation
    sys::Size_T readLength = 0;
    if (!strLenPlusNullByte || strLenPlusNullByte > mMaxLength)
    {
        readLength = mMaxLength;
    }

    // get the next line or return null
    if (!feof((FILE*)mExecPipe.getPipe()) &&
        fgets(mCharString.get(), (int)readLength, (FILE*)mExecPipe.getPipe()) != NULL )
    {
        strcpy(cStr, mCharString.get());

        // add 1 because of null termination
        return strlen(cStr) + 1;
    }
    else
    {
        // no byte read --
        // either none left or eof reached
        return -1;
    }
}

sys::SSize_T io::PipeStream::streamTo(OutputStream& soi,
                                      sys::SSize_T numBytes)
{
    size_t totalBytesRead = 0;
    if (numBytes == IS_END)
    {
        // read line by line
        while (!feof((FILE*)mExecPipe.getPipe()))
        {
            if (fgets(mCharString.get(), (int)mMaxLength, 
                        (FILE*)mExecPipe.getPipe()) != NULL)
            {
                size_t bytesRead = strlen(mCharString.get());

                // write without null termination
                soi.write(mCharString.get(), bytesRead - 1);
                totalBytesRead += bytesRead - 1;
            }
        }
    }
    else
    {
        size_t bytesLeft = numBytes;
        while (bytesLeft)
        {
            // read line by line
            if (!feof((FILE*)mExecPipe.getPipe()) && 
                fgets(mCharString.get(), (int)mMaxLength, 
                        (FILE*)mExecPipe.getPipe()) != NULL)
            {
                size_t bytesRead = strlen(mCharString.get());

                // write without null termination
                soi.write(mCharString.get(), bytesRead - 1);
                totalBytesRead += bytesRead - 1;

                // subtract 1 because we want to have room for the null character
                bytesLeft = numBytes - totalBytesRead - 1;
            }
        }
    }

    // null terminate the stream
    soi.write(0);
    return totalBytesRead + 1;
}

