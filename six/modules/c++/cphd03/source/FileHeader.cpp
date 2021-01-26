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
#include <cphd03/FileHeader.h>

#include <string.h>
#include <sstream>

#include <nitf/coda-oss.hpp>
#include <mem/ScopedArray.h>
#include <except/Exception.h>
#include <str/Manip.h>

namespace cphd03
{
const char FileHeader::DEFAULT_VERSION[] = "0.3";

FileHeader::FileHeader() :
    mVersion(DEFAULT_VERSION),
    mXmlDataSize(0),
    mXmlByteOffset(0),
    mVbDataSize(0),
    mVbByteOffset(0),
    mCphdDataSize(0),
    mCphdByteOffset(0)
{
}

void FileHeader::read(io::SeekableInputStream& inStream)
{
    if (!isCPHD(inStream))
    {
        throw except::Exception(Ctxt("Not a CPHD file"));
    }

    // Read mVersion first
    mVersion = readVersion(inStream);

    // Block read the header for more efficient IO
    KeyValuePair headerEntry;
    std::string headerBlock;
    blockReadHeader(inStream, 1024, headerBlock);

    // Read each line with its tokens
    if (!headerBlock.empty())
    {
        std::vector<std::string> headerLines = str::split(headerBlock, "\n");
        for (size_t ii = 0; ii < headerLines.size(); ++ii)
        {
            // Determine our header entry
            tokenize(headerLines[ii], KVP_DELIMITER, headerEntry);
            if (headerEntry.first == "XML_DATA_SIZE")
            {
                mXmlDataSize = str::toType<int64_t>(headerEntry.second);
            }
            else if (headerEntry.first == "XML_BYTE_OFFSET")
            {
                mXmlByteOffset = str::toType<int64_t>(headerEntry.second);
            }
            else if (headerEntry.first == "VB_DATA_SIZE")
            {
               mVbDataSize = str::toType<int64_t>(headerEntry.second);
            }
            else if (headerEntry.first == "VB_BYTE_OFFSET")
            {
               mVbByteOffset = str::toType<int64_t>(headerEntry.second);
            }
            else if (headerEntry.first == "CPHD_DATA_SIZE")
            {
               mCphdDataSize = str::toType<int64_t>(headerEntry.second);
            }
            else if (headerEntry.first == "CPHD_BYTE_OFFSET")
            {
               mCphdByteOffset = str::toType<int64_t>(headerEntry.second);
            }
            else if (headerEntry.first == "CLASSIFICATION")
            {
                mClassification = headerEntry.second;
            }
            else if (headerEntry.first == "RELEASE_INFO")
            {
                mReleaseInfo = headerEntry.second;
            }
            else
            {
                throw except::Exception(Ctxt(
                        "Invalid CPHD header entry '" + headerEntry.first));
            }
        }
    }

    // check for any required values that are uninitialized
    if (mXmlDataSize == 0  ||
        mXmlByteOffset == 0  ||
        mVbDataSize == 0  ||
        mVbByteOffset == 0  ||
        mCphdDataSize == 0  ||
        mCphdByteOffset == 0)
    {
        throw except::Exception(Ctxt("CPHD header information is incomplete"));
    }
}

// Does not include the Section Terminator
std::string FileHeader::toString() const
{
    std::ostringstream os;

    // Send the values as they are, no calculating

    // File type
    os << FILE_TYPE << "/" << mVersion << LINE_TERMINATOR;

    // Classification fields, if present
    if (!mClassification.empty())
    {
        os << "CLASSIFICATION" << KVP_DELIMITER << mClassification
           << LINE_TERMINATOR;
    }

    if (!mReleaseInfo.empty())
    {
        os << "RELEASE_INFO" << KVP_DELIMITER << mReleaseInfo
           << LINE_TERMINATOR;
    }

    // Required fields.
    os << "XML_DATA_SIZE" << KVP_DELIMITER << mXmlDataSize << LINE_TERMINATOR
       << "XML_BYTE_OFFSET" << KVP_DELIMITER << mXmlByteOffset
       << LINE_TERMINATOR
       << "VB_DATA_SIZE" << KVP_DELIMITER << mVbDataSize << LINE_TERMINATOR
       << "VB_BYTE_OFFSET" << KVP_DELIMITER << mVbByteOffset << LINE_TERMINATOR
       << "CPHD_DATA_SIZE" << KVP_DELIMITER << mCphdDataSize << LINE_TERMINATOR
       << "CPHD_BYTE_OFFSET" << KVP_DELIMITER << mCphdByteOffset
       << LINE_TERMINATOR;

    return os.str();
}

size_t FileHeader::set(int64_t xmlSize,
                       int64_t vbmSize,
                       int64_t cphd03Size)
{
    // Resolve all of the offsets based on known sizes.
    setXMLsize(xmlSize);
    setVBMsize(vbmSize);
    setCPHDsize(cphd03Size);

    return set();
}

size_t FileHeader::set()
{
    // Compute the three offsets that depend on a stable header size
    // loop until header size doesn't change
    size_t initialHeaderSize;
    do
    {
        initialHeaderSize = size();

        // Add the header section terminator, not part of the header size
        setXMLoffset(initialHeaderSize + 2);

        // Add two for the XML section terminator
        int64_t vbmOffs = getXMLoffset() + getXMLsize() + 2;

        // Add padding (VBMs are doubles)
        const int64_t remainder = vbmOffs % sizeof(double);
        if (remainder != 0)
        {
            vbmOffs += sizeof(double) - remainder;
        }

        setVBMoffset(vbmOffs);

        // Position to the CPHD, no padding needed, as VBM entries are all
        // doubles
        setCPHDoffset (getVBMoffset() + getVBMsize());

    } while (size() != initialHeaderSize);

    return size();
}

std::ostream& operator<< (std::ostream& os, const FileHeader& fh)
{
    os << "FileHeader::\n"
       << "  mVersion       : " << fh.mVersion << "\n"
       << "  mXmlDataSize   : " << fh.mXmlDataSize << "\n"
       << "  mXmlByteOffset : " << fh.mXmlByteOffset << "\n"
       << "  mVbDataSize    : " << fh.mVbDataSize << "\n"
       << "  mVbByteOffset  : " << fh.mVbByteOffset << "\n"
       << "  mCphdDataSize  : " << fh.mCphdDataSize << "\n"
       << "  mCphdByteOffset: " << fh.mCphdByteOffset << "\n"
       << "  mClassification: " << fh.mClassification << "\n"
       << "  mReleaseInfo   : " << fh.mReleaseInfo << "\n";
    return os;
}
}
