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

#include <io/FileInputStream.h>
#include <coda_oss/span.h>

namespace io
{
template<typename TPath, typename T>
void readFileContents_(const TPath& pathname, std::vector<T>& buffer)
{
    io::FileInputStream inStream(pathname);
    buffer.resize(inStream.available());
    if (!buffer.empty())
    {
        inStream.read(coda_oss::span<T>(buffer.data(), buffer.size()), true);
    }
}
void readFileContents(const std::string& pathname,
                      std::vector<sys::byte>& buffer)
{
    readFileContents_(pathname, buffer);
}
void readFileContents(const sys::filesystem::path& pathname, std::vector<coda_oss::byte>& buffer)
{
    readFileContents_(pathname, buffer);
}

void readFileContents(const std::string& pathname, std::string& str)
{
    std::vector<sys::byte> buffer;
    readFileContents(pathname, buffer);

    if (buffer.empty())
    {
        str.clear();
    }
    else
    {
        str.assign(reinterpret_cast<char*>(&buffer[0]), buffer.size());
    }
}
}
