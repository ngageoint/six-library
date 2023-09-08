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

#include <scene/sys_Conf.h>
#include <io/SeekableStreams.h>

#include "cphd/Types.h"

namespace cphd
{

/*
 *  \struct BaseFileHeader
 *
 *  \brief Base class for file header
 *
 *  Stores file header parsing information
 */
class BaseFileHeader
{
public:
    //! Type of file is CPHD
    static const char FILE_TYPE[];
    //! Key value pair delimiter ":="
    static const char KVP_DELIMITER[];
    //! Line delimiter "\n"
    static const char LINE_TERMINATOR;
    //! Section delimiter "\f"
    static const char SECTION_TERMINATOR;
    //! Max header size 10MB
    static const size_t MAX_HEADER_SIZE;

    virtual ~BaseFileHeader()
    {
    }

    /*
     *  \func readVersion
     *
     *  \brief Reads version of CPHD file
     *
     *  Given a Seekable input stream of a CPHD file, reads the version
     *  of the CPHD
     *
     *  \param inStream Input Stream of CPHD file
     *
     *  \return Verision string
     *
     *  \throws except::Exception if file is not CPHD
     */
    static
    std::string strReadVersion(io::SeekableInputStream& inStream);
    static Version readVersion(io::SeekableInputStream& inStream);

    /*
     *  \func read
     *
     *  \brief Read header
     *
     *  Given a Seekable input stream of a CPHD file, reads
     *  header information
     *
     *  \param inStream Input Stream of CPHD file
     *
     *  \throws except::Exception if expected header entry is not found
     *  \throws except::Exception if any header info is missing or empty
     */
    virtual void readImpl(io::SeekableInputStream& inStream) = 0;

    /*
     *  \func toString
     *
     *  \brief Convert header to string
     *
     *  For writing to file
     *  Does not include section terminator string
     */
    virtual std::string toString() const = 0;

    /*
     *  \func size
     *
     *  \brief Size of string
     *
     *  Counts size of header string (not including section terminator)
     *
     *  \return Size of header
     */
    size_t size() const
    {
        return toString().size();
    }

protected:
    /*
     *  \func isCPHD
     *
     *  \brief Checks if file stream is CPHD
     *
     *  \return true if file version is a CPHD,
     *  false otherwise
     */
    static
    bool isCPHD(io::SeekableInputStream& inStream);

    /*
     *  Typedef of header key value pairs
     */
    typedef std::pair<std::string, std::string> KeyValuePair;

    /*
     *  \func tokenize
     *
     *  \brief Parse header
     *
     *  Split header into key value pairs at specified delimiter
     */
    static
    void tokenize(const std::string& in,
                  const std::string& delimiter,
                  KeyValuePair& kvPair);

    /*
     *  \func tokenize
     *
     *  \brief Parse header
     *
     *  Split header into key value pairs at specified delimiter
     *
     *  \return Returns typedef KeyValuePair type
     */
    static
    KeyValuePair tokenize(const std::string& in, const std::string& delimiter)
    {
        KeyValuePair kvPair;
        tokenize(in, delimiter, kvPair);
        return kvPair;
    }

    /*
     *  \func blockReadHeader
     *
     *  \brief Block read header
     */
    void blockReadHeader(io::SeekableInputStream& inStream,
                         size_t blockSize,
                         std::string& headerBlock);
};
}

#endif
