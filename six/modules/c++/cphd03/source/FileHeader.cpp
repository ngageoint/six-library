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

#include <string.h>
#include <sstream>

#include <sys/Conf.h>
#include <mem/ScopedArray.h>
#include <except/Exception.h>
#include <str/Manip.h>
#include <cphd03/FileHeader.h>

namespace cphd03
{
const char FileHeader::FILE_TYPE[] = "CPHD";
const char FileHeader::DEFAULT_VERSION[] = "0.3";
const char FileHeader::KVP_DELIMITER[] = " := ";
const char FileHeader::LINE_TERMINATOR = '\n';
const char FileHeader::SECTION_TERMINATOR = '\f';
const size_t FileHeader::MAX_HEADER_SIZE = 10485760;

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

bool FileHeader::isCPHD(io::SeekableInputStream& inStream)
{
    inStream.seek(0, io::Seekable::START);

    char buf[4] = {'\0', '\0', '\0', '\0'};
    inStream.read(buf, 4);

    return (::strncmp(buf, FILE_TYPE, 4) == 0);
}

std::string FileHeader::readVersion(io::SeekableInputStream& inStream)
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

void FileHeader::tokenize(const std::string& in,
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

void FileHeader::blockReadHeader(io::SeekableInputStream& inStream,
                                 size_t blockSize,
                                 std::string& headerBlock)
{
    static const char ERROR_MSG[] = "CPHD file malformed: Header must terminate with '\\f\\n'";

    mem::ScopedArray<sys::byte> buf(new sys::byte[blockSize + 1]);
    std::fill_n(buf.get(), blockSize + 1, 0);
    headerBlock.clear();

    // read each block in succession
    size_t terminatorLoc = std::string::npos;
    size_t totalBytesRead = 0;
    while (inStream.read(buf.get(), blockSize) != io::InputStream::IS_EOF &&
           terminatorLoc == std::string::npos)
    {
        std::string thisBlock = buf.get();

        // find the terminator in the block
        terminatorLoc = thisBlock.find('\f');
        if (terminatorLoc != std::string::npos)
        {
            // read one more byte if our block missed the trailing '\n'
            if (terminatorLoc == thisBlock.length() - 1)
            {
                sys::byte c(0);
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
                mXmlDataSize = str::toType<sys::Off_T>(headerEntry.second);
            }
            else if (headerEntry.first == "XML_BYTE_OFFSET")
            {
                mXmlByteOffset = str::toType<sys::Off_T>(headerEntry.second);
            }
            else if (headerEntry.first == "VB_DATA_SIZE")
            {
               mVbDataSize = str::toType<sys::Off_T>(headerEntry.second);
            }
            else if (headerEntry.first == "VB_BYTE_OFFSET")
            {
               mVbByteOffset = str::toType<sys::Off_T>(headerEntry.second);
            }
            else if (headerEntry.first == "CPHD03_DATA_SIZE")
            {
               mCphdDataSize = str::toType<sys::Off_T>(headerEntry.second);
            }
            else if (headerEntry.first == "CPHD03_BYTE_OFFSET")
            {
               mCphdByteOffset = str::toType<sys::Off_T>(headerEntry.second);
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
       << "CPHD03_DATA_SIZE" << KVP_DELIMITER << mCphdDataSize << LINE_TERMINATOR
       << "CPHD03_BYTE_OFFSET" << KVP_DELIMITER << mCphdByteOffset
       << LINE_TERMINATOR;

    return os.str();
}

size_t FileHeader::set(sys::Off_T xmlSize,
                       sys::Off_T vbmSize,
                       sys::Off_T cphd03Size)
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
        sys::Off_T vbmOffs = getXMLoffset() + getXMLsize() + 2;

        // Add padding (VBMs are doubles)
        const sys::Off_T remainder = vbmOffs % sizeof(double);
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
