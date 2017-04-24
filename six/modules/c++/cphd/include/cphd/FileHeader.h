/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

#include <cphd/BaseFileHeader.h>
#include <io/SeekableStreams.h>

namespace cphd
{
class FileHeader : public BaseFileHeader
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
    size_t set(sys::Off_T xmlBlockSize, sys::Off_T supportBlockSize,
            sys::Off_T pvpBlockSize, sys::Off_T signalBlockSize);

    // Required elements
    void setXMLBlockSize(sys::Off_T size)
    {
        mXmlBlockSize = size;
    }

    void setPvpBlockSize(sys::Off_T size)
    {
        mPvpBlockSize = size;
    }

    void setSignalBlockSize(sys::Off_T size)
    {
        mSignalBlockSize = size;
    }

    void setXMLBlockByteOffset(sys::Off_T offset)
    {
        mXmlBlockByteOffset = offset;
    }

    void setPvpBlockByteOffset(sys::Off_T offset)
    {
        mPvpBlockByteOffset = offset;
    }

    void setSignalBlockByteOffset(sys::Off_T offset)
    {
        mSignalBlockByteOffset = offset;
    }

    void setClassification(const std::string& str)
    {
        mClassification = str;
    }

    void setReleaseInfo(const std::string& str)
    {
        mReleaseInfo = str;
    }

    sys::Off_T getXMLBlockSize() const
    {
        return mXmlBlockSize;
    }

    sys::Off_T getPvpBlockSize() const
    {
        return mPvpBlockSize;
    }

    sys::Off_T getSignalBlockSize() const
    {
        return mSignalBlockSize;
    }

    sys::Off_T getXMLBlockByteOffset() const
    {
        return mXmlBlockByteOffset;
    }

    sys::Off_T getPvpBlockByteOffset() const
    {
        return mPvpBlockByteOffset;
    }

    sys::Off_T getSignalBlockByteOffset() const
    {
        return mSignalBlockByteOffset;
    }

    std::string getClassification() const
    {
        return mClassification;
    }

    std::string getReleaseInfo() const
    {
        return mReleaseInfo;
    }

    // Pad bytes don't include the Section terminator
    // TODO: How is this used?
    /*
    sys::Off_T getPadBytes() const
    {
        return (getVBMoffset() - (getXMLoffset() + getXMLsize() + 2));
    }
    */

    // Optional elements
    void setSupportBlockSize(sys::Off_T size)
    {
        mSupportBlockSize = size;
    }

    void setSupportBlockByteOffset(sys::Off_T offset)
    {
        mSupportBlockByteOffset = offset;
    }

    sys::Off_T getSupportBlockSize() const
    {
        return mSupportBlockSize;
    }

    sys::Off_T getSupportBlockByteOffset() const
    {
        return mSupportBlockByteOffset;
    }

private:
    friend std::ostream& operator<< (std::ostream& os, const FileHeader& fh);

private:
    // File type header
    std::string mVersion;

    // Required key-value pairs
    sys::Off_T mXmlBlockSize;
    sys::Off_T mXmlBlockByteOffset;
    sys::Off_T mPvpBlockSize;
    sys::Off_T mPvpBlockByteOffset;
    sys::Off_T mSignalBlockSize;
    sys::Off_T mSignalBlockByteOffset;
    std::string mClassification;
    std::string mReleaseInfo;

    // Optional key-value pairs
    sys::Off_T mSupportBlockSize;
    sys::Off_T mSupportBlockByteOffset;
};
}

#endif
