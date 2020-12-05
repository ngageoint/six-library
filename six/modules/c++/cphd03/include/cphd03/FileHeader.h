/* =========================================================================
 * This file is part of cphd03-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * cphd03-c++ is free software; you can redistribute it and/or modify
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

#ifndef __CPHD03_FILE_HEADER_H__
#define __CPHD03_FILE_HEADER_H__

#include <string>
#include <ostream>
#include <utility>

#include <io/SeekableStreams.h>
#include <cphd/BaseFileHeader.h>

namespace cphd03
{
class FileHeader : public cphd::BaseFileHeader
{
public:
    static const char DEFAULT_VERSION[];

    FileHeader();

    virtual ~FileHeader()
    {
    }

    virtual void read(io::SeekableInputStream& inStream);

    // Convert header info to string (for writing to file)
    // Does not include section terminator string
    virtual std::string toString() const;

    // Set the file header to its final state
    // Compute the offsets of the sections that depend on the file header size
    // Add in padding to modulo 8 also.
    // Returns size of the header
    size_t set();
    size_t set(ptrdiff_t xmlSize, ptrdiff_t vbmSize, ptrdiff_t cphd03Size);

    // Required elements
    void setXMLsize(ptrdiff_t size)
    {
        mXmlDataSize = size;
    }

    void setVBMsize(ptrdiff_t size)
    {
        mVbDataSize = size;
    }

    void setCPHDsize(ptrdiff_t size)
    {
        mCphdDataSize = size;
    }

    void setXMLoffset(ptrdiff_t offset)
    {
        mXmlByteOffset = offset;
    }

    void setVBMoffset(ptrdiff_t offset)
    {
        mVbByteOffset = offset;
    }

    void setCPHDoffset(ptrdiff_t offset)
    {
        mCphdByteOffset = offset;
    }

    ptrdiff_t getXMLsize() const
    {
        return mXmlDataSize;
    }

    ptrdiff_t getVBMsize() const
    {
        return mVbDataSize;
    }

    ptrdiff_t getCPHDsize() const
    {
        return mCphdDataSize;
    }

    ptrdiff_t getXMLoffset() const
    {
        return mXmlByteOffset;
    }

    ptrdiff_t getVBMoffset() const
    {
        return mVbByteOffset;
    }

    ptrdiff_t getCPHDoffset() const
    {
        return mCphdByteOffset;
    }

    // Pad bytes don't include the Section terminator
    ptrdiff_t getPadBytes() const
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
    friend std::ostream& operator<< (std::ostream& os, const FileHeader& fh);

private:
    // File type header
    std::string mVersion;

    // Required key-value pairs
    ptrdiff_t mXmlDataSize;
    ptrdiff_t mXmlByteOffset;
    ptrdiff_t mVbDataSize;
    ptrdiff_t mVbByteOffset;
    ptrdiff_t mCphdDataSize;
    ptrdiff_t mCphdByteOffset;

    // Optional key-value pairs
    std::string mClassification;
    std::string mReleaseInfo;
};
}

#endif

