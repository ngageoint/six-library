/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2013 - 2014, MDA Information Systems LLC
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
 
#ifndef __MEM_SWAP_BUFFER_H__
#define __MEM_SWAP_BUFFER_H__

#include <algorithm>
#include <sys/Conf.h>
#include <mem/ScopedAlignedArray.h>

namespace mem
{

/*!
 *  \class SwapBuffer
 *  \brief Buffer for ping/pong processing
 *
 *  This buffer allows for users to both create data internally
 *  by specifying the number of bytes need per buffer, or the user
 *  can pass in externally created buffers. However, it's the user's
 *  responsibility to deallocate any externally created memory.
 *
 *  NOTE: This class was intentionally created without a reset method
 *        because losing track of the state of allocation on the 
 *        current memory could result in segmentation faults. 
 *        Specifically, if a user sends in data pointers, then uses 
 *        a reset method, they could be tempted to delete the pointers
 *        returned from the get* methods, which would segfault when 
 *        the ScopedAlignedArrays go out of scope.
 */
struct SwapBuffer
{
    /*!
     *  Allocate the buffers to the size needed --
     *  It is suggested to allocate these buffers to the largest
     *  image segment size, because these buffers are reusable 
     *  during processing.
     *  This internally creates and manages the memory requested.
     */
    SwapBuffer(size_t numBytes,
               size_t alignment = sys::SSE_INSTRUCTION_ALIGNMENT) :
        mNumBytes(numBytes),
        mAlignedValid(mNumBytes, alignment),
        mAlignedScratch(mNumBytes, alignment),
        mValid(mAlignedValid.get()),
        mScratch(mAlignedScratch.get())
    {
    }

    /*!
     *  Pass in externally created memory --
     *  It is the responsibility of the user to deallocate any
     *  memory passed into this class.
     */
    SwapBuffer(void* valid, void* scratch, size_t numBytes) :
        mNumBytes(numBytes),
        mValid(valid),
        mScratch(scratch)
    {
    }

    SwapBuffer(const SwapBuffer&) = delete;
    SwapBuffer& operator=(const SwapBuffer&) = delete;
    
    //! Get the number of bytes
    size_t getNumBytes() const 
    { 
        return mNumBytes; 
    }

    //! Grab the currently active buffer.  Users should read data
    //  from this buffer during processing.
    template<typename T>
    T* getValidBuffer()
    {
        return static_cast<T*>(mValid);
    }

    //! Grab the currently active buffer.  Users should read data
    //  from this buffer during processing.
    template<typename T>
    const T* getValidBuffer() const
    {
        return static_cast<T*>(mValid);
    }

    //! Grab the currently inactive buffer.  Users should write
    //  data to this buffer during processing.
    template<typename T>
    T* getScratchBuffer()
    {
        return static_cast<T*>(mScratch);
    }

    //! Grab the currently inactive buffer.  Users should write
    //  data to this buffer during processing.
    template<typename T>
    const T* getScratchBuffer() const
    {
        return static_cast<T*>(mScratch);
    }

    //! Swap the buffers
    virtual void swap()
    {
        std::swap(mValid, mScratch);
    }

private:
    const size_t mNumBytes;

    const ScopedAlignedArray<sys::byte> mAlignedValid;
    const ScopedAlignedArray<sys::byte> mAlignedScratch;

    void* mValid;
    void* mScratch;
};

}

#endif
