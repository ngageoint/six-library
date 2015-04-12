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

#include "io/FileOutputStreamOS.h"

#if !defined(USE_IO_STREAMS)

io::FileOutputStreamOS::FileOutputStreamOS(const std::string& str,
        int creationFlags)
{
    mFile.create(str, sys::File::WRITE_ONLY, creationFlags);

}

void io::FileOutputStreamOS::create(const std::string& str,
                                    int creationFlags)
{
    mFile.create(str, sys::File::WRITE_ONLY, creationFlags);
    if (!isOpen())
    {
        throw except::FileNotFoundException(
            std::string("File could not be opened: ") + str
        );
    }
}


/*!
 * This method defines a given OutputStream. By defining,
 * this method, you can define the unique attributes of an OutputStream
 * inheriting class.
 * \param b   The byte array to write to the stream
 * \param len the length of bytes to write
 * \throw IOException
 */
void io::FileOutputStreamOS::write(const sys::byte* b, sys::Size_T len)
{
    mFile.writeFrom((const char*)b, len);
}

void io::FileOutputStreamOS::flush()
{
    // Anything?
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

