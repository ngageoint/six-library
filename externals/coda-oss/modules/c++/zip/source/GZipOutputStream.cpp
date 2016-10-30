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

#include "zip/GZipOutputStream.h"

using namespace zip;

GZipOutputStream::GZipOutputStream(std::string file)
{
    mFile = gzopen(file.c_str(), "wb");
    if (mFile == NULL)
        throw except::IOException(Ctxt(FmtX("Failed to open gzip stream [%s]",
                file.c_str())));

}

void GZipOutputStream::write(const sys::byte* b, sys::Size_T len)
{
    sys::Size_T written = 0;
    int rv = 0;
    do
    {
        rv = gzwrite(mFile, (const voidp)&(b[written]), len - written);
        if (rv < 0)
        {
            std::string err(gzerror(mFile, &rv));
            throw except::Exception(Ctxt(err));
        }
        if (!rv)
            break;
        written += rv;
    }
    while (written < len);

}

void GZipOutputStream::close()
{
    gzclose( mFile);
    mFile = NULL;
}


