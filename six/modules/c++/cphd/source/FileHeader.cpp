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
#include <string.h>
#include <sstream>
#include <map>
#include <stdexcept>

#include <nitf/coda-oss.hpp>
#include <mem/ScopedArray.h>
#include <except/Exception.h>
#include <str/Manip.h>
#include <cphd/FileHeader.h>
#include <cphd/Types.h>

namespace cphd
{

Version FileHeader::defaultVersion = Version::v1_0_1;
Version FileHeader::getDefaultVersion()
{
    return defaultVersion;
}
void FileHeader::setDefaultVersion(Version version)
{
    defaultVersion = version;
}
static const char* getDefaultVersion_()
{
    static std::string strDefaultVersion; // returning a pointer
    strDefaultVersion = to_string(FileHeader::getDefaultVersion());
    return strDefaultVersion.c_str();
}

const char* FileHeader::DEFAULT_VERSION = getDefaultVersion_();
FileHeader::FileHeader(Version version) : mVersion(version)
{
    // reinitialize in case value has changed
    DEFAULT_VERSION = getDefaultVersion_();
}

FileHeader FileHeader::read(io::SeekableInputStream& inStream)
{
    if (!isCPHD(inStream))
    {
        throw except::Exception(Ctxt("Not a CPHD file"));
    }

    // Read mVersion first
    FileHeader retval(readVersion(inStream));
    retval.readAfterValidVersion(inStream);
    return retval;
}
void FileHeader::readImpl(io::SeekableInputStream&)
{
    throw std::logic_error("Should use 'static' read()");
}
void FileHeader::readAfterValidVersion(io::SeekableInputStream& inStream)
{
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

            if (headerEntry.first == "XML_BLOCK_SIZE")
            {
                mXmlBlockSize = str::toType<int64_t>(headerEntry.second);
            }
            else if (headerEntry.first == "XML_BLOCK_BYTE_OFFSET")
            {
                mXmlBlockByteOffset =
                        str::toType<int64_t>(headerEntry.second);
            }
            else if (headerEntry.first == "SUPPORT_BLOCK_SIZE")
            {
               mSupportBlockSize = str::toType<int64_t>(headerEntry.second);
            }
            else if (headerEntry.first == "SUPPORT_BLOCK_BYTE_OFFSET")
            {
               mSupportBlockByteOffset =
                    str::toType<int64_t>(headerEntry.second);
            }
            else if (headerEntry.first == "PVP_BLOCK_SIZE")
            {
               mPvpBlockSize = str::toType<int64_t>(headerEntry.second);
            }
            else if (headerEntry.first == "PVP_BLOCK_BYTE_OFFSET")
            {
               mPvpBlockByteOffset =
                    str::toType<int64_t>(headerEntry.second);
            }
            else if (headerEntry.first == "SIGNAL_BLOCK_SIZE")
            {
                mSignalBlockSize = str::toType<int64_t>(headerEntry.second);
            }
            else if (headerEntry.first == "SIGNAL_BLOCK_BYTE_OFFSET")
            {
                mSignalBlockByteOffset =
                        str::toType<int64_t>(headerEntry.second);
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

    /*
    "... we found that the six library does a check when reading a CPHD file to ensure that the Release Info field not only exists,
    but that it is not an empty string. The CPHD spec does require that the field exists, but does not seem to prohibit it from being
    an empty string.
    
    We have gotten some vendor CPHDs with an empty field here and have failed reading the file due to this.
    I checked and SARPY is able to read (it must not implement the same check)."
    */
    //if (mReleaseInfo.empty())
    //{
    //    throw except::Exception(Ctxt("CPHD header information is incomplete"));
    //}

    // check for any required values that are uninitialized
    if (mXmlBlockSize == 0  ||
        mXmlBlockByteOffset == 0  ||
        mPvpBlockSize == 0  ||
        mPvpBlockByteOffset == 0  ||
        mSignalBlockSize == 0  ||
        mSignalBlockByteOffset == 0 ||
        mClassification.empty())
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
    os << FILE_TYPE << "/" << to_string(mVersion) << LINE_TERMINATOR;

    // Classification fields, if present
    if (mSupportBlockSize > 0)
    {
        os << "SUPPORT_BLOCK_SIZE" << KVP_DELIMITER << mSupportBlockSize
           << LINE_TERMINATOR
           << "SUPPORT_BLOCK_BYTE_OFFSET" << KVP_DELIMITER
           << mSupportBlockByteOffset << LINE_TERMINATOR;
    }

    // Required fields.
    os << "XML_BLOCK_SIZE" << KVP_DELIMITER << mXmlBlockSize << LINE_TERMINATOR
       << "XML_BLOCK_BYTE_OFFSET" << KVP_DELIMITER << mXmlBlockByteOffset
       << LINE_TERMINATOR
       << "PVP_BLOCK_SIZE" << KVP_DELIMITER << mPvpBlockSize << LINE_TERMINATOR
       << "PVP_BLOCK_BYTE_OFFSET" << KVP_DELIMITER << mPvpBlockByteOffset
       << LINE_TERMINATOR
       << "SIGNAL_BLOCK_SIZE" << KVP_DELIMITER << mSignalBlockSize
       << LINE_TERMINATOR
       << "SIGNAL_BLOCK_BYTE_OFFSET" << KVP_DELIMITER << mSignalBlockByteOffset
       << LINE_TERMINATOR
       << "CLASSIFICATION" << KVP_DELIMITER << mClassification
       << LINE_TERMINATOR
       << "RELEASE_INFO" << KVP_DELIMITER << mReleaseInfo << LINE_TERMINATOR
       << SECTION_TERMINATOR << LINE_TERMINATOR;
    return os.str();
}

Version FileHeader::getVersion() const
{
    return mVersion;
}

void FileHeader::setVersion(Version version)
{
    mVersion = version;
}

size_t FileHeader::set(int64_t xmlBlockSize,
                       int64_t supportBlockSize,
                       int64_t pvpBlockSize,
                       int64_t signalBlockSize)
{
    // Resolve all of the offsets based on known sizes.
    setXMLBlockSize(xmlBlockSize);
    setSupportBlockSize(supportBlockSize);
    setPvpBlockSize(pvpBlockSize);
    setSignalBlockSize(signalBlockSize);
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
        int64_t xmlOffset = initialHeaderSize + 2;
        setXMLBlockByteOffset(xmlOffset);


        if (mSupportBlockSize > 0)
        {
            // Add two for the XML section terminator
            int64_t supportOff = getXMLBlockByteOffset() + getXMLBlockSize() + 2;
            setSupportBlockByteOffset(supportOff);

            // Calculate pvp offset based on support position and size
            int64_t pvpOff = getSupportBlockByteOffset() +
                    getSupportBlockSize();
            // Add padding (pvp are doubles)
            const int64_t pvpRemainder = pvpOff % sizeof(double);
            if (pvpRemainder != 0)
            {
                pvpOff += sizeof(double) - pvpRemainder;
            }
            setPvpBlockByteOffset(pvpOff);
        }
        else
        {
            // Add two for the XML section terminator
            int64_t pvpOff = getXMLBlockByteOffset() +
                    getXMLBlockSize() + 2;
            // Add padding (pvp are doubles)
            const int64_t pvpRemainder = pvpOff % sizeof(double);
            if (pvpRemainder != 0)
            {
                pvpOff += sizeof(double) - pvpRemainder;
            }
            setPvpBlockByteOffset(pvpOff);
        }

        // Position to the CPHD, no padding needed, as PVP entries are all
        // doubles
        setSignalBlockByteOffset(getPvpBlockByteOffset() + getPvpBlockSize());

    } while (size() != initialHeaderSize);

    return size();
}

//! Pad bytes don't include the Section terminator
int64_t FileHeader::getPvpPadBytes() const
{
    if (mSupportBlockSize != 0)
    {
        return (getPvpBlockByteOffset() - (getSupportBlockByteOffset() + getSupportBlockSize()));
    }
    return (getPvpBlockByteOffset() - (getXMLBlockByteOffset() + getXMLBlockSize() + 2));
}


std::ostream& operator<< (std::ostream& os, const FileHeader& fh)
{
    os << "FileHeader::\n"
       << "  mVersion               : " << to_string(fh.mVersion) << "\n"
       << "  mXmlBlockSize          : " << fh.mXmlBlockSize << "\n"
       << "  mXmlBlockByteOffset    : " << fh.mXmlBlockByteOffset << "\n"
       << "  mSupportBlockSize      : " << fh.mSupportBlockSize << "\n"
       << "  mSupportBlockSize      : " << fh.mSupportBlockByteOffset << "\n"
       << "  mPvpBlockByteOffset    : " << fh.mPvpBlockSize << "\n"
       << "  mPvpBlockByteOffset    : " << fh.mPvpBlockByteOffset << "\n"
       << "  mSignalBlockSize       : " << fh.mSignalBlockSize << "\n"
       << "  mSignalBlockByteOffset : " << fh.mSignalBlockByteOffset << "\n"
       << "  mClassification: " << fh.mClassification << "\n"
       << "  mReleaseInfo   : " << fh.mReleaseInfo << "\n";
    return os;
}
}
