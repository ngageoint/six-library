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

    void readImpl(io::SeekableInputStream&) override;
    void read(io::SeekableInputStream& inStream)
    {
        readImpl(inStream);
    }

    // Convert header info to string (for writing to file)
    // Does not include section terminator string
    virtual std::string toString() const;

    // Set the file header to its final state
    // Compute the offsets of the sections that depend on the file header size
    // Add in padding to modulo 8 also.
    // Returns size of the header
    size_t set();
    size_t set(int64_t xmlSize, int64_t vbmSize, int64_t cphd03Size);

    // Required elements
    void setXMLsize(int64_t size)
    {
        mXmlDataSize = size;
    }

    void setVBMsize(int64_t size)
    {
        mVbDataSize = size;
    }

    void setCPHDsize(int64_t size)
    {
        mCphdDataSize = size;
    }

    void setXMLoffset(int64_t offset)
    {
        mXmlByteOffset = offset;
    }

    void setVBMoffset(int64_t offset)
    {
        mVbByteOffset = offset;
    }

    void setCPHDoffset(int64_t offset)
    {
        mCphdByteOffset = offset;
    }

    int64_t getXMLsize() const
    {
        return mXmlDataSize;
    }

    int64_t getVBMsize() const
    {
        return mVbDataSize;
    }

    int64_t getCPHDsize() const
    {
        return mCphdDataSize;
    }

    int64_t getXMLoffset() const
    {
        return mXmlByteOffset;
    }

    int64_t getVBMoffset() const
    {
        return mVbByteOffset;
    }

    int64_t getCPHDoffset() const
    {
        return mCphdByteOffset;
    }

    // Pad bytes don't include the Section terminator
    int64_t getPadBytes() const
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
    int64_t mXmlDataSize;
    int64_t mXmlByteOffset;
    int64_t mVbDataSize;
    int64_t mVbByteOffset;
    int64_t mCphdDataSize;
    int64_t mCphdByteOffset;

    // Optional key-value pairs
    std::string mClassification;
    std::string mReleaseInfo;
};
}

#endif

