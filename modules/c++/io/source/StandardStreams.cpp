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

#include "io/StandardStreams.h"

_STDERR_DEFINE_MUTEX_SEMICOLON_
_STDOUT_DEFINE_MUTEX_SEMICOLON_
void io::StandardOutStream::write(const sys::byte* b, sys::Size_T len)
{
    _STDSTREAM_BEGIN_CS_SEMICOLON_
    std::cout.write((const char*)b, len);
    _STDSTREAM_END_CS_SEMICOLON_
    //int returnVal = fwrite(b, len, len, stdout);
    if (!std::cout.good())
        throw except::IOException(
            Ctxt(
                FmtX("std::cout stream is bad after requested write: (%d)",
                     len)) );
}

void io::StandardOutStream::flush()
{
    _STDSTREAM_BEGIN_CS_SEMICOLON_
    std::cout.flush();
    _STDSTREAM_END_CS_SEMICOLON_
}

void io::StandardErrStream::write(const sys::byte* b, sys::Size_T len)
{
    _STDSTREAM_BEGIN_CS_SEMICOLON_
    std::cerr.write((const char*)b, len);
    //int returnVal = fwrite(b, len, len, stderr);
    _STDSTREAM_END_CS_SEMICOLON_
    if (!std::cerr.good())
        throw except::IOException(
            Ctxt(
                FmtX("std::cerr stream is bad after requested write: (%d)",
                     len) ) );
}

void io::StandardErrStream::flush()
{
    _STDSTREAM_BEGIN_CS_SEMICOLON_
    std::cerr.flush();
    _STDSTREAM_END_CS_SEMICOLON_
}
