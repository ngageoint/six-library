/* =========================================================================
 * This file is part of zip-c++
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * zip-c++ is free software; you can redistribute it and/or modify
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

#include "zip/GZipInputStream.h"

using namespace zip;

GZipInputStream::GZipInputStream(const std::string& file)
{
    mFile = gzopen(file.c_str(), "rb");
    if (mFile == NULL)
    {
        throw except::IOException(Ctxt(
                "Failed to open gzip stream [" + file + "]"));
    }
}

void GZipInputStream::close()
{
    gzclose( mFile);
    mFile = NULL;
}

sys::SSize_T GZipInputStream::readImpl(void* buffer, size_t len)
{
    auto rv = gzread(mFile, buffer, static_cast<unsigned int>(len));
    if (rv == -1)
    {
        const std::string err(gzerror(mFile, &rv));
        throw except::IOException(Ctxt(err));
    }
    else if (rv == 0)
        return io::InputStream::IS_EOF;

    return (sys::SSize_T) rv;
}
