/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifndef __CPHD_FILE_HEADER_H__
#define __CPHD_FILE_HEADER_H__

#include <string>
#include <ostream>
#include <utility>

#include <io/SeekableStreams.h>

namespace cphd
{
class FileHeader
{
public:
    static const char FILE_TYPE[];
    static const char DEFAULT_VERSION[];
    static const char KVP_DELIMITER[];
    static const char LINE_TERMINATOR;
    static const char SECTION_TERMINATOR;
    static const size_t MAX_HEADER_SIZE;

    FileHeader();

    static
    bool isCPHD(io::SeekableInputStream& inStream);
    
    static
    std::string readVersion(io::SeekableInputStream& inStream);

    void read(io::SeekableInputStream& inStream);

    // Convert header info to string (for writing to file)
    // Does not include section terminator string
    std::string toString() const;

    // Returns header file size in bytes, not including the section terminator
    size_t size() const
    {
        return toString().size();
    }

    // Set the file header to its final state
    // Compute the offsets of the sections that depend on the file header size
    // Add in padding to modulo 8 also.
    // Returns size of the header
    size_t set();
    size_t set(sys::Off_T xmlSize, sys::Off_T vbmSize, sys::Off_T cphdSize);

    // Required elements
    void setXMLsize(sys::Off_T size)
    {
        mXmlDataSize = size;
    }

    void setVBMsize(sys::Off_T size)
    {
        mVbDataSize = size;
    }

    void setCPHDsize(sys::Off_T size)
    {
        mCphdDataSize = size;
    }

    void setXMLoffset(sys::Off_T offset)
    {
        mXmlByteOffset = offset;
    }

    void setVBMoffset(sys::Off_T offset)
    {
        mVbByteOffset = offset;
    }

    void setCPHDoffset(sys::Off_T offset)
    {
        mCphdByteOffset = offset;
    }

    sys::Off_T getXMLsize() const
    {
        return mXmlDataSize;
    }

    sys::Off_T getVBMsize() const
    {
        return mVbDataSize;
    }

    sys::Off_T getCPHDsize() const
    {
        return mCphdDataSize;
    }

    sys::Off_T getXMLoffset() const
    {
        return mXmlByteOffset;
    }

    sys::Off_T getVBMoffset() const
    {
        return mVbByteOffset;
    }

    sys::Off_T getCPHDoffset() const
    {
        return mCphdByteOffset;
    }

    // Pad bytes don't include the Section terminator
    sys::Off_T getPadBytes() const
    {
        return (getVBMoffset() - (getXMLoffset() + getXMLsize() + 2));
    }

    // Optional elements
    void setClassification(const std::string& str)
    {
        mClassification = str;
    }

    void setReleaseInfo(const std::string& str)
    {
        mReleaseInfo = str;
    }

    std::string getClassification() const
    {
        return mClassification;
    }

    std::string getReleaseInfo() const
    {
        return mReleaseInfo;
    }
   
private:
    typedef std::pair<std::string, std::string> KeyValuePair;

    friend std::ostream& operator<< (std::ostream& os, const FileHeader& fh);

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

private:
    // File type header
    std::string mVersion;

    // Required key-value pairs
    sys::Off_T mXmlDataSize;
    sys::Off_T mXmlByteOffset;
    sys::Off_T mVbDataSize;
    sys::Off_T mVbByteOffset;
    sys::Off_T mCphdDataSize;
    sys::Off_T mCphdByteOffset;

    // Optional key-value pairs
    std::string mClassification;
    std::string mReleaseInfo;
};
}

#endif
