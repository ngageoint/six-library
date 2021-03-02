/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
 *
 * mem-c++ is free software; you can redistribute it and/or modify
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

#ifndef __MEM_SCRATCH_MEMORY_H__
#define __MEM_SCRATCH_MEMORY_H__

#include <stddef.h>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <except/Exception.h>
#include <mem/BufferView.h>
#include <sys/Conf.h>

namespace mem
{
/*!
 *  \class ScratchMemory
 *  \brief Handle reservation of scratch memory segments within a single buffer.
 *
 *  Memory segments should be reserved during a "setup" phase using the put
 *  method. Once all segments are reserved, call the setup method to set up
 *  the underlying memory and ensure the alignment requirements of each segment.
 *  The get method may be used afterwards to obtain pointers to the memory
 *  segments.
 */
class ScratchMemory
{
public:
    //! Default constructor
    ScratchMemory();

    /*!
     * \brief Reserve a buffer segment within this scratch memory buffer.
     *
     * \param key Identifier for scratch segment
     * \param numElements Size of scratch buffer
     * \param numBuffers Number of distinct buffers to set up. Defaults to 1.
     * \param alignment Number of bytes to align segment pointer. Defaults to
     *                  sys::SSE_INSTRUCTION_ALIGNMENT.
     *
     * \throws except::Exception if the given key has already been used
     */
    template <typename T>
    void put(const std::string& key,
             size_t numElements,
             size_t numBuffers = 1,
             size_t alignment = sys::SSE_INSTRUCTION_ALIGNMENT);

    /*!
     * \brief Release a segment so that that memory may be reused
     *
     * \param key Identifier for scratch segment
     */
    void release(const std::string& key);

    /*!
     * \brief Get pointer to buffer segment.
     *
     * \param key Identifier for scratch segment
     * \param indexBuffer Index of distinct buffer. Defaults to 0.
     *
     * \return Pointer to buffer segment
     *
     * \throws except::Exception if the scratch memory has not been set up,
     *         the key does not exist, or index of buffer is out of bounds
     */
    template <typename T>
    T* get(const std::string& key, size_t indexBuffer = 0);

    /*!
     * \brief Get const pointer to buffer segment.
     *
     * \param key Identifier for scratch segment
     * \param indexBuffer Index of distinct buffer. Defaults to 0.
     *
     * \return Const pointer to buffer segment
     *
     * \throws except::Exception if the scratch memory has not been set up,
     *         the key does not exist, or index of buffer is out of bounds
     */
    template <typename T>
    const T* get(const std::string& key, size_t indexBuffer = 0) const;

    /*!
     * \brief Get buffer view of buffer segment.
     *
     * \param key Identifier for scratch segment
     * \param indexBuffer Index of distinct buffer. Defaults to 0.
     *
     * \return Buffer view of buffer segment
     *
     * \throws except::Exception if the scratch memory has not been set up,
     *         the key does not exist, or index of buffer is out of bounds
     */
    template <typename T>
    BufferView<T> getBufferView(const std::string& key, size_t indexBuffer = 0);

    /*!
     * \brief Get const buffer view of buffer segment.
     *
     * \param key Identifier for scratch segment
     * \param indexBuffer Index of distinct buffer. Defaults to 0.
     *
     * \return Const buffer view of buffer segment
     *
     * \throws except::Exception if the scratch memory has not been set up,
     *         the key does not exist, or index of buffer is out of bounds
     */
    template <typename T>
    BufferView<const T> getBufferView(const std::string& key,
                                      size_t indexBuffer = 0) const;

    /*!
     * \brief Ensure underlying memory is properly set up and position segment
     *        pointers.
     *
     * \param scratchBuffer Storage to use for scratch memory. If buffer of
     *        size 0 is passed, memory is allocated internally. Defaults to
     *        an empty buffer.
     *
     * \throws except::Exception if the scratchBuffer passed in is too small
     *         to hold the requested scratch memory or has size > 0 with null
     *         data pointer
     */
    void setup(const BufferView<sys::ubyte>& scratchBuffer =
            BufferView<sys::ubyte>());

    /*!
     * \brief Get number of bytes needed to store scratch memory, including the
     *        maximum possible alignment overhead.
     */
    size_t getNumBytes() const
    {
        return mNumBytesNeeded;
    }

    ScratchMemory(const ScratchMemory&) = delete;
    ScratchMemory& operator=(const ScratchMemory&) = delete;

private:
    struct Segment
    {
        Segment(size_t numBytes, size_t numBuffers, size_t alignment, size_t offset);

        size_t numBytes;
        size_t numBuffers;
        size_t alignment;
        size_t offset;
        std::vector<sys::ubyte*> buffers;
    };

    const Segment& lookupSegment(const std::string& key,
                                 size_t indexBuffer) const;

    std::map<std::string, Segment> mSegments;
    std::vector<sys::ubyte> mStorage;
    std::vector<std::string> mKeyOrder;
    std::set<std::string> mReleasedKeys;
    std::set<std::string> mConnectedKeys;

    BufferView<sys::ubyte> mBuffer;
    size_t mNumBytesNeeded;
    size_t mOffset;
};
}

#include <mem/ScratchMemory.hpp>

#endif
