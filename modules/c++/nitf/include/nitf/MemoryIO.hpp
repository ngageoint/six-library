/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifndef __NITF_MEMORY_IO_HPP__
#define __NITF_MEMORY_IO_HPP__

#include "nitf/NITFException.hpp"
#include "nitf/System.hpp"
#include "nitf/IOInterface.hpp"

/*!
 * \file MemoryIO.hpp
 * \brief Contains wrapper implementation for BufferAdapter
 */

namespace nitf
{

/*!
 *  \class MemoryIO
 *  \brief The C++ wrapper of the nitf_BufferAdapter
 */
class MemoryIO : public IOInterface
{
public:
    MemoryIO(size_t capacity) throw(nitf::NITFException);

    // If adopt is true, the memory will be deallocated via NRT_FREE(), so it
    // must be allocated via NRT_MALLOC() (not new[]).  If adopt is false, the
    // allocation method does not matter.
    MemoryIO(void* buffer, size_t size, bool adopt = false)
            throw(nitf::NITFException);

private:
    static
    nitf_IOInterface* create(void* buffer,
                             size_t size,
                             bool adopt) throw(nitf::NITFException);
};

}
#endif
