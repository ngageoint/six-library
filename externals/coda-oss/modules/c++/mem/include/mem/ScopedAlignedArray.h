/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifndef __MEM_SCOPED_ALIGNED_ARRAY_H__
#define __MEM_SCOPED_ALIGNED_ARRAY_H__

#include <cstddef>

#include <sys/Conf.h>

namespace mem
{
    /*!
     *  \class ScopedAlignedArray
     *  \brief This class provides RAII for alignedAlloc() and alignedFree()
     */
    template <class T>
    struct ScopedAlignedArray
    {
        typedef T ElementType;

        explicit ScopedAlignedArray(
            size_t numElements = 0,
            size_t alignment = sys::SSE_INSTRUCTION_ALIGNMENT) :
            mArray(allocate(numElements, alignment))
        {
        }

        ~ScopedAlignedArray()
        {
            if (mArray)
            {
                // Don't expect sys::alignedFree() would ever throw, but just
                // in case...
                try
                {
                    sys::alignedFree(mArray);
                }
                catch (...)
                {
                }
            }
        }

        void reset(size_t numElements = 0, 
                   size_t alignment = sys::SSE_INSTRUCTION_ALIGNMENT)
        {
            if (mArray)
            {
                sys::alignedFree(mArray);
                mArray = nullptr;
            }

            mArray = allocate(numElements, alignment);
        }

        T& operator[](std::ptrdiff_t idx) const
        {
            return mArray[idx];
        }

        T* get() const
        {
            return mArray;
        }

        T* release()
        {
            T* const array = mArray;
            mArray = nullptr;
            return array;
        }

        ScopedAlignedArray(const ScopedAlignedArray&) = delete;
        ScopedAlignedArray& operator=(const ScopedAlignedArray&) = delete;

    private:
        static
        T* allocate(size_t numElements, size_t alignment)
        {
            if (numElements > 0)
            {
                const size_t numBytes(numElements * sizeof(T));
                return static_cast<T *>(sys::alignedAlloc(numBytes, 
                                                          alignment));
            }
            else
            {
                return nullptr;
            }
        }

    private:
        T* mArray;
    };
}

#endif
