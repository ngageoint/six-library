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

#include "io/FileInputStreamIOS.h"

#if defined(USE_IO_STREAMS)

io::FileInputStreamIOS::FileInputStreamIOS(const char* inputFile,
                                     std::ios::openmode mode)
{
    open(inputFile, mode);
}

io::FileInputStreamIOS::FileInputStreamIOS(const std::string& inputFile,
                                     std::ios::openmode mode)
{
    open(inputFile.c_str(), mode);
}

/*!
 * Returns the number of bytes that can be read
 * without blocking by the next caller of a method for this input
 * 
 * \throw except::IOException
 * \return number of bytes which are readable
 * 
 */
sys::Off_T io::FileInputStreamIOS::available()
{
    sys::Off_T where = mFStream.tellg();
    mFStream.seekg(0, std::ios::end);
    sys::Off_T until = mFStream.tellg();
    mFStream.seekg(where, std::ios::beg);
    return (until - where);
}

sys::Off_T io::FileInputStreamIOS::tell()
{
    return mFStream.tellg();
}

sys::Off_T io::FileInputStreamIOS::seek(sys::Off_T offset, Whence whence)
{
    std::ios::seekdir flags;
    switch (whence)
    {
        case START:
            flags = std::ios::beg;
            break;

        case END:
            flags = std::ios::end;
            break;

        default:
            flags = std::ios::cur;

    }

    mFStream.seekg(offset, flags);
    return tell();
}


bool io::FileInputStreamIOS::isOpen()
{
    return mFStream.is_open() && mFStream.good();
}
void io::FileInputStreamIOS::open(const char *file,
                               std::ios::openmode mode)
{
    mFStream.open(file, mode);
    if (!isOpen())
        throw except::IOException(Ctxt(
                                      FmtX(
                                          "File %s could not be opened for reading",
                                          file)
                                  )
                                 );
}
void io::FileInputStreamIOS::close()
{
    mFStream.close();
}

sys::SSize_T io::FileInputStreamIOS::read(sys::byte* b, sys::Size_T len)
{
    ::memset(b, 0, len);
    sys::Off_T avail = available();
    if (mFStream.eof() || avail <= 0) return io::InputStream::IS_EOF;
    if (len < (sys::Size_T)avail)
        avail = len;

    if (avail > 0)
    {
        sys::SSize_T bytesRead(0);
        // There is a 'huge-gantic' bug in Forte 6.2
        // in which 'read()' reads the first character twice
#if defined(__SUNPRO_CC) && (__SUNPRO_CC == 0x530)

        while (bytesRead < avail && mFStream.good())
        {
            b[bytesRead++] = mFStream.get();
        }
#else
        mFStream.read((char *)b, avail);
        bytesRead = mFStream.gcount();
#endif
        return bytesRead;
    }

    return 0;
}

#endif
