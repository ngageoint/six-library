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
#ifndef __CPHD_SUPPORT_BLOCK_H__
#define __CPHD_SUPPORT_BLOCK_H__

#include <iostream>
#include <string>
#include <complex>
#include <unordered_map>

#include <sys/Conf.h>
#include <io/SeekableStreams.h>
#include <types/RowCol.h>

#include <mem/ScopedArray.h>
#include <mem/BufferView.h>

#include <cphd/Data.h>
#include <cphd/Utilities.h>

namespace cphd
{

/*
 *  \struct SupportBlock
 *
 *  \brief This class contains information about the SupportBlock CPHD data.
 */
//  Provides methods to read support block data from CPHD file/stream
class SupportBlock
{
public:
    /*
     *  \func SupportBlock
     *
     *  \brief Constructor initializes book keeping information
     *
     *  \param pathname Input CPHD pathname to initialize a file input stream
     *  \param data Data section from CPHD
     *  \param startSupport CPHD header keyword "SUPPORT_BLOCK_BYTE_OFFSET"
     *  \param sizeSupport CPHD header keyword "SUPPORT_BLOCK_SIZE"
     */
    SupportBlock(const std::string& pathname,
                 const cphd::Data& data,
                 int64_t startSupport,
                 int64_t sizeSupport);

    /*
     *  \func SupportBlock
     *
     *  \brief Constructor initializes book keeping information
     *
     *  \param inStream Input stream to an already opened CPHD file
     *  \param data Data section from CPHD
     *  \param startSupport CPHD header keyword "SUPPORT_BLOCK_BYTE_OFFSET"
     *  \param sizeSupport CPHD header keyword "SUPPORT_BLOCK_SIZE"
     */
    SupportBlock(std::shared_ptr<io::SeekableInputStream> inStream,
                 const cphd::Data& data,
                 int64_t startSupport,
                 int64_t sizeSupport);

    /*
     *  \func getFileOffset
     *
     *  \brief Get the byte offset of a specific support array in the CPHD file
     *
     *  \param id Unique id of support array
     *
     *  \return Returns offset from start of CPHD file
     */
    int64_t getFileOffset(const std::string& id) const;

    /*
     *  \func read
     *
     *  \brief Read the specified support array.
     *
     *   Performs endian swapping if necessary
     *
     *  \param id unique identifier of support array
     *  \param numThreads Number of threads to use for endian swapping if
     *   necessary
     *  \param[in,out] data A pre allocated mem::BufferView that will hold the data
     *   read from the file.
     *
     *  \throws except::Exception Throws if buffer has not been allocated to a sufficient size
     *   (numRows *  numCols *  bytesPerElement)
     */
    void read(const std::string& id,
              size_t numThreads,
              const mem::BufferView<std::byte>& data) const;

    /*
     *  \func read
     *
     *  \brief Read the specified support array
     *
     *   Performs endian swapping if necessary
     *
     *  \param id unique identifier of support array
     *  \param numThreads Number of threads to use for endian swapping if
     *   necessary
     *  \param[out] data std::unique_ptr<[]> that will hold the data read from the file.
     */
    // Same as above but allocates the memory
    void read(const std::string& id,
              size_t numThreads,
              std::unique_ptr<std::byte[]>& data) const;

    /*
     *  \func readAll
     *
     *  \brief Read all the support arrays
     *
     *   Performs endian swapping if necessary
     *
     *  \param numThreads Number of threads to use for endian swapping if
     *   necessary
     *  \param[out] data std::unique_ptr<[]> that will hold the data read from the file.
     *
     */
    void readAll(size_t numThreads,
                 std::unique_ptr<std::byte[]>& data) const;

private:
    //! Initialize mOffsets for each array
    // both for uncompressed and compressed data
    void initialize();

private:
    // Noncopyable
    SupportBlock(const SupportBlock& ) = delete;
    const SupportBlock& operator=(const SupportBlock& ) = delete;

private:
    const std::shared_ptr<io::SeekableInputStream> mInStream;
    cphd::Data mData;
    const int64_t mSupportOffset;       // offset in bytes to start of SupportBlock
    const size_t mSupportSize;             // total size in bytes of SupportBlock
    std::unordered_map<std::string,int64_t> mOffsets; // Offset to start of each support array

    friend std::ostream& operator<< (std::ostream& os, const SupportBlock& d);
};
}

#endif
