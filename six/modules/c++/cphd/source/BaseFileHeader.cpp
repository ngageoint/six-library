/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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
#include <cphd/BaseFileHeader.h>

#include <vector>

#include <mem/ScopedArray.h>
#include <io/SeekableStreams.h>
#include <str/Manip.h>
#include <nitf/coda-oss.hpp>

namespace cphd
{
const char BaseFileHeader::FILE_TYPE[] = "CPHD";
const char BaseFileHeader::KVP_DELIMITER[] = " := ";
const char BaseFileHeader::LINE_TERMINATOR = '\n';
const char BaseFileHeader::SECTION_TERMINATOR = '\f';
const size_t BaseFileHeader::MAX_HEADER_SIZE = 10485760;

bool BaseFileHeader::isCPHD(io::SeekableInputStream& inStream)
{
    inStream.seek(0, io::Seekable::START);

    char buf[4] = {'\0', '\0', '\0', '\0'};
    inStream.read(buf, 4);

    return (::strncmp(buf, FILE_TYPE, 4) == 0);
}

std::string BaseFileHeader::strReadVersion(io::SeekableInputStream& inStream)
{
    char buf[128];
    inStream.seek(0, io::Seekable::START);
    inStream.readln(buf, sizeof(buf));

    const KeyValuePair kvPair(tokenize(buf, "/"));
    if (kvPair.first != FILE_TYPE)
    {
        throw except::Exception(Ctxt("Not a CPHD file"));
    }

    // Remove any trailing whitespace from the version
    std::string ret = kvPair.second;
    str::trim(ret);

    return ret;
}
Version BaseFileHeader::readVersion(io::SeekableInputStream& inStream)
{
    return toVersion(strReadVersion(inStream));
}
Version BaseFileHeader::toVersion(const std::string& strVersion)
{
    #define SIX_cphd_FileHeader_setVersion_map_entry(v) { to_string(v),  v} // avoid copy/paste errors
    static const std::map<std::string, Version> string_to_vesion
    {
        { "1.0",  Version::v1_0_0},  // existing files; should be "1.0.0"
        SIX_cphd_FileHeader_setVersion_map_entry(Version::v1_0_0),
        SIX_cphd_FileHeader_setVersion_map_entry(Version::v1_0_1),
        SIX_cphd_FileHeader_setVersion_map_entry(Version::v1_1_0),
    };
    #undef SIX_cphd_FileHeader_setVersion_map_entry
    const auto it = string_to_vesion.find(strVersion);
    if (it != string_to_vesion.end())
    {
        return it->second;
    }
    throw std::logic_error("Unkown 'Version' value.");
}

void BaseFileHeader::tokenize(const std::string& in,
                              const std::string& delimiter,
                              KeyValuePair& kvPair)
{
    const std::string::size_type pos = in.find(delimiter);
    if (pos == std::string::npos)
    {
        throw except::Exception(Ctxt(
                "Delimiter '" + delimiter + "' not found in '" + in + "'"));
    }

    kvPair.first = in.substr(0, pos);
    kvPair.second = in.substr(pos + delimiter.length());
}

void BaseFileHeader::blockReadHeader(io::SeekableInputStream& inStream,
                                     size_t blockSize,
                                     std::string& headerBlock)
{
    static const char ERROR_MSG[] =
            "CPHD file malformed: Header must terminate with '\\f\\n'";

    std::vector<std::byte> buf(blockSize + 1, static_cast<std::byte>(0));
    headerBlock.clear();

    // read each block in succession
    size_t terminatorLoc = std::string::npos;
    size_t totalBytesRead = 0;
    while (inStream.read(buf.data(), blockSize) != io::InputStream::IS_EOF &&
           terminatorLoc == std::string::npos)
    {
        std::string thisBlock = reinterpret_cast<const char*>(buf.data());

        // find the terminator in the block
        terminatorLoc = thisBlock.find('\f');
        if (terminatorLoc != std::string::npos)
        {
            // read one more byte if our block missed the trailing '\n'
            if (terminatorLoc == thisBlock.length() - 1)
            {
                char c;
                inStream.read(&c, 1);

                thisBlock.insert(thisBlock.length(), &c, 1);
            }

            // verify proper formatting
            if (thisBlock[terminatorLoc + 1] != '\n')
            {
                throw except::Exception(Ctxt(ERROR_MSG));
            }

            // trim off anything after the terminator
            thisBlock = thisBlock.substr(0, terminatorLoc);
        }

        // build the header
        headerBlock += thisBlock;

        // verify we aren't past 10MB in the header --
        // this stops processing before we start reading into the
        // image data and potentially run out of memory
        totalBytesRead += thisBlock.length();
        if (totalBytesRead > MAX_HEADER_SIZE)
        {
            throw except::Exception(Ctxt(ERROR_MSG));
        }
    }
}
}

