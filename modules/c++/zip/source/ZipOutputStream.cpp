/* =========================================================================
 * This file is part of zip-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <zip/ZipOutputStream.h>
#include <io/FileInputStream.h>
#include <except/Exception.h>

namespace zip
{
ZipOutputStream::ZipOutputStream(const std::string& pathname)
{
    mZip = zipOpen64(pathname.c_str(), APPEND_STATUS_CREATE);
    if (mZip == nullptr)
        throw except::IOException(Ctxt("Failed to open zip stream " + 
                pathname));

}

void ZipOutputStream::createFileInZip(const std::string& pathname,
                                      const std::string& comment,
                                      const std::string& password)
{
    zip_fileinfo zipFileInfo;

    memset(&zipFileInfo, 0, sizeof(zipFileInfo));

    // Add the file
    sys::Int32_T results = zipOpenNewFileInZip3_64(
            mZip,
            pathname.c_str(),
            &zipFileInfo,
            nullptr,
            0,
            nullptr,
            0,
            comment.empty() ? nullptr : comment.c_str(),
            Z_DEFLATED,
            Z_DEFAULT_COMPRESSION,
            0,
            -MAX_WBITS,
            DEF_MEM_LEVEL,
            Z_DEFAULT_STRATEGY,
            password.empty() ? nullptr : password.c_str(),
            0,
            0);

    if (results != Z_OK)
         throw except::IOException(Ctxt("Failed to create file " + 
                pathname));
}

void ZipOutputStream::closeFileInZip()
{
    sys::Int32_T results = zipCloseFileInZip(mZip);
    if (results != Z_OK)
         throw except::IOException(Ctxt("Failed to close file at zip location."));
}

void ZipOutputStream::write(const std::string& inputPathname,
                            const std::string& zipPathname)
{
    io::FileInputStream input(inputPathname);

    createFileInZip(zipPathname);
    input.streamTo(*this);
    closeFileInZip();
}

void ZipOutputStream::write(const void* buffer, size_t len)
{
    // Write the contents to the location
    const auto results = zipWriteInFileInZip(mZip, buffer, static_cast<unsigned int>(len));

    if (results != Z_OK)
         throw except::IOException(Ctxt("Failed to write file to zip location."));
}

void ZipOutputStream::close()
{
    sys::Int32_T results = zipClose(mZip, nullptr);
    if (results != Z_OK)
        throw except::IOException(Ctxt("Failed to save zip file."));
}
}
