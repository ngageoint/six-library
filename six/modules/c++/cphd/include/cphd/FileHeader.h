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
#ifndef __CPHD_FILE_HEADER_H__
#define __CPHD_FILE_HEADER_H__

#include <string>
#include <ostream>
#include <utility>

#include <io/SeekableStreams.h>
#include <cphd/BaseFileHeader.h>

namespace cphd
{
/*
 *  \class FileHeader
 *
 *  \brief Stores CPHD file header information
 */
class FileHeader : public BaseFileHeader
{
public:
    static const char DEFAULT_VERSION[];

    /*
     *  \func FileHeader
     *
     *  \brief Constructor
     *   initialize default variables
     */
    FileHeader();

    virtual ~FileHeader()
    {
    }

    /*
     *  \func read
     *
     *  \brief Initialize member variables from input stream
     *
     *  \param inStream Valid input stream of CPHD file
     */
    void read(io::SeekableInputStream& inStream) override;

    /*
     *  \func toString
     *
     *  \brief Convert header info to string
     *
     *  For writing to file
     *  Does not include section terminator string
     *
     *  \return Returns header as string
     */
    std::string toString() const override;

    /*
     *  \func getVersion
     *
     *  \brief Get the file version
     *
     *  Get the version of the CPHD file
     *  ex: 1.0.0, 1.0.1 etc
     *
     * returns file version string
     */
    std::string getVersion() const;

    /*
     *  \func setVersion
     *
     *  \brief Set the file version
     *
     *  Set the version of the CPHD file
     *  ex: 1.0.0, 1.0.1 etc
     *
     */
    void setVersion(const std::string& version);

    /*
     *  \func set
     *
     *  \brief Set the file header
     *
     *  Set the file header to its final state.
     *  Compute the offsets of the sections that
     *  depend on the file header size
     *
     *  \return Return size of header
     */
    size_t set();

    /*
     *  \func set
     *
     *  \brief Set the file header to its final state
     *
     *  Set the file header to its final state.
     *  Compute the offsets of the sections that depend
     *  on the file header size.
     *  Adds padding to modulo 8 for pvp block
     *
     *  \param xmlBlockSize Size of XML block
     *  \param supportBlockSize Size of support block
     *  \param pvpBlockSize Size of pvp block
     *  \param signalBlockSize Size of signal block
     *
     *  \return Return size of header
     */
    size_t set(ptrdiff_t xmlBlockSize, ptrdiff_t supportBlockSize,
            ptrdiff_t pvpBlockSize, ptrdiff_t signalBlockSize);

    //! Set required elements
    void setXMLBlockSize(ptrdiff_t size)
    {
        mXmlBlockSize = size;
    }
    void setPvpBlockSize(ptrdiff_t size)
    {
        mPvpBlockSize = size;
    }
    void setSignalBlockSize(ptrdiff_t size)
    {
        mSignalBlockSize = size;
    }
    void setXMLBlockByteOffset(ptrdiff_t offset)
    {
        mXmlBlockByteOffset = offset;
    }
    void setPvpBlockByteOffset(ptrdiff_t offset)
    {
        mPvpBlockByteOffset = offset;
    }
    void setSignalBlockByteOffset(ptrdiff_t offset)
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

    //! Get required elements
    ptrdiff_t getXMLBlockSize() const
    {
        return mXmlBlockSize;
    }
    ptrdiff_t getPvpBlockSize() const
    {
        return mPvpBlockSize;
    }
    ptrdiff_t getSignalBlockSize() const
    {
        return mSignalBlockSize;
    }
    ptrdiff_t getXMLBlockByteOffset() const
    {
        return mXmlBlockByteOffset;
    }
    ptrdiff_t getPvpBlockByteOffset() const
    {
        return mPvpBlockByteOffset;
    }
    ptrdiff_t getSignalBlockByteOffset() const
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

    /*
     *  \func getPvpPadBytes
     *  \brief Calculate padding in CPHD file for PVP block
     *
     *  Pad bytes don't include section terminator
     *
     *  \return number of pad bytes
     */
    ptrdiff_t getPvpPadBytes() const;

    //! Optional elements
    void setSupportBlockSize(ptrdiff_t size)
    {
        mSupportBlockSize = size;
    }
    void setSupportBlockByteOffset(ptrdiff_t offset)
    {
        mSupportBlockByteOffset = offset;
    }

    ptrdiff_t getSupportBlockSize() const
    {
        return mSupportBlockSize;
    }
    ptrdiff_t getSupportBlockByteOffset() const
    {
        return mSupportBlockByteOffset;
    }

private:
    friend std::ostream& operator<< (std::ostream& os, const FileHeader& fh);

private:
    // File type header
    std::string mVersion;

    // Required key-value pairs
    ptrdiff_t mXmlBlockSize;
    ptrdiff_t mXmlBlockByteOffset;
    ptrdiff_t mPvpBlockSize;
    ptrdiff_t mPvpBlockByteOffset;
    ptrdiff_t mSignalBlockSize;
    ptrdiff_t mSignalBlockByteOffset;
    std::string mClassification;
    std::string mReleaseInfo;

    // Optional key-value pairs
    ptrdiff_t mSupportBlockSize;
    ptrdiff_t mSupportBlockByteOffset;
};
}

#endif
