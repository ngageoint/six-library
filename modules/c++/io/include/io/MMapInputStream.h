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

#ifndef __IO_MMAP_INPUT_STREAM_H__
#define __IO_MMAP_INPUT_STREAM_H__

#include <cstdio>
#include "sys/OS.h"
#include "io/SeekableStreams.h"


namespace io
{

class MMapInputStream : public SeekableInputStream
{
public:
    MMapInputStream() : mFile(nullptr), mLength(0), mData(nullptr), mMark(0)
    {}
    MMapInputStream(const std::string& inputFile,
                    char* flags = "r+b") :
            mFile(nullptr), mLength(0), mData(nullptr), mMark(0)
    {
        open(inputFile, flags);

    }

    virtual ~MMapInputStream()
    {
        if (mFile)
        {
            close();
        }
    }

    virtual void open(const std::string& fname, char* flags);

    virtual void close();

    sys::Handle_T getHandle();

    virtual off_t available()
    {
        return (mLength - mMark);
    }

    virtual off_t seek(off_t off);

    virtual off_t tell()
    {
        return mMark;
    }

protected:
    virtual sys::SSize_T readImpl(void* buffer, size_t len);

    virtual void _map();
    virtual void _unmap();
    sys::OS mOs;

    FILE* mFile;
    size_t mLength;
    sys::byte* mData;
    size_t mMark;

};

}

#endif
