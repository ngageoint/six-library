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

#include "io/FileOutputStreamIOS.h"

#if defined(USE_IO_STREAMS)

io::FileOutputStreamIOS::FileOutputStreamIOS(const char* outputFile,
                                       int creationFlags)
{
    std::ios::openmode mode = std::ios::out;

    if ( (creationFlags & sys::File::TRUNCATE) != sys::File::TRUNCATE ||
            (creationFlags & std::ios::app) == std::ios::app)
        mode |= std::ios::app;

    open(outputFile, mode);
}

io::FileOutputStreamIOS::FileOutputStreamIOS(const std::string& outputFile,
                                       int creationFlags)
{
    std::ios::openmode mode = std::ios::out;

    if ( (creationFlags & sys::File::TRUNCATE) != sys::File::TRUNCATE ||
            (creationFlags & std::ios::app) == std::ios::app)
        mode |= std::ios::app;

    open(outputFile.c_str(), mode);
}

bool io::FileOutputStreamIOS::isOpen()
{
    return mFStream.is_open() && mFStream.good();
}

void io::FileOutputStreamIOS::open(const char *file,
                                std::ios::openmode mode)
{
    mFStream.open(file, mode);
    if (!isOpen())
    {
        throw except::Error(Ctxt(FmtX("File could not be opened: %s", file)));
    }
}

void io::FileOutputStreamIOS::close()
{
    if (!isOpen()) return ;
    mFStream.close();

    if (mFStream.is_open())
    {
        throw except::IOException(Ctxt("File could not be closed"));
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
void io::FileOutputStreamIOS::write(const sys::byte* b, sys::Size_T len)
{
    //std::string s((const char*)b, len);
    //EVAL(s.c_str());
    mFStream.write((const char*)b, len);
}


sys::Off_T io::FileOutputStreamIOS::seek(sys::Off_T offset,
                                         io::Seekable::Whence whence)
{
    std::ios::seekdir dir;
    switch (whence)
    {
    case io::Seekable::START:
        dir = std::ios::beg;
        break;
    case io::Seekable::END:
        dir = std::ios::end;
        break;
    default:
        dir = std::ios::cur;
        break;
    }

    mFStream.seekp(offset, dir);
    return tell();
}
    
sys::Off_T io::FileOutputStreamIOS::tell()
{
    return mFStream.tellp();
}

#endif
