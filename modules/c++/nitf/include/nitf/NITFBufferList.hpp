/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_BUFFER_LIST_HPP__
#define __NITF_BUFFER_LIST_HPP__
#pragma once

#include <stddef.h>
#include <vector>
#include <std/span>
#include <std/cstddef> // std::byte

#include "nitf/coda-oss.hpp"
#include "nitf/System.hpp"
#include "nitf/exports.hpp"

namespace nitf
{
/*!
 * \class NITFBuffer
 * \brief Represents a pointer to raw NITF bytes and its length
 */
struct NITRO_NITFCPP_API NITFBuffer
{
    /*!
     * Initializes to an empty buffer
     */
    NITFBuffer() = default;

    /*!
     * Initializes to the specified pointer and size.  No copy is made and
     * this object does not take ownership.
     *
     * \param data The raw bytes of data
     * \param numBytes The number of bytes of contiguous data this
     * represents
     */
    NITFBuffer(const void* data, size_t numBytes) noexcept;

    const void* mData = nullptr;
    size_t mNumBytes = 0;

    std::span<const std::byte> getBytes() const noexcept
    {
        return sys::make_span(static_cast<const std::byte*>(mData), mNumBytes);
    }
};

/*!
 * \class NITFBufferList
 * \brief Represents a sequence of buffers which appear in contiguous order
 * in the NITF (the underlying pointers are not contiguous)
 */
struct NITRO_NITFCPP_API NITFBufferList
{
    //! The buffers
    std::vector<NITFBuffer> mBuffers;

    /*!
     * \return The total number of bytes across all the buffers
     */
    size_t getTotalNumBytes() const noexcept;

    /*!
     * \return Whether or not the buffer list is empty
     */
    bool empty() const noexcept
    {
        return mBuffers.empty();
    }

    /*!
     * Clear the buffers
     */
    void clear() noexcept
    {
        mBuffers.clear();
    }

    /*!
     * Push data onto the buffer list
     *
     * \param data The raw bytes
     * \param numBytes The number of bytes of data
     */
    void pushBack(const void* data, size_t numBytes)
    {
        mBuffers.push_back(NITFBuffer(data, numBytes));
    }

    /*!
     * Push data onto the buffer list
     *
     * \tparam DataT The type of data
     *
     * \param data The raw bytes
     */
    template <typename DataT>
    void pushBack(const std::vector<DataT>& data)
    {
        pushBack(data.empty() ? nullptr : data.data(),
                 data.size() * sizeof(DataT));
    }

    /*!
     * Get the number of blocks of data of size 'blockSize'.  In cases
     * where the block size is not an even multiple of the total number of
     * bytes, the last block will be larger than the block size (rather
     * than there being one more block which is smaller than the block
     * size).  This is intentional in order to make this easily usable with
     * Amazon's S3 storage with multipart uploads where there is a minimum
     * part size (there may be multiple machines, all with their portion
     * of the SICD, performing a multipart upload of their parts, and
     * only the last overall part of the object can be less than the
     * minimum part size).
     *
     * \param blockSize The desired block size
     *
     * \return The associated number of blocks
     */
    size_t getNumBlocks(size_t blockSize) const;

    /*!
     * Get the number of bytes in the specified block.  All blocks will be
     * the same size except for the last block (see getNumBlocks() for
     * details).
     *
     * \param blockSize The desired block size
     * \param blockIdx The 0-based block index
     *
     * \return The number of bytes in this block
     */
    size_t getNumBytesInBlock(size_t blockSize, size_t blockIdx) const;

    /*!
     * Returns a pointer to contiguous memory associated with the desired
     * block.  If this block lies entirely within a NITFBuffer, no copy
     * is performed.  Otherwise, the scratch buffer is resized, the bytes
     * are copied into this, and a pointer to the scratch buffer is
     * returned.
     *
     * \param blockSize The desired block size.  See getNumBlocks() for a
     * description on the behavior of the last block.
     * \param blockIdx The 0-based block index
     * \param[out] scratch Scratch buffer.  This will be resized and used
     * if the underlying memory for this block is not contiguous (i.e. it
     * spans NITFBuffers).
     * \param[out] numBytes The number of bytes in this block
     *
     * \return A pointer to contiguous memory associated with this block
     */
    const void* getBlock(size_t blockSize,
                         size_t blockIdx,
                         std::vector<sys::byte>& scratch,
                         size_t& numBytes) const;
    const void* getBlock(size_t blockSize,
                         size_t blockIdx,
                         std::vector<std::byte>& scratch,
                         size_t& numBytes) const;};
}

#endif
