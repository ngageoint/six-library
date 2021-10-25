/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#include "six/WriteControl.h"

const char six::WriteControl::OPT_BYTE_SWAP[] = "ByteSwap";
const char six::WriteControl::OPT_BUFFER_SIZE[] = "BufferSize";

static six::BufferList convertBufferList(const six::buffer_list& buffers)
{
    six::BufferList retval;
    for (const auto& buffer : buffers)
    {
        const void* buffer_ = buffer.data();
        retval.push_back(static_cast<six::BufferList::value_type>(buffer_));
    }
    return retval;
}

void six::WriteControl::save(const buffer_list& sources, const std::filesystem::path& toFile,
    const std::vector<std::filesystem::path>& schemaPaths)
{
    // This is only for backwards compatibility, to avoid breaking existing code.
    //  Derived classes SHOULD override save() and use the std::span<>s in buffer_list.
    std::vector<std::string> schemaPaths_;
    std::transform(schemaPaths.begin(), schemaPaths.end(), std::back_inserter(schemaPaths_),
        [](const std::filesystem::path& p) { return p.string(); });
    save(::convertBufferList(sources), toFile.string(), schemaPaths_);
}