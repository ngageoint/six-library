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

#include "io/FileOutputStreamOS.h"

#if !defined(USE_IO_STREAMS)

io::FileOutputStreamOS::FileOutputStreamOS(const coda_oss::filesystem::path& str,
        int creationFlags)
{
    mFile.create(str.string(), sys::File::WRITE_ONLY, creationFlags);
}

void io::FileOutputStreamOS::create(const coda_oss::filesystem::path& str_,
                                    int creationFlags)
{
    const auto str = str_.string();
    mFile.create(str, sys::File::WRITE_ONLY, creationFlags);
    if (!isOpen())
    {
        throw except::FileNotFoundException(
            "File could not be opened: " + str);
    }
}

void io::FileOutputStreamOS::write(const void* buffer, size_t len)
{
    mFile.writeFrom(buffer, len);
}

void io::FileOutputStreamOS::flush()
{
    mFile.flush();
}

sys::Off_T io::FileOutputStreamOS::seek(sys::Off_T offset,
                                        io::Seekable::Whence whence)
{
    int fileWhence;
    switch (whence)
    {
    case io::Seekable::START:
        fileWhence = sys::File::FROM_START;
        break;
    case io::Seekable::END:
        fileWhence = sys::File::FROM_END;
        break;
    default:
        fileWhence = sys::File::FROM_CURRENT;
        break;
    }
    return mFile.seekTo(offset, fileWhence);
}

sys::Off_T io::FileOutputStreamOS::tell()
{
    return mFile.getCurrentOffset();
}

#endif
