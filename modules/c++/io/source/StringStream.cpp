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

#include "io/StringStream.h"

sys::Off_T io::StringStream::available()
{
    return mStream.available();
}

void io::StringStream::write(const sys::byte* data, sys::Size_T size)
{
    mStream.write(data, size);
}

void io::StringStream::write(const std::string& s)
{
    mStream.write((sys::byte*)s.c_str(), (sys::Size_T)s.length());
}

sys::SSize_T io::StringStream::read(sys::byte *data, sys::Size_T size)
{
    return mStream.read(data, size);
}
