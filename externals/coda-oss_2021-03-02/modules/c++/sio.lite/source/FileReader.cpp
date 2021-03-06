/* =========================================================================
 * This file is part of sio.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sio.lite-c++ is free software; you can redistribute it and/or modify
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
#include "sio/lite/FileReader.h"

sys::Off_T sio::lite::FileReader::seek( sys::Off_T offset, Whence whence )
{
    if (whence == START)
        offset = offset + headerLength;
    
    ( (io::FileInputStream*)inputStream )->seek(offset, whence);
    return this->tell();
}

sys::Off_T sio::lite::FileReader::tell()
{
    return ( (io::FileInputStream*)inputStream )->tell() - headerLength;
}

void sio::lite::FileReader::killStream()
{
    if (inputStream && own)
    {
        io::FileInputStream* file = (io::FileInputStream*)inputStream;
        if (file->isOpen()) file->close();
        delete inputStream;
    }
}

