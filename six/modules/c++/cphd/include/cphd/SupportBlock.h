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
#include <mem/SharedPtr.h>
#include <mem/BufferView.h>

#include <cphd/Data.h>
#include <cphd/Utilities.h>

namespace cphd
{

//  This class contains information about the SupportBlock CPHD data.
//  It contains the cphd::Data structure.
//
//  Due to the large size of CPHD SupportBlock, this object does not contain
//  any actual SupportBlock data
class SupportBlock
{
public:
    static const std::string ALL;
    /*
     * \param pathname Input CPHD pathname
     * \param data Data section from CPHD
     * \param startSupport CPHD header keyword "SUPPORT_BLOCK_BYTE_OFFSET"
     * \param sizeSupport CPHD header keyword "SUPPORT_BLOCK_SIZE"
     */
    SupportBlock(const std::string& pathname,
                 const cphd::Data& data,
                 sys::Off_T startSupport,
                 sys::Off_T sizeSupport);

    /*
     * \param inStream Input stream to an already opened CPHD file
     * \param data Data section from CPHD
     * \param startSupport CPHD header keyword "SUPPORT_BLOCK_BYTE_OFFSET"
     * \param sizeSupport CPHD header keyword "SUPPORT_BLOCK_SIZE"
     */
    SupportBlock(mem::SharedPtr<io::SeekableInputStream> inStream,
                 const cphd::Data& data,
                 sys::Off_T startSupport,
                 sys::Off_T sizeSupport);

    /*
     * Return offset from start of CPHD file for a support array with
     * specified id
     * \param id unique id of support array
     */
    sys::Off_T getFileOffset(const std::string& id) const;

    /*
     * Read the specified support array
     * Performs endian swapping if necessary
     *
     * \param id unique identifier of support array
     * \param numThreads Number of threads to use for endian swapping if
     * necessary
     * \param data Will contain the read in data.  Throws if buffer has not
     * been allocated to a sufficient size
     * (numRows * numCols * bytesPerElement)
     */
    void read(const std::string& id,
              size_t numThreads,
              const mem::BufferView<sys::ubyte>& data) const;

    // Same as above but allocates the memory
    void read(const std::string& id,
              size_t numThreads,
              mem::ScopedArray<sys::ubyte>& data) const;

    //! Reads all the support Arrays into data
    void readAll(size_t numThreads,
                 mem::ScopedArray<sys::ubyte>& data) const;

private:
    // Initialize mOffsets for each array
    // both for uncompressed and compressed data
    void initialize();

private:
    // Noncopyable
    SupportBlock(const SupportBlock& );
    const SupportBlock& operator=(const SupportBlock& );

private:
    const mem::SharedPtr<io::SeekableInputStream> mInStream;
    cphd::Data mData;
    const sys::Off_T mSupportOffset;       // offset in bytes to start of SupportBlock
    const size_t mSupportSize;             // total size in bytes of SupportBlock
    std::unordered_map<std::string,sys::Off_T> mOffsets; // Offset to start of each support array

    friend std::ostream& operator<< (std::ostream& os, const SupportBlock& d);
};
}

#endif
