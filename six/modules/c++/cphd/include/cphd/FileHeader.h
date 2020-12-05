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
    size_t set(sys::Off_T xmlBlockSize, sys::Off_T supportBlockSize,
            sys::Off_T pvpBlockSize, sys::Off_T signalBlockSize);

    //! Set required elements
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

    //! Get required elements
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

    /*
     *  \func getPvpPadBytes
     *  \brief Calculate padding in CPHD file for PVP block
     *
     *  Pad bytes don't include section terminator
     *
     *  \return number of pad bytes
     */
    sys::Off_T getPvpPadBytes() const;

    //! Optional elements
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
