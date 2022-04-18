/* =========================================================================
 * This file is part of io-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

#ifndef CODA_OSS_io_ReadUtils_h_INCLUDED_
#define CODA_OSS_io_ReadUtils_h_INCLUDED_

#include <string>
#include <vector>

#include <sys/Conf.h>
#include <coda_oss/cstddef.h> // byte
#include <sys/filesystem.h>

namespace io
{
/*!
 * Reads the contents of a file (binary or text), putting the raw bytes in
 * 'buffer'.  These are the exact bytes of the file, so text files will not
 * contain a null terminator.
 *
 * \param pathname Pathname of the file to read in
 * \param buffer Raw bytes of the file
 */
void readFileContents(const std::string& pathname,
                      std::vector<sys::byte>& buffer);
void readFileContents(const sys::filesystem::path& pathname, std::vector<coda_oss::byte>& buffer);

/*!
 * Reads the contents of a file into a string.  The file is assumed to be a
 * text file.
 *
 * \param pathname Pathname of the file to read in
 * \param[out] str Contents of the file
 */
void readFileContents(const std::string& pathname, std::string& str);

/*!
 * Reads the contents of a file into a string.  The file is assumed to be a
 * text file.
 *
 * \param pathname Pathname of the file to read in
 *
 * \return Contents of the file
 */
inline
std::string readFileContents(const std::string& pathname)
{
    std::string str;
    readFileContents(pathname, str);
    return str;
}
}

#endif  // CODA_OSS_io_ReadUtils_h_INCLUDED_
