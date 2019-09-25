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
#ifndef __CPHD_BASE_FILE_HEADER_H__
#define __CPHD_BASE_FILE_HEADER_H__

#include <string>
#include <ostream>
#include <utility>

#include <io/SeekableStreams.h>

namespace cphd
{
class BaseFileHeader
{
public:
    static const char FILE_TYPE[];
    static const char KVP_DELIMITER[];
    static const char LINE_TERMINATOR;
    static const char SECTION_TERMINATOR;
    static const size_t MAX_HEADER_SIZE;

    virtual ~BaseFileHeader()
    {
    }

    static
    std::string readVersion(io::SeekableInputStream& inStream);

    virtual void read(io::SeekableInputStream& inStream) = 0;

    // Convert header info to string (for writing to file)
    // Does not include section terminator string
    virtual std::string toString() const = 0;

    // Returns header file size in bytes, not including the section terminator
    size_t size() const
    {
        return toString().size();
    }

protected:
    static
    bool isCPHD(io::SeekableInputStream& inStream);

    typedef std::pair<std::string, std::string> KeyValuePair;

    static
    void tokenize(const std::string& in,
                  const std::string& delimiter,
                  KeyValuePair& kvPair);

    static
    KeyValuePair tokenize(const std::string& in, const std::string& delimiter)
    {
        KeyValuePair kvPair;
        tokenize(in, delimiter, kvPair);
        return kvPair;
    }

    void blockReadHeader(io::SeekableInputStream& inStream,
                         size_t blockSize,
                         std::string& headerBlock);
};
}

#endif
