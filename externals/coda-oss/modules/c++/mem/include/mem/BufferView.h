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

#ifndef __MEM_BUFFER_VIEW_H__
#define __MEM_BUFFER_VIEW_H__

#include <vector>
#include <sys/Conf.h>
#include <sstream>
#include <except/Exception.h>
#include <stddef.h>

namespace mem
{
/*!
 *  \struct BufferView
 *  \brief Simple utility to allow allocation and splitting of work buffers
 *
 *  This class just represents views of the same buffer.  This buffer can
 *  be split into sub-views as desired, which have the same interface.
 *
 *  This is really just to simplify interfaces and make it easy to manipulate
 *  a small set of memory pools
 *
 */
template <typename T>
struct BufferView
{
    explicit BufferView(T* buffer = nullptr, size_t bufferSize = 0) :
        data(buffer), size(bufferSize)
    {
    }

    T* data;
    size_t size;

    /**
      Returns a new bufferView that "takes" sectionSize T from the bufferView
      For instance, if the current bufferView had 100 ints, and you called 
      section with sectionSize = 10, section() would return a new BufferView
      of size 10 and the original BufferView would be left with 90 ints.

      @sectionSize size of the new section
    
      @throws except::Exception if there is not enough space 

     */
    mem::BufferView<T> section(size_t sectionSize)
    {
        if(size < sectionSize)
        {   
            std::ostringstream oss;

            oss << "BufferView::section() called with sectionSize: " << sectionSize << " when";
            oss << " there were only " << size << " elements in the BufferView";
            
            throw except::Exception(Ctxt(oss.str()));
        } 

        mem::BufferView<T> newSection(data, sectionSize);

        size -= sectionSize;
        data += sectionSize;

        return newSection;
    }

    /**
      returns a vector of as many n-sized BufferViews as possible
      the last BufferView will have the remainder (size % n) elements if
      size % n != 0

      @n target size of each BufferView fragment

     */
    std::vector<BufferView> split(size_t n) const
    {
        const size_t newSize = size / n;
        const size_t lastSize = size - (n - 1) * newSize;
        T *head = data;

        std::vector<BufferView> buffers(n); 
        for (size_t ii = 0, last_ii = n - 1; ii < n; ++ii, head += newSize)
        {
            buffers[ii] = BufferView(head, (ii == last_ii) ? lastSize : newSize);
        }             
        return buffers;
    }

};

}

#endif
