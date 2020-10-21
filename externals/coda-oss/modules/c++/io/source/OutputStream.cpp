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

#include "io/OutputStream.h"

#include <string>
#include <stdexcept>

#include <sys/String.h>

static sys::u8string convert(const std::string& str, io::TextEncoding encoding)
{
    if (encoding == io::TextEncoding::Utf8)
    {
        return str::toUtf8(str);
    }

    throw std::invalid_argument("Unexpected 'encoding' value.");
}

template<typename T>
inline static void write(io::OutputStream& stream, const T& str)
{
    const auto buffer = reinterpret_cast<const sys::byte*>(str.c_str());
    const sys::Size_T len{str.length()};
    stream.write(buffer, len);
}
void io::OutputStream::write(const std::string& str)
{
    if (pEncoding == nullptr)
    {
        ::write(*this, str);
    }
    else
    {
        const auto u8str = convert(str, *pEncoding);
        ::write(*this, u8str);
    }
}
